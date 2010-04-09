/* Service a fsys_startup RPC.

   Copyright (C) 1995, 1996, 1999, 2000, 2002, 2004, 2010
     Free Software Foundation, Inc.

   Written by Miles Bader and Michael I. Bushnell.

   This file is part of the GNU Hurd.

   The GNU Hurd is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   The GNU Hurd is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.  */

#include <hurd.h>
#include <mach/notify.h>
#include "fshelp.h"


/* The data passed in the various messages we're interested in.  */
struct fsys_startup_request
{
  mach_msg_header_t head;
  mach_msg_type_t flagsType;
  int flags;
  mach_msg_type_t control_portType;
  mach_port_t control_port;
};

struct fsys_startup_reply
{
  mach_msg_header_t head;
  mach_msg_type_t RetCodeType;
  kern_return_t RetCode;
  mach_msg_type_t realnodeType;
  mach_port_t realnode;
};

error_t
fshelp_service_fsys_startup (fshelp_open_fn_t underlying_open_fn,
			     void *cookie, mach_port_t port, long timeout,
			     task_t task, fsys_t *control)
{
  /* These should be optimized away to pure integer constants.  */
  const mach_msg_type_t flagsCheck =
    {
      MACH_MSG_TYPE_INTEGER_32,	/* msgt_name = */
      32,			/* msgt_size = */
      1,			/* msgt_number = */
      TRUE,			/* msgt_inline = */
      FALSE,			/* msgt_longform = */
      FALSE,			/* msgt_deallocate = */
      0				/* msgt_unused = */
    };
  const mach_msg_type_t control_portCheck =
    {
      MACH_MSG_TYPE_PORT_SEND,	/* msgt_name = */
      32,			/* msgt_size = */
      1,			/* msgt_number = */
      TRUE,			/* msgt_inline = */
      FALSE,			/* msgt_longform = */
      FALSE,			/* msgt_deallocate = */
      0				/* msgt_unused = */
    };
  const mach_msg_type_t RetCodeType =
    {
      MACH_MSG_TYPE_INTEGER_32,	/* msgt_name = */
      32,			/* msgt_size = */
      1,			/* msgt_number = */
      TRUE,			/* msgt_inline = */
      FALSE,			/* msgt_longform = */
      FALSE,			/* msgt_deallocate = */
      0				/* msgt_unused = */
    };
  const mach_msg_type_t realnodeType =
    {
      -1,			/* msgt_name = */
      32,			/* msgt_size = */
      1,			/* msgt_number = */
      TRUE,			/* msgt_inline = */
      FALSE,			/* msgt_longform = */
      FALSE,			/* msgt_deallocate = */
      0				/* msgt_unused = */
    };

  /* Return true iff TYPE fails to match CHECK.  */
  inline int type_check (const mach_msg_type_t *type,
			 const mach_msg_type_t *check)
    {
      union
      {
	unsigned32_t word;
	mach_msg_type_t type;
      } t, c;
      t.type = *type;
      c.type = *check;
      return t.word != c.word;
    }

  error_t err;
  union
  {
    mach_msg_header_t head;
    struct fsys_startup_request startup;
  }
  request;
  struct fsys_startup_reply reply;

  /* Wait for the fsys_startup message...  */
  err = mach_msg (&request.head, (MACH_RCV_MSG | MACH_RCV_INTERRUPT
				  | (timeout ? MACH_RCV_TIMEOUT : 0)),
		  0, sizeof(request), port, timeout, MACH_PORT_NULL);
  if (err)
    return err;

  /* Check whether we actually got a no-senders notification instead.  */
  if (request.head.msgh_id == MACH_NOTIFY_NO_SENDERS)
    return EDIED;

  /* Construct our reply to the fsys_startup rpc.  */
  reply.head.msgh_size = sizeof(reply);
  reply.head.msgh_bits =
    MACH_MSGH_BITS(MACH_MSGH_BITS_REMOTE(request.head.msgh_bits), 0);
  reply.head.msgh_remote_port = request.head.msgh_remote_port;
  reply.head.msgh_local_port = MACH_PORT_NULL;
  reply.head.msgh_seqno = 0;
  reply.head.msgh_id = request.head.msgh_id + 100;
  reply.RetCodeType = RetCodeType;

  if (request.head.msgh_id != 22000)
    reply.RetCode = MIG_BAD_ID;
  else if (type_check (&request.startup.control_portType, &control_portCheck)
	   || type_check (&request.startup.flagsType, &flagsCheck))
    reply.RetCode = MIG_BAD_ARGUMENTS;
  else
    {
      mach_msg_type_name_t realnode_type;

      *control = request.startup.control_port;

      reply.RetCode =
	(*underlying_open_fn) (request.startup.flags,
			       &reply.realnode, &realnode_type, task,
			       cookie);

      reply.realnodeType = realnodeType;
      reply.realnodeType.msgt_name = realnode_type;

      if (!reply.RetCode && reply.realnode != MACH_PORT_NULL)
	/* The message can't be simple because of the port.  */
	reply.head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
    }

  err = mach_msg (&reply.head, MACH_SEND_MSG | MACH_SEND_INTERRUPT,
		  sizeof(reply), 0,
		  request.head.msgh_remote_port,
		  MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
  if (err == MACH_SEND_INTERRUPTED
      && reply.realnodeType.msgt_name == MACH_MSG_TYPE_MOVE_SEND)
    /* For MACH_SEND_INTERRUPTED, we'll have pseudo-received the message
       and might have to clean up a generated send right.  */
    mach_port_deallocate (mach_task_self (), reply.realnode);

  if (reply.RetCode)
    /* Make our error return be the earlier one.  */
    err = reply.RetCode;

  return err;
}
