/*
   Copyright (C) 1995, 1996, 1999, 2000, 2002, 2004, 2010
     Free Software Foundation, Inc.

   Written by Miles Bader and Michael I. Bushnell.

   This file is part of the GNU Hurd.

   The GNU Hurd is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2, or (at
   your option) any later version.

   The GNU Hurd is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. */

#include <hurd.h>
#include <mach/notify.h>
#include <errno.h>
#include <fcntl.h>
#include <assert.h>
#include "fshelp.h"


error_t
fshelp_start_translator_long (fshelp_open_fn_t underlying_open_fn,
			      void *cookie, char *name, char *argz,
			      int argz_len, mach_port_t *fds,
			      mach_msg_type_name_t fds_type, int fds_len,
			      mach_port_t *ports,
			      mach_msg_type_name_t ports_type, int ports_len,
			      int *ints, int ints_len,
			      uid_t owner_uid,
			      int timeout, fsys_t *control)
{
  error_t err;
  file_t executable;
  mach_port_t bootstrap = MACH_PORT_NULL;
  mach_port_t task = MACH_PORT_NULL;
  mach_port_t prev_notify, proc, saveport, childproc;
  int ports_moved = 0;

  /* Find the translator itself.  Since argz has zero-separated elements, we
     can use it as a normal string representing the first element.  */
  executable = file_name_lookup(name, O_EXEC, 0);
  if (executable == MACH_PORT_NULL)
    return errno;

  /* Create a bootstrap port for the translator.  */
  err =
    mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &bootstrap);
  if (err)
    goto lose;

  /* Create the task for the translator.  */
  err = task_create (mach_task_self (),
#ifdef KERN_INVALID_LEDGER
		     NULL, 0,	/* OSF Mach */
#endif
		     0, &task);
  if (err)
    goto lose;

  /* XXX 25 is BASEPRI_USER, which isn't exported by the kernel.  Ideally,
     nice values should be used, perhaps with a simple wrapper to convert
     them to Mach priorities.  */
  err = task_priority(task, 25, FALSE);

  if (err)
    goto lose_task;

  /* Designate TASK as our child and set it's owner accordingly. */
  proc = getproc ();
  proc_child (proc, task);
  err = proc_task2proc (proc, task, &childproc);
  mach_port_deallocate (mach_task_self (), proc);
  if (err)
    goto lose_task;
  err = proc_setowner (childproc, owner_uid, owner_uid == (uid_t) -1);
  mach_port_deallocate (mach_task_self (), childproc);
  if (err)
    goto lose_task;

  assert (ports_len > INIT_PORT_BOOTSTRAP);
  switch (ports_type)
    {
    case MACH_MSG_TYPE_MAKE_SEND:
    case MACH_MSG_TYPE_MAKE_SEND_ONCE:
      break;

    case MACH_MSG_TYPE_MOVE_SEND:
      if (ports[INIT_PORT_BOOTSTRAP] != MACH_PORT_NULL)
	mach_port_deallocate (mach_task_self (), ports[INIT_PORT_BOOTSTRAP]);
      mach_port_insert_right (mach_task_self (), bootstrap, bootstrap,
			      MACH_MSG_TYPE_MAKE_SEND);
      break;

    case MACH_MSG_TYPE_COPY_SEND:
      mach_port_insert_right (mach_task_self (), bootstrap, bootstrap,
			      MACH_MSG_TYPE_MAKE_SEND);
      break;

    default:
      abort ();
    }

  saveport = ports[INIT_PORT_BOOTSTRAP];
  ports[INIT_PORT_BOOTSTRAP] = bootstrap;

  /* Try and exec the translator in TASK...  */
  err = file_exec (executable, task, EXEC_DEFAULTS,
		   argz, argz_len, 0, 0,
		   fds, fds_type, fds_len,
		   ports, ports_type, ports_len,
		   ints, ints_len, 0, 0, 0, 0);
  ports_moved = 1;

  if (ports_type == MACH_MSG_TYPE_COPY_SEND)
    mach_port_deallocate (mach_task_self (), bootstrap);
  ports[INIT_PORT_BOOTSTRAP] = saveport;

  if (err)
    goto lose_task;

  /* Ask to be told if TASK dies.  */
  err =
    mach_port_request_notification(mach_task_self(),
				   bootstrap, MACH_NOTIFY_NO_SENDERS, 0,
				   bootstrap, MACH_MSG_TYPE_MAKE_SEND_ONCE,
				   &prev_notify);
  if (err)
    goto lose_task;

  /* Ok, cool, we've got a running(?) program, now rendezvous with it if
     possible using the startup protocol on the bootstrap port... */
  err = fshelp_service_fsys_startup(underlying_open_fn, cookie, bootstrap,
				    timeout, task, control);

 lose_task:
  if (err)
    task_terminate (task);

 lose:
  if (!ports_moved)
    {
      int i;

      if (fds_type == MACH_MSG_TYPE_MOVE_SEND)
	for (i = 0; i < fds_len; i++)
	  mach_port_deallocate (mach_task_self (), fds[i]);
      if (ports_type == MACH_MSG_TYPE_MOVE_SEND)
	for (i = 0; i < ports_len; i++)
	  mach_port_deallocate (mach_task_self (), ports[i]);
    }
  if (bootstrap != MACH_PORT_NULL)
    mach_port_destroy(mach_task_self(), bootstrap);
  if (executable != MACH_PORT_NULL)
    mach_port_deallocate(mach_task_self(), executable);
  if (task != MACH_PORT_NULL)
    mach_port_deallocate(mach_task_self(), task);

  return err;
}
