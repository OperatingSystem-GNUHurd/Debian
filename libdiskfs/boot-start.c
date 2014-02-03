/*
   Copyright (C) 1993,94,95,96,97,98,99,2000,01,02,10,11
   	Free Software Foundation, Inc.

This file is part of the GNU Hurd.

The GNU Hurd is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

The GNU Hurd is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with the GNU Hurd; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* Written by Michael I. Bushnell.  */

#include "priv.h"
#include <stdio.h>
#include <hurd.h>
#include <hurd/fsys.h>
#include <hurd/exec.h>
#include <hurd/startup.h>
#include <hurd/paths.h>
#include <fcntl.h>
#include <device/device.h>
#include <sys/reboot.h>
#include <string.h>
#include <argz.h>
#include <error.h>
#include <pids.h>
#include "fsys_S.h"
#include "fsys_reply_U.h"

static mach_port_t diskfs_exec_ctl;
extern task_t diskfs_exec_server_task;
static task_t parent_task = MACH_PORT_NULL;

static pthread_mutex_t execstartlock;
static pthread_cond_t execstarted;

const char *diskfs_boot_init_program = _HURD_INIT;

static void start_execserver ();

char **diskfs_argv = 0;

static mach_port_t
get_console ()
{
  mach_port_t device_master, console;
  error_t err = get_privileged_ports (0, &device_master);

  if (err)
    return MACH_PORT_NULL;

  err = device_open (device_master, D_WRITE | D_READ, "console", &console);
  if (err)
    return MACH_PORT_NULL;

  return console;
}

/* Make sure we have the privileged ports.  */
void
_diskfs_boot_privports (void)
{
  assert (diskfs_boot_filesystem ());
  if (_hurd_host_priv == MACH_PORT_NULL)
    {
      /* We are the boot command run by the real bootstrap filesystem.
	 We get the privileged ports from it as init would.  */
      mach_port_t bootstrap;
      error_t err = task_get_bootstrap_port (mach_task_self (), &bootstrap);
      assert_perror (err);
      err = fsys_getpriv (bootstrap, &_hurd_host_priv, &_hurd_device_master,
			  &parent_task);
      mach_port_deallocate (mach_task_self (), bootstrap);
      assert_perror (err);
    }
}

/* Once diskfs_root_node is set, call this if we are a bootstrap
   filesystem.  */
