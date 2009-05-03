/*    Copyright (C) 2008 Free Software Foundation, Inc.
      Written by Zheng Da.

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

#include <stdio.h>
#include <argp.h>
#include <error.h>
#include <syslog.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stddef.h>

#include <hurd.h>
#include <mach.h>

#include "proc_proxy.h"

struct port_bucket *proc_bucket;
struct port_class *proc_class;

mach_port_t master_port;

static struct hurd_ihash proc_ht
= HURD_IHASH_INITIALIZER (offsetof (struct vproc, p_task_hashloc));

static char *args_doc = "command line";
static char *doc = "Hurd proc proxy";
static struct argp_option options[] = 
{
    {"master-device", 'M', "FILE", 0, "Get the pseudo master device from a translator"},
    { 0 }
};

vpstruct_t
reqport_find (mach_port_t reqport)
{
  vpstruct_t p;
  p = ports_lookup_port (proc_bucket, reqport, proc_class);
  return p;
}

void
task_clean_routine (void *port)
{
  vpstruct_t proc = port;
  hurd_ihash_locp_remove (&proc_ht, proc->p_task_hashloc);
  mach_port_deallocate (mach_task_self (), proc->proc);
  mach_port_deallocate (mach_task_self (), proc->task_id);
}

vpstruct_t
create_task (task_t task)
{
  vpstruct_t proc;
  error_t err = ports_create_port (proc_class, proc_bucket,
				   sizeof (*proc), &proc);
  if (err)
    error (3, err, "ports_create_port");

  proc->task_id = task;

  hurd_ihash_add (&proc_ht, proc->task_id, proc);
  return proc;
}

vpstruct_t
find_task (task_t task)
{
  return hurd_ihash_find (&proc_ht, task) ? : create_task (task);
}

int
request_server (mach_msg_header_t *inp, mach_msg_header_t *outp)
{
  extern boolean_t process_server (mach_msg_header_t *, mach_msg_header_t *);
  return process_server (inp, outp);
}

void
print_cmd (int argc, char *argv[])
{
  int i;
  for (i = 0; i < argc; i++)
    {
      debug ("%s ", argv[i]);
    }
  debug ("\n");
}

static char **comm_argv;
static int comm_argc;

static int add_comm_arg (char *arg)
{
  static int comm_capacity = 0;
  if (comm_capacity <= comm_argc)
    {
      comm_capacity += 8;
      comm_argv = (char **) realloc (comm_argv,
				     comm_capacity * sizeof (char *));
      if (comm_argv == NULL)
	error (1, errno, "realloc");
    }

  comm_argv[comm_argc] = arg;
  comm_argc++;

  return 0;
}

static void add_comm_line (char *line)
{
  char *p = line;
  char *start;

  /* Skip the space */
  for (; *p != 0 && isspace (*p); p++);
  start = p;

  while (1)
    {
      /* Find the end of an option */
      for (; *p != 0 && !isspace (*p); p++);
      if (*p)
	{
	  *p = 0;
	  p++;
	}
      add_comm_arg (start);

      /* Find the beginning of an option */
      for (; *p != 0 && isspace (*p); p++);
      if (*p == 0)
	break;
      start = p;
    }
}

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  switch (key)
    {
    case 'M':
      master_port = file_name_lookup (arg, 0, 0);
      if (master_port == MACH_PORT_NULL)
	error (1, errno, "file_name_lookup");
      break;
    case ARGP_KEY_ARG:
      add_comm_line (arg);
      break;
    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}

int
main (int argc, char *argv[])
{
  error_t err;
  vpstruct_t child_proc;
  mach_port_t receive_port;
  mach_port_t proc_port;
  int pipe_fd1s[2];
  int pipe_fd2s[2];
  pid_t child_pid;
  task_t child_task;
  struct argp argp = 
    { options, parse_opt, args_doc, doc};

  argp_parse (&argp, argc, argv, 0, 0, 0);
  add_comm_arg (NULL);

  if (pipe (pipe_fd1s) < 0)
    error (2, errno, "pipe");
  if (pipe (pipe_fd2s) < 0)
    error (2, errno, "pipe");

  err = mach_port_allocate (mach_task_self (), MACH_PORT_RIGHT_DEAD_NAME,
			    &proc_port);
  if (err != KERN_SUCCESS)
    error (2, err, "mach_port_allocate");

  child_pid = fork ();
  if (child_pid < 0)
    error (2, errno, "fork");
  else if (child_pid == 0)
    {
      /* generate a new process group for child process,
       * it's needed to set the child process group to foreground */
      setpgid (getpid (), getpid ());

      close (pipe_fd1s[1]);
      close (pipe_fd2s[0]);

      debug ("child process starts\n");
      read (pipe_fd1s[0], &proc_port, sizeof (proc_port));
      print_cmd (comm_argc, comm_argv);
      write (pipe_fd2s[1], &proc_port, sizeof (proc_port));

      setproc (proc_port);
      debug ("child: current proc is %d\n", getproc ());
      if (execvp (comm_argv[0], comm_argv) < 0)
	error (2, errno, "execvp");
    }

  debug ("create a child process %d\n", child_pid);

  /* Set the child process group as the foreground group. */
  tcsetpgrp (fileno (stdout), child_pid);

  close (pipe_fd1s[0]);
  close (pipe_fd2s[1]);

  proc_bucket = ports_create_bucket ();
  proc_class = ports_create_class (task_clean_routine, 0);

  /* Make the send right to the proc porxy in the child process. */
  child_task = pid2task (child_pid);
  err = mach_port_destroy (child_task, proc_port);
  if (err != KERN_SUCCESS)
    error (2, err, "mach_port_destroy");
  child_proc = create_task (child_task);
  receive_port = ports_get_right (child_proc);
  err = mach_port_insert_right (child_task, proc_port, receive_port,
				MACH_MSG_TYPE_MAKE_SEND);
  if (err != KERN_SUCCESS)
    error (2, err, "mach_port_insert_right");

  /* Tell the child the send right. */
  write (pipe_fd1s[1], &proc_port, sizeof(proc_port));
  /* Synchronize. The child process should run first. */
  read (pipe_fd2s[0], &proc_port, sizeof(proc_port));

  debug ("the proxy starts\n");
  proc_task2proc (getproc(), child_task, &child_proc->proc);

  while (1)
    {
      int status;
      int ret;

      ports_manage_port_operations_one_thread (proc_bucket, 
					       request_server, 500);
      ret = waitpid (child_pid, &status, WNOHANG);
      if (ret < 0)
	error (1, errno, "waitpid");

      /* if the child process exits */
      if (ret == 1 && WIFEXITED (status))
	break;

    }
  debug ("proc proxy exits\n");

  return 0;
}

