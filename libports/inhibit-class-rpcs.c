/* 
   Copyright (C) 1995 Free Software Foundation, Inc.
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

void
ports_inhibit_class_rpcs (struct port_class *class)
{
  struct port_info *pi;
  struct rpc_info *rpc;
  
  mutex_lock (&_ports_lock);
  
  for (pi = class->ports; pi; pi = pi->next)
    for (rpc = pi->current_rpcs; rpc; rpc = rpc->next)
      hurd_thread_cancel (rpc->thread);

  while (class->rpcs)
    {
      class->flags |= PORT_CLASS_INHIBIT_WAIT;
      condition_wait (&_ports_block, &_ports_lock);
    }
  
  class->flags |= PORT_CLASS_INHIBITED;
  class->flags &= ~PORT_CLASS_INHIBIT_WAIT;

  mutex_unlock (&_ports_lock);
}

			 