void
diskfs_start_bootstrap ()
{
  mach_port_t root_pt, startup_pt, bootpt;
  retry_type retry;
  char pathbuf[1024];
  string_t retry_name;
  mach_port_t portarray[INIT_PORT_MAX];
  mach_port_t fdarray[3];	/* XXX */
  task_t newt;
  error_t err;
  char *exec_argv, *exec_env;
  const char *initname;
  size_t exec_argvlen, exec_envlen;
  struct port_info *bootinfo;
  struct protid *rootpi;
  struct peropen *rootpo;
  mach_port_t diskfs_exec;
  unsigned int init_lookups = 0;

  /* Create the port for current and root directory.  */
  err = diskfs_make_peropen (diskfs_root_node, O_READ | O_EXEC, 0,
			     &rootpo);
  assert_perror (err);

  err = diskfs_create_protid (rootpo, 0, &rootpi);
  assert_perror (err);

  /* Get us a send right to copy around.  */
  root_pt = ports_get_send_right (rootpi);
  ports_port_deref (rootpi);

  if (diskfs_exec_server_task == MACH_PORT_NULL)
    {
      /* We are the boot command run by the real bootstrap filesystem.
	 Our parent (the real bootstrap filesystem) provides us a root
	 directory where we look up /servers/exec like any non-bootstrap
	 filesystem would.  */
      assert (_hurd_ports);
      assert (_hurd_ports[INIT_PORT_CRDIR].port != MACH_PORT_NULL);
      diskfs_exec = file_name_lookup (_SERVERS_EXEC, 0, 0);
      if (diskfs_exec == MACH_PORT_NULL)
	error (1, errno, "%s", _SERVERS_EXEC);
      else
	{
#ifndef NDEBUG
	  /* Make sure this is really a port to another server.  */
	  struct port_info *pi = ports_lookup_port (diskfs_port_bucket,
						    diskfs_exec, 0);
	  assert (!pi);
#endif
	}

      /* Here we assume the parent has already printed:
	 	Hurd server bootstrap: bootfs[bootdev] exec ourfs
      */
      printf ("\nContinuing on new root filesystem %s:", diskfs_disk_name);
      fflush (stdout);
    }
  else
    {
      uid_t idlist[] = {0, 0, 0};
      file_t execnode;

      printf ("Hurd server bootstrap: %s[%s]",
	      program_invocation_short_name, diskfs_disk_name);
      fflush (stdout);

      /* Get the execserver going and wait for its fsys_startup */
      pthread_mutex_init (&execstartlock, NULL);
      pthread_cond_init (&execstarted, NULL);
      pthread_mutex_lock (&execstartlock);
      start_execserver ();
      pthread_cond_wait (&execstarted, &execstartlock);
      pthread_mutex_unlock (&execstartlock);
      assert (diskfs_exec_ctl != MACH_PORT_NULL);

      /* Contact the exec server.  */
      err = fsys_getroot (diskfs_exec_ctl, root_pt, MACH_MSG_TYPE_COPY_SEND,
			  idlist, 3, idlist, 3, 0,
			  &retry, retry_name, &diskfs_exec);
      assert_perror (err);
      assert (retry == FS_RETRY_NORMAL);
      assert (retry_name[0] == '\0');
      assert (diskfs_exec != MACH_PORT_NULL);

      /* Attempt to set the active translator for the exec server so that
	 filesystems other than the bootstrap can find it.  */
      err = dir_lookup (root_pt, _SERVERS_EXEC, O_NOTRANS, 0,
			&retry, pathbuf, &execnode);
      if (err)
	{
	  error (0, err, "cannot set translator on %s", _SERVERS_EXEC);
	  mach_port_deallocate (mach_task_self (), diskfs_exec_ctl);
	}
      else
	{
	  assert (retry == FS_RETRY_NORMAL);
	  assert (retry_name[0] == '\0');
	  assert (execnode != MACH_PORT_NULL);
	  err = file_set_translator (execnode, 0, FS_TRANS_SET, 0, 0, 0,
				     diskfs_exec_ctl, MACH_MSG_TYPE_COPY_SEND);
	  mach_port_deallocate (mach_task_self (), diskfs_exec_ctl);
	  mach_port_deallocate (mach_task_self (), execnode);
	  assert_perror (err);
	}
      diskfs_exec_ctl = MACH_PORT_NULL;	/* Not used after this.  */
    }

  /* Cache the exec server port for file_exec to use.  */
  _hurd_port_set (&_diskfs_exec_portcell, diskfs_exec);

  if (_diskfs_boot_command)
    {
      /* We have a boot command line to run instead of init.  */
      err = argz_create (_diskfs_boot_command, &exec_argv, &exec_argvlen);
      assert_perror (err);
      initname = exec_argv;
      while (*initname == '/')
	initname++;
    }
  else
    {
      /* Choose the name of the startup server to execute.  */
      initname = diskfs_boot_init_program;
      while (*initname == '/')
	initname++;

      exec_argvlen = asprintf (&exec_argv, "/%s%c", initname, '\0');
      assert (exec_argvlen != -1);
      err = argz_add_sep (&exec_argv, &exec_argvlen,
			  diskfs_boot_command_line, ' ');
      assert_perror (err);

      initname = exec_argv + 1;
    }

 lookup_init:
  err = dir_lookup (root_pt, initname, O_READ, 0,
		    &retry, pathbuf, &startup_pt);
  init_lookups++;
  if (err)
    {
      printf ("\nCannot find startup program `%s': %s\n",
	      initname, strerror (err));
      fflush (stdout);
      free (exec_argv);
      assert_perror (err);	/* XXX this won't reboot properly */
    }
  else if (retry == FS_RETRY_MAGICAL && pathbuf[0] == '/')
    {
      assert (sysconf (_SC_SYMLOOP_MAX) < 0 ||
	      init_lookups < sysconf (_SC_SYMLOOP_MAX));

      /* INITNAME is a symlink with an absolute target, so try again.  */
      initname = strdupa (pathbuf);
      goto lookup_init;
    }

  assert (retry == FS_RETRY_NORMAL);
  assert (pathbuf[0] == '\0');

  err = ports_create_port (diskfs_initboot_class, diskfs_port_bucket,
			   sizeof (struct port_info), &bootinfo);
  assert_perror (err);
  bootpt = ports_get_send_right (bootinfo);
  ports_port_deref (bootinfo);

  portarray[INIT_PORT_CRDIR] = root_pt;
  portarray[INIT_PORT_CWDIR] = root_pt;
  portarray[INIT_PORT_AUTH] = MACH_PORT_NULL;
  portarray[INIT_PORT_PROC] = MACH_PORT_NULL;
  portarray[INIT_PORT_CTTYID] = MACH_PORT_NULL;
  portarray[INIT_PORT_BOOTSTRAP] = bootpt;

  fdarray[0] = fdarray[1] = fdarray[2] = get_console (); /* XXX */

  err = argz_create (environ, &exec_env, &exec_envlen);
  assert_perror (err);

  err = task_create (mach_task_self (),
#ifdef KERN_INVALID_LEDGER
		     NULL, 0,	/* OSF Mach */
#endif
		     0, &newt);
  assert_perror (err);
  if (_diskfs_boot_pause)
    {
      printf ("pausing for %s...\n", exec_argv);
      getc (stdin);
    }
  printf (" %s", basename (exec_argv));
  fflush (stdout);
  err = exec_exec (diskfs_exec, startup_pt, MACH_MSG_TYPE_COPY_SEND,
		   newt, 0, exec_argv, exec_argvlen, exec_env, exec_envlen,
		   fdarray, MACH_MSG_TYPE_COPY_SEND, 3,
		   portarray, MACH_MSG_TYPE_COPY_SEND, INIT_PORT_MAX,
		   /* Supply no intarray, since we have no info for it.
		      With none supplied, it will use the defaults.  */
		   NULL, 0, 0, 0, 0, 0);
  free (exec_argv);
  free (exec_env);
  mach_port_deallocate (mach_task_self (), root_pt);
  mach_port_deallocate (mach_task_self (), startup_pt);
  mach_port_deallocate (mach_task_self (), bootpt);
  assert_perror (err);
}

