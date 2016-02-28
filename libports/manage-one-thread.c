/* 
   Copyright (C) 1995, 1996 Free Software Foundation, Inc.
   Writtenb by Michael I. Bushnell.

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

#include <stdio.h>
#include <assert.h>
#include "ports.h"

mach_msg_return_t
ported_mach_msg_server_timeout (boolean_t (*demux) (mach_msg_header_t *request,
						    mach_msg_header_t *reply),
				mach_msg_size_t max_size,
				mach_port_t rcv_name,
				mach_msg_option_t option,
				mach_msg_timeout_t timeout)
{
  register mig_reply_header_t *request, *reply;
  register mach_msg_return_t mr;

  if (max_size == 0)
    {
#ifdef MACH_RCV_LARGE
      option |= MACH_RCV_LARGE;
      max_size = 2 * __vm_page_size; /* Generic.  Good? XXX */
#else
      max_size = 4 * __vm_page_size; /* XXX */
#endif
    }

  request = alloca (max_size);
  reply = alloca (max_size);

  while (1)
    {
get_request:
      mr = mach_msg (&request->Head, MACH_RCV_MSG|option,
		     0, max_size, rcv_name,
		     timeout, MACH_PORT_NULL);
      while (mr == MACH_MSG_SUCCESS)
	{
	  /* We have a request message.
	   * Pass it to DEMUX for processing.  */

	  (void) (*demux) (&request->Head, &reply->Head);
	  assert (reply->Head.msgh_size <= max_size);

	  switch (reply->RetCode)
	    {
	    case KERN_SUCCESS:
	      /* Hunky dory.  */
	      break;

	    case MIG_NO_REPLY:
	      /* The server function wanted no reply sent.
	       * Loop for another request.  */
	      goto get_request;

	    default:
	      /* Some error; destroy the request message to release any
	       * port rights or VM it holds.  Don't destroy the reply port
	       * right, so we can send an error message.  */
	      request->Head.msgh_remote_port = MACH_PORT_NULL;
	      mach_msg_destroy (&request->Head);
	      break;
	    }

	  if (reply->Head.msgh_remote_port == MACH_PORT_NULL)
	    {
	      /* No reply port, so destroy the reply.  */
	      if (reply->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX)
		mach_msg_destroy (&reply->Head);
	      goto get_request;
	    }

	  /* Send the reply and the get next request.  */

	    {
	      /* Swap the request and reply buffers.  mach_msg will read the
	       * reply message from the buffer we pass and write the new
	       * request message to the same buffer.  */
	      void *tmp = request;
	      request = reply;
	      reply = tmp;
	    }

	  mr = mach_msg (&request->Head,
			 MACH_SEND_MSG|MACH_RCV_MSG|option,
			 request->Head.msgh_size, max_size, rcv_name,
			 timeout, MACH_PORT_NULL);
	}

      /* A message error occurred.  */

      switch (mr)
	{
	case MACH_RCV_TOO_LARGE:
#ifdef MACH_RCV_LARGE
	  /* The request message is larger than MAX_SIZE, and has not
	   * been dequeued.  The message header has the actual size of
	   * the message.  We recurse here in hopes that the compiler
	   * will optimize the tail-call and allocate some more stack
	   * space instead of way too much.  */
	  return ported_mach_msg_server_timeout (demux, request->Head.msgh_size,
						 rcv_name, option, timeout);
#else
	  /* XXX the kernel has destroyed the msg */
	  break;
#endif
	case MACH_SEND_INVALID_DEST:
	  /* The reply can't be delivered, so destroy it.  This error
	   * indicates only that the requester went away, so we
	   * continue and get the next request.  */
	  mach_msg_destroy (&request->Head);
	  break;

	default:
	  /* Some other form of lossage; return to caller.  */
	  return mr;
	}
    }
}


void
ports_manage_port_operations_one_thread (struct port_bucket *bucket,
					 ports_demuxer_type demuxer,
					 int timeout)
{
  struct ports_thread thread;
  error_t err;

  int 
  internal_demuxer (mach_msg_header_t *inp,
		    mach_msg_header_t *outheadp)
    {
      struct port_info *pi;
      struct rpc_info link;
      int status;
      error_t err;
      register mig_reply_header_t *outp = (mig_reply_header_t *) outheadp;
      static const mach_msg_type_t RetCodeType = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

      /* Fill in default response. */
      outp->Head.msgh_bits 
	= MACH_MSGH_BITS(MACH_MSGH_BITS_REMOTE(inp->msgh_bits), 0);
      outp->Head.msgh_size = sizeof *outp;
      outp->Head.msgh_remote_port = inp->msgh_remote_port;
      outp->Head.msgh_local_port = MACH_PORT_NULL;
      outp->Head.msgh_seqno = 0;
      outp->Head.msgh_id = inp->msgh_id + 100;
      outp->RetCodeType = RetCodeType;
      outp->RetCode = MIG_BAD_ID;

      if (MACH_MSGH_BITS_LOCAL (inp->msgh_bits) ==
	  MACH_MSG_TYPE_PROTECTED_PAYLOAD)
	pi = ports_lookup_payload (bucket, inp->msgh_protected_payload, NULL);
      else
	{
	  pi = ports_lookup_port (bucket, inp->msgh_local_port, 0);
	  if (pi)
	    {
	      /* Store the objects address as the payload and set the
		 message type accordingly.  This prevents us from
		 having to do another hash table lookup in the intran
		 functions if protected payloads are not supported by
		 the kernel.  */
	      inp->msgh_bits =
		MACH_MSGH_BITS_OTHER (inp->msgh_bits)
		| MACH_MSGH_BITS (MACH_MSGH_BITS_REMOTE (inp->msgh_bits),
				  MACH_MSG_TYPE_PROTECTED_PAYLOAD);
	      inp->msgh_protected_payload = (unsigned long) pi;
	    }
	}

      if (pi)
	{
	  err = ports_begin_rpc (pi, inp->msgh_id, &link);
	  if (err)
	    {
	      mach_port_deallocate (mach_task_self (), inp->msgh_remote_port);
	      outp->RetCode = err;
	      status = 1;
	    }
	  else
	    {
	      /* No need to check cancel threshold here, because
		 in a single threaded server the cancel is always
		 handled in order. */
	      status = demuxer (inp, outheadp);
	      ports_end_rpc (pi, &link);
	    }
	  ports_port_deref (pi);
	}
      else
	{
	  outp->RetCode = EOPNOTSUPP;
	  status = 1;
	}

      _ports_thread_quiescent (&bucket->threadpool, &thread);
      return status;
    }

  /* XXX It is currently unsafe for most servers to terminate based on
     inactivity because a request may arrive after a server has
     started shutting down, causing the client to receive an error.
     Prevent the service loop from terminating by setting TIMEOUT to
     zero.  */
  timeout = 0;

  _ports_thread_online (&bucket->threadpool, &thread);
  do
    err = ported_mach_msg_server_timeout (internal_demuxer, 0, bucket->portset, 
					  timeout ? MACH_RCV_TIMEOUT : 0, timeout);
  while (err != MACH_RCV_TIMED_OUT);
  _ports_thread_offline (&bucket->threadpool, &thread);
}
