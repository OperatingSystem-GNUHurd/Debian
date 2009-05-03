/* 
   Copyright (C) 2009 Free Software Foundation, Inc.
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

/* This file implements the server-side RPC functions of mach_host. */

#include <mach.h>
#include <hurd.h>
#include <string.h>
#include <assert.h>

#include "util.h"
#include "mach_proxy.h"

kern_return_t
S_mach_port_names (mach_port_t task, mach_port_array_t *names,
		   mach_msg_type_number_t *namesCnt,
		   mach_port_type_array_t *types,
		   mach_msg_type_number_t *typesCnt)
{
  struct task_info *task_pi;
  error_t err;

  task_pi = ports_lookup_port (port_bucket, task, task_portclass);
  if (task_pi == NULL)
    return EOPNOTSUPP;
  err = mach_port_names (task_pi->task_port,
			 names, namesCnt, types, typesCnt);
  debug ("%s", strerror (err));
  /* The deallocation bit is set,
   * so 'names' and 'types' will be deallocated after the reply is sent. */
  ports_port_deref (task_pi);
  return err;
}

/* Routine mach_port_type */
kern_return_t
S_mach_port_type (mach_port_t task, mach_port_t name, mach_port_type_t *ptype)
{
  struct task_info *task_pi;
  error_t err;

  task_pi = ports_lookup_port (port_bucket, task, task_portclass);
  if (task_pi == NULL)
    return EOPNOTSUPP;
  err = mach_port_type (task_pi->task_port, name, ptype);
  debug ("%s", strerror (err));
  ports_port_deref (task_pi);
  return err;
}

/* Routine mach_port_rename */
kern_return_t
S_mach_port_rename (mach_port_t task,
		    mach_port_t old_name, mach_port_t new_name)
{
  struct task_info *task_pi;
  error_t err;

  task_pi = ports_lookup_port (port_bucket, task, task_portclass);
  if (task_pi == NULL)
    return EOPNOTSUPP;
  /* old_name and new_name are just names,
   * so I don't need to deallocate them.
   * It should be the same for other RPCs that only deal with port names */
  err = mach_port_rename (task_pi->task_port, old_name, new_name);
  debug ("%s", strerror (err));
  ports_port_deref (task_pi);
  return err;
}

/* Routine mach_port_allocate_name */
kern_return_t
S_mach_port_allocate_name (mach_port_t task,
			   mach_port_right_t right, mach_port_t name)
{
  struct task_info *task_pi;
  error_t err;

  task_pi = ports_lookup_port (port_bucket, task, task_portclass);
  if (task_pi == NULL)
    return EOPNOTSUPP;
  err = mach_port_allocate_name (task_pi->task_port, right, name);
  debug ("%s", strerror (err));
  ports_port_deref (task_pi);
  return err;
}

/* Routine mach_port_allocate */
kern_return_t
S_mach_port_allocate (mach_port_t task,
		      mach_port_right_t right, mach_port_t *name)
{
  struct task_info *task_pi;
  error_t err;

  task_pi = ports_lookup_port (port_bucket, task, task_portclass);
  if (task_pi == NULL)
    return EOPNOTSUPP;
  err = mach_port_allocate (task_pi->task_port, right, name);
  debug ("get a request from task %d: %s", task_pi->task_port, strerror (err));
  ports_port_deref (task_pi);
  return err;
}

/* Routine mach_port_destroy */
kern_return_t
S_mach_port_destroy (mach_port_t task, mach_port_t name)
{
  struct task_info *task_pi;
  error_t err;

  task_pi = ports_lookup_port (port_bucket, task, task_portclass);
  if (task_pi == NULL)
    return EOPNOTSUPP;
  err = mach_port_destroy (task_pi->task_port, name);
  debug ("get a request from port %d: %s",
	 task_pi->task_port, strerror (err));
  ports_port_deref (task_pi);
  return err;
}