/* We look like an execserver to the execserver itself; it makes this
   call (as does any task) to get its state.  We can't give it all of
   its ports (we'll provide those with a later call to exec_init).  */
kern_return_t
diskfs_S_exec_startup_get_info (mach_port_t port,
				vm_address_t *user_entry,
				vm_address_t *phdr_data,
				vm_size_t *phdr_size,
				vm_address_t *base_addr,
				vm_size_t *stack_size,
				int *flags,
				char **argvP,
				mach_msg_type_number_t *argvlen,
				char **envpP __attribute__ ((unused)),
				mach_msg_type_number_t *envplen,
				mach_port_t **dtableP,
				mach_msg_type_name_t *dtablepoly,
				mach_msg_type_number_t *dtablelen,
				mach_port_t **portarrayP,
				mach_msg_type_name_t *portarraypoly,
				mach_msg_type_number_t *portarraylen,
				int **intarrayP,
				mach_msg_type_number_t *intarraylen)
{
  error_t err;
  mach_port_t *portarray, *dtable;
  mach_port_t rootport;
  struct ufsport *upt;
  struct protid *rootpi;
  struct peropen *rootpo;

  if (!(upt = ports_lookup_port (diskfs_port_bucket, port,
				 diskfs_execboot_class)))
    return EOPNOTSUPP;

  *user_entry = 0;
  *phdr_data = *base_addr = 0;
  *phdr_size = *stack_size = 0;

  /* We have no args for it.  Tell it to look on its stack
     for the args placed there by the boot loader.  */
  *argvlen = *envplen = 0;
  *flags = EXEC_STACK_ARGS;

  if (*portarraylen < INIT_PORT_MAX)
    *portarrayP = mmap (0, INIT_PORT_MAX * sizeof (mach_port_t),
			PROT_READ|PROT_WRITE, MAP_ANON, 0, 0);
  portarray = *portarrayP;
  *portarraylen = INIT_PORT_MAX;

  if (*dtablelen < 3)
    *dtableP = mmap (0, 3 * sizeof (mach_port_t), PROT_READ|PROT_WRITE,
		     MAP_ANON, 0, 0);
  dtable = *dtableP;
  *dtablelen = 3;
  dtable[0] = dtable[1] = dtable[2] = get_console (); /* XXX */

  *intarrayP = NULL;
  *intarraylen = 0;

  err = diskfs_make_peropen (diskfs_root_node, O_READ | O_EXEC, 0, &rootpo);
  assert_perror (err);

  err = diskfs_create_protid (rootpo, 0, &rootpi);
  assert_perror (err);

  rootport = ports_get_right (rootpi);
  ports_port_deref (rootpi);
  portarray[INIT_PORT_CWDIR] = rootport;
  portarray[INIT_PORT_CRDIR] = rootport;
  portarray[INIT_PORT_AUTH] = MACH_PORT_NULL;
  portarray[INIT_PORT_PROC] = MACH_PORT_NULL;
  portarray[INIT_PORT_CTTYID] = MACH_PORT_NULL;
  portarray[INIT_PORT_BOOTSTRAP] = port; /* use the same port */

  *portarraypoly = MACH_MSG_TYPE_MAKE_SEND;

  *dtablepoly = MACH_MSG_TYPE_COPY_SEND;

  ports_port_deref (upt);
  return 0;
}

