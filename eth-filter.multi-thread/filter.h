/* 
   Copyright (C) 2008 Free Software Foundation, Inc.
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

#ifndef FILTER_H
#define FILTER_H

#include <hurd.h>
#include <mach.h>
#include <hurd/ports.h>

#include "bpf_impl.h"

struct filter_msg;

struct proxy_user
{
  struct port_info pi;
  struct proxy *proxy;
};

struct proxy_device
{
  struct port_info pi;
  struct proxy *proxy;
};

struct proxy
{
  struct proxy_device *device;
  mach_port_t deliver_port;
  hurd_ihash_locp_t p_deliverport_hashloc;
  mach_port_t device_port;
};

int deliver_msg (struct net_rcv_msg *msg, queue_head_t *port_list, 
		 mach_port_t dest);
int filter_device_write (struct filter_msg *msg);
int filter_deliver (struct filter_msg *msg);

#endif