/* Routine mach_port_deallocate */
kern_return_t
S_mach_port_deallocate (mach_port_t task, mach_port_t name)
{
  struct task_info *task_pi;
  error_t err;

  task_pi = ports_lookup_port (port_bucket, task, task_portclass);
  if (task_pi == NULL)
    return EOPNOTSUPP;
  err = mach_port_deallocate (task_pi->task_port, name);
  debug ("get a request from task %d, name: %d, %s",
	 task_pi->task_port, name, strerror (err));
  ports_port_deref (task_pi);
  return err;
}

/* Routine mach_port_get_refs */
kern_return_t
S_mach_port_get_refs (mach_port_t task, mach_port_t name,
		      mach_port_right_t right, mach_port_urefs_t *refs)
{
  struct task_info *task_pi;
  error_t err;

  task_pi = ports_lookup_port (port_bucket, task, task_portclass);
  if (task_pi == NULL)
    return EOPNOTSUPP;
  err = mach_port_get_refs (task_pi->task_port, name, right, refs);
  debug ("%s", strerror (err));
  ports_port_deref (task_pi);
  return err;
}

/* Routine mach_port_mod_refs */
kern_return_t
S_mach_port_mod_refs (mach_port_t task, mach_port_t name,
		      mach_port_right_t right, mach_port_delta_t delta)
{
  struct task_info *task_pi;
  error_t err;

  task_pi = ports_lookup_port (port_bucket, task, task_portclass);
  if (task_pi == NULL)
    return EOPNOTSUPP;
  err = mach_port_mod_refs (task_pi->task_port, name, right, delta);
  debug ("get a request from task %d: port: %d, right: %d, delta: %d, %s",
	 task_pi->task_port, name, right, delta, strerror (err));
  ports_port_deref (task_pi);
  return err;
}

/* Routine old_mach_port_get_receive_status */
kern_return_t
S_old_mach_port_get_receive_status (mach_port_t task, mach_port_t name,
				    old_mach_port_status_t *status)
{
  debug ("%s", strerror (EOPNOTSUPP));
  assert (0);
  /* Hurd currently doesn't use it. */
  return EOPNOTSUPP;
}

/* Routine mach_port_set_qlimit */
kern_return_t
S_mach_port_set_qlimit (mach_port_t task, mach_port_t name,
			mach_port_msgcount_t qlimit)
{
  struct task_info *task_pi;
  error_t err;

  task_pi = ports_lookup_port (port_bucket, task, task_portclass);
  if (task_pi == NULL)
    return EOPNOTSUPP;
  err = mach_port_set_qlimit (task_pi->task_port, name, qlimit);
  debug ("%s", strerror (err));
  ports_port_deref (task_pi);
  return err;
}

/* Routine mach_port_set_mscount */
kern_return_t
S_mach_port_set_mscount (mach_port_t task, mach_port_t name,
			 mach_port_mscount_t mscount)
{
  struct task_info *task_pi;
  error_t err;

  task_pi = ports_lookup_port (port_bucket, task, task_portclass);
  if (task_pi == NULL)
    return EOPNOTSUPP;
  err = mach_port_set_mscount (task_pi->task_port, name, mscount);
  debug ("%s", strerror (err));
  ports_port_deref (task_pi);
  return err;
}

/* Routine mach_port_get_set_status */
kern_return_t
S_mach_port_get_set_status (mach_port_t task, mach_port_t name,
			    mach_port_array_t *members,
			    mach_msg_type_number_t *membersCnt)
{
  struct task_info *task_pi;
  error_t err;

  task_pi = ports_lookup_port (port_bucket, task, task_portclass);
  if (task_pi == NULL)
    return EOPNOTSUPP;
  err = mach_port_get_set_status (task_pi->task_port,
				  name, members, membersCnt);
  debug ("%s", strerror (err));
  ports_port_deref (task_pi);
  /* The deallocation bit is set,
   * so 'members' will be deallocated after the reply is sent. */
  return err;
}

/* Routine mach_port_move_member */
kern_return_t
S_mach_port_move_member (mach_port_t task, mach_port_t member,
			 mach_port_t after)
{
  struct task_info *task_pi;
  error_t err;

  task_pi = ports_lookup_port (port_bucket, task, task_portclass);
  if (task_pi == NULL)
    return EOPNOTSUPP;
  err = mach_port_move_member (task_pi->task_port, member, after);
  debug ("%s", strerror (err));
  ports_port_deref (task_pi);
  return err;
}