/* Called by S_fsys_startup for execserver bootstrap.  The execserver
   is able to function without a real node, hence this fraud.  */
error_t
diskfs_execboot_fsys_startup (mach_port_t port, int flags,
			      mach_port_t ctl,
			      mach_port_t *real,
			      mach_msg_type_name_t *realpoly)
{
  error_t err;
  string_t pathbuf;
  enum retry_type retry;
  struct port_info *pt;
  struct protid *rootpi;
  struct peropen *rootpo;
  mach_port_t rootport;

  if (!(pt = ports_lookup_port (diskfs_port_bucket, port,
				diskfs_execboot_class)))
    return EOPNOTSUPP;

  err = diskfs_make_peropen (diskfs_root_node, flags, 0, &rootpo);
  assert_perror (err);
  err = diskfs_create_protid (rootpo, 0, &rootpi);
  assert_perror (err);
  rootport = ports_get_send_right (rootpi);
  ports_port_deref (rootpi);

  err = dir_lookup (rootport, _SERVERS_EXEC, flags|O_NOTRANS, 0,
		    &retry, pathbuf, real);
  assert_perror (err);
  assert (retry == FS_RETRY_NORMAL);
  assert (pathbuf[0] == '\0');
  *realpoly = MACH_MSG_TYPE_MOVE_SEND;

  mach_port_deallocate (mach_task_self (), rootport);

  diskfs_exec_ctl = ctl;

  pthread_mutex_lock (&execstartlock);
  pthread_cond_signal (&execstarted);
  pthread_mutex_unlock (&execstartlock);
  ports_port_deref (pt);
  return 0;
}

/* Called by init to get the privileged ports as described
   in <hurd/fsys.defs>. */
kern_return_t
diskfs_S_fsys_getpriv (mach_port_t port,
		       mach_port_t reply, mach_msg_type_name_t reply_type,
		       mach_port_t *host_priv, mach_msg_type_name_t *hp_type,
		       mach_port_t *dev_master, mach_msg_type_name_t *dm_type,
		       mach_port_t *fstask, mach_msg_type_name_t *task_type)
{
  error_t err;
  struct port_info *init_bootstrap_port =
    ports_lookup_port (diskfs_port_bucket, port, diskfs_initboot_class);

  if (!init_bootstrap_port)
    return EOPNOTSUPP;

  err = get_privileged_ports (host_priv, dev_master);
  if (!err)
    {
      *fstask = mach_task_self ();
      *hp_type = *dm_type = MACH_MSG_TYPE_MOVE_SEND;
      *task_type = MACH_MSG_TYPE_COPY_SEND;
    }

  ports_port_deref (init_bootstrap_port);

  return err;
}

