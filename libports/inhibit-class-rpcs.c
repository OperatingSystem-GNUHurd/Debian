/*
   Copyright (C) 1995, 1996, 2000 Free Software Foundation, Inc.
   Written by Michael I. Bushnell.

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

#include "ports.h"
#include <hurd.h>
#include <cthreads.h>

error_t
ports_inhibit_class_rpcs (struct port_class *class)
{
  error_t err = 0;

  mutex_lock (&_ports_lock);

  if (class->flags & (PORT_CLASS_INHIBITED | PORT_CLASS_INHIBIT_WAIT))
    err = EBUSY;
  else
    {
      struct port_info *pi;
      struct rpc_info *rpc;
      int this_one = 0;

      for (pi = class->ports; pi; pi = pi->next)
	for (rpc = pi->current_rpcs; rpc; rpc = rpc->next)
	  {
	    /* Avoid cancelling the calling thread.  */
	    if (rpc->thread == hurd_thread_self ())
	      this_one = 1;
	    else
	      hurd_thread_cancel (rpc->thread);
	  }

      while (class->rpcs > this_one)
	{
	  class->flags |= PORT_CLASS_INHIBIT_WAIT;
	  if (hurd_condition_wait (&_ports_block, &_ports_lock))
	    /* We got cancelled.  */
	    {
	      err = EINTR;
	      break;
	    }
	}

      class->flags &= ~PORT_CLASS_INHIBIT_WAIT;
      if (! err)
	class->flags |= PORT_CLASS_INHIBITED;
    }

  mutex_unlock (&_ports_lock);

  return err;
}