/* Routine mach_port_request_notification */
kern_return_t
S_mach_port_request_notification (mach_port_t task, mach_port_t name,
				  mach_msg_id_t id, mach_port_mscount_t sync,
				  mach_port_t notify, mach_port_t *previous,
				  mach_msg_type_name_t *previousPoly)
{
  struct task_info *task_pi;
  mach_msg_type_name_t notify_type = MACH_MSG_TYPE_MOVE_SEND_ONCE;
  error_t err;

  task_pi = ports_lookup_port (port_bucket, task, task_portclass);
  if (task_pi == NULL)
    return EOPNOTSUPP;
  /* notify is send-once right from the client. */
  err = mach_port_request_notification (task_pi->task_port, name, id, sync,
					notify, notify_type, previous);
  debug ("get a request from task %d, port: %d, id: %d, notify: %d, notify_type: %d, old port: %d, %s",
	 task_pi->task_port, name, id, notify, notify_type, *previous, strerror (err));
  ports_port_deref (task_pi);
  if (err)
    return err;
  *previousPoly = MACH_MSG_TYPE_MOVE_SEND_ONCE;

  return 0;
}

/* Routine mach_port_insert_right */
kern_return_t
S_mach_port_insert_right (mach_port_t task, mach_port_t name,
			  mach_port_t poly, mach_msg_type_name_t polyPoly)
{
  struct task_info *task_pi;
  error_t err;

  task_pi = ports_lookup_port (port_bucket, task, task_portclass);
  if (task_pi == NULL)
    return EOPNOTSUPP;
  /* polyPoly can only be three possible values: MACH_MSG_TYPE_PORT_SEND,
   * MACH_MSG_TYPE_PORT_SEND_ONCE and MACH_MSG_TYPE_PORT_RECEIVE,
   * so port will be deallocated when mach_port_insert_right is called. */
  err = mach_port_insert_right (task_pi->task_port, name, poly, polyPoly);
  debug ("get a request from task %d: %s", task_pi->task_port, strerror (err));
  ports_port_deref (task_pi);
  return err;
}

/* Routine mach_port_extract_right */
kern_return_t
S_mach_port_extract_right (mach_port_t task, mach_port_t name,
			   mach_msg_type_name_t msgt_name, mach_port_t *poly,
			   mach_msg_type_name_t *polyPoly)
{
  struct task_info *task_pi;
  error_t err;

  task_pi = ports_lookup_port (port_bucket, task, task_portclass);
  if (task_pi == NULL)
    return EOPNOTSUPP;
  err = mach_port_extract_right (task_pi->task_port, name, msgt_name,
				 poly, polyPoly);
  debug ("%s", strerror (err));
  ports_port_deref (task_pi);
  /* *polyPoly can only be MACH_MSG_TYPE_PORT_SEND, MACH_MSG_TYPE_PORT_RECEIVE,
   * or MACH_MSG_TYPE_PORT_SEND_ONCE, so the port extracted from 'task' 
   * will be moved to the client. */
  return err;
}

/* Routine mach_port_get_receive_status */
kern_return_t
S_mach_port_get_receive_status (mach_port_t task, mach_port_t name,
				mach_port_status_t *status)
{
  struct task_info *task_pi;
  error_t err;

  task_pi = ports_lookup_port (port_bucket, task, task_portclass);
  if (task_pi == NULL)
    return EOPNOTSUPP;
  err = mach_port_get_receive_status (task_pi->task_port, name, status);
  debug ("%s", strerror (err));
  ports_port_deref (task_pi);
  return err;
}

/* Routine mach_port_set_seqno */
kern_return_t
S_mach_port_set_seqno (mach_port_t task, mach_port_t name,
		       mach_port_seqno_t seqno)
{
  struct task_info *task_pi;
  error_t err;

  task_pi = ports_lookup_port (port_bucket, task, task_portclass);
  if (task_pi == NULL)
    return EOPNOTSUPP;
  err = mach_port_set_seqno (task_pi->task_port, name, seqno);
  debug ("%s", strerror (err));
  ports_port_deref (task_pi);
  return err;
}