/* Called by init to give us ports to the procserver and authserver as
   described in <hurd/fsys.defs>. */
kern_return_t
diskfs_S_fsys_init (mach_port_t port,
		    mach_port_t reply, mach_msg_type_name_t replytype,
		    mach_port_t procserver,
		    mach_port_t authhandle)
{
  struct port_infe *pt;
  static int initdone = 0;
  mach_port_t host, startup;
  error_t err;
  mach_port_t root_pt;
  struct protid *rootpi;
  struct peropen *rootpo;

  pt = ports_lookup_port (diskfs_port_bucket, port, diskfs_initboot_class);
  if (!pt)
    return EOPNOTSUPP;
  ports_port_deref (pt);
  if (initdone)
    return EOPNOTSUPP;
  initdone = 1;

  /* init is single-threaded, so we must reply to its RPC before doing
     anything which might attempt to send an RPC to init.  */
  fsys_init_reply (reply, replytype, 0);

  /* Allocate our references here; _hurd_init will consume a reference
     for the library itself. */
  err = mach_port_mod_refs (mach_task_self (),
			    procserver, MACH_PORT_RIGHT_SEND, +1);
  assert_perror (err);
  err = mach_port_mod_refs (mach_task_self (),
			    authhandle, MACH_PORT_RIGHT_SEND, +1);
  assert_perror (err);

  if (diskfs_auth_server_port != MACH_PORT_NULL)
    mach_port_deallocate (mach_task_self (), diskfs_auth_server_port);
  diskfs_auth_server_port = authhandle;

  if (diskfs_exec_server_task != MACH_PORT_NULL)
    {
      process_t execprocess;
      err = proc_task2proc (procserver, diskfs_exec_server_task, &execprocess);
      assert_perror (err);

      /* Declare that the exec server is our child. */
      proc_child (procserver, diskfs_exec_server_task);
      proc_mark_exec (execprocess);

      /* Don't start this until now so that exec is fully authenticated
	 with proc. */
      HURD_PORT_USE (&_diskfs_exec_portcell,
		     exec_init (port, authhandle,
				execprocess, MACH_MSG_TYPE_COPY_SEND));
      mach_port_deallocate (mach_task_self (), execprocess);

      /* We don't need this anymore. */
      mach_port_deallocate (mach_task_self (), diskfs_exec_server_task);
      diskfs_exec_server_task = MACH_PORT_NULL;
    }
  else
    {
      mach_port_t bootstrap;
      process_t parent_proc;

      assert (parent_task != MACH_PORT_NULL);

      /* Tell the proc server that our parent task is our child.  This
	 makes the process hierarchy fail to represent the real order of
	 who created whom, but it sets the owner and authentication ids to
	 root.  It doesn't really matter that the parent fs task be
	 authenticated, but the exec server needs to be authenticated to
	 complete the boot handshakes with init.  The exec server gets its
	 privilege by the parent fs doing proc_child (code above) after
	 we send it fsys_init (below).  */

      err = proc_child (procserver, parent_task);
      assert_perror (err);

      /* Get the parent's proc server port so we can send it in the fsys_init
	 RPC just as init would.  */
      err = proc_task2proc (procserver, parent_task, &parent_proc);
      assert_perror (err);

      /* We don't need this anymore. */
      mach_port_deallocate (mach_task_self (), parent_task);
      parent_task = MACH_PORT_NULL;

      proc_mark_exec (parent_proc);

      /* Give our parent (the real bootstrap filesystem) an fsys_init
	 RPC of its own, as init would have sent it.  */
      err = task_get_bootstrap_port (mach_task_self (), &bootstrap);
      assert_perror (err);
      err = fsys_init (bootstrap, parent_proc, MACH_MSG_TYPE_COPY_SEND,
		       authhandle);
      mach_port_deallocate (mach_task_self (), parent_proc);
      mach_port_deallocate (mach_task_self (), bootstrap);
      assert_perror (err);
    }

  /* Get a port to the root directory to put in the library's
     data structures.  */
  err = diskfs_make_peropen (diskfs_root_node, O_READ|O_EXEC, 0, &rootpo);
  assert_perror (err);
  err = diskfs_create_protid (rootpo, 0, &rootpi);
  assert_perror (err);
  root_pt = ports_get_send_right (rootpi);
  ports_port_deref (rootpi);

  /* We need two send rights, for the crdir and cwdir slots.  */
  mach_port_mod_refs (mach_task_self (), root_pt,
		      MACH_PORT_RIGHT_SEND, +1);

  if (_hurd_ports)
    {
      /* We already have a portarray, because somebody responded to
	 exec_startup on our initial bootstrap port, even though we are
	 supposedly the bootstrap program.  The losing `boot' that runs on
	 UX does this.  */
      _hurd_port_set (&_hurd_ports[INIT_PORT_PROC], procserver); /* Consume. */
      _hurd_port_set (&_hurd_ports[INIT_PORT_AUTH], authhandle); /* Consume. */
      _hurd_port_set (&_hurd_ports[INIT_PORT_CRDIR], root_pt); /* Consume. */
      _hurd_port_set (&_hurd_ports[INIT_PORT_CWDIR], root_pt); /* Consume. */
      _hurd_proc_init (diskfs_argv, NULL, 0);
    }
  else
    {
      /* We have no portarray or intarray because there was
	 no exec_startup data; _hurd_init was never called.
	 We now have the crucial ports, so create a portarray
	 and call _hurd_init.  */
      mach_port_t *portarray;
      unsigned int i;
      portarray = mmap (0, INIT_PORT_MAX * sizeof *portarray,
			PROT_READ|PROT_WRITE, MAP_ANON, 0, 0);
      if (MACH_PORT_NULL != (mach_port_t) 0)
	for (i = 0; i < INIT_PORT_MAX; ++i)
	  portarray[i] = MACH_PORT_NULL;
      portarray[INIT_PORT_PROC] = procserver;
      portarray[INIT_PORT_AUTH] = authhandle;
      portarray[INIT_PORT_CRDIR] = root_pt;
      portarray[INIT_PORT_CWDIR] = root_pt;
      _hurd_init (0, diskfs_argv, portarray, INIT_PORT_MAX, NULL, 0);
    }

  err = get_privileged_ports (&host, 0);
  if (err)
    return err;

  proc_register_version (procserver, host, diskfs_server_name, "",
			 diskfs_server_version);

  err = proc_getmsgport (procserver, HURD_PID_STARTUP, &startup);
  if (!err)
    {
      startup_essential_task (startup, mach_task_self (), MACH_PORT_NULL,
			      diskfs_server_name, host);
      mach_port_deallocate (mach_task_self (), startup);
    }

  mach_port_deallocate (mach_task_self (), host);
  mach_port_deallocate (mach_task_self (), procserver);

  _diskfs_init_completed ();

  return MIG_NO_REPLY;		/* Already replied above.  */
}

/* Start the execserver running (when we are a bootstrap filesystem).  */
static void
start_execserver (void)
{
  error_t err;
  mach_port_t right;
  extern task_t diskfs_exec_server_task; /* Set in opts-std-startup.c.  */
  struct port_info *execboot_info;

  assert (diskfs_exec_server_task != MACH_PORT_NULL);

  err = ports_create_port (diskfs_execboot_class, diskfs_port_bucket,
			   sizeof (struct port_info), &execboot_info);
  assert_perror (err);
  right = ports_get_send_right (execboot_info);
  ports_port_deref (execboot_info);
  task_set_special_port (diskfs_exec_server_task, TASK_BOOTSTRAP_PORT, right);
  mach_port_deallocate (mach_task_self (), right);

  if (_diskfs_boot_pause)
    {
      printf ("pausing for exec\n");
      getc (stdin);
    }
  task_resume (diskfs_exec_server_task);

  printf (" exec");
  fflush (stdout);
}
