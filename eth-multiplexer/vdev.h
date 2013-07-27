/*
   Copyright (C) 2008 Free Software Foundation, Inc.
   Written by Zheng Da.

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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111, USA. */

#ifndef VDEV_H
#define VDEV_H

#include <net/if.h>

#include <hurd.h>
#include <mach.h>
#include <hurd/ports.h>
#include <device/net_status.h>

#include <bpf_impl.h>

#include "queue.h"
#include "util.h"

#define MAX_SERVERS 10
#define ETH_MTU 1500

struct vether_device
{
  /* The ports used by the socket server to send packets to the interface. */
  struct port_info dev_pi;
  mach_port_t dev_port;

  char name[IFNAMSIZ];

  short if_header_size;
  short if_mtu;
  short if_header_format;
  short if_address_size;
  short if_flags;
  char if_address[ETH_ALEN];

  struct vether_device *next;
  struct vether_device **pprev;

  if_filter_list_t port_list;
};

typedef int (*dev_act_func) (struct vether_device *);

int serv_connect (mach_port_t port);
int serv_disconnect ();
struct vether_device *lookup_dev_by_name (char *name);
int remove_dead_port_from_dev (mach_port_t dead_port);
struct vether_device *add_vdev (char *name, int size,
				struct port_class *class,
				struct port_bucket *bucket);
void destroy_vdev (void *port);
int has_vdev ();
int deliver_msg (struct net_rcv_msg *msg, struct vether_device *vdev);
int deliver_pack (char *data, int datalen, struct vether_device *vdev);
boolean_t all_dev_close ();
int broadcast_pack (char *data, int datalen, struct vether_device *from_vdev);
int broadcast_msg (struct net_rcv_msg *msg);
int get_dev_num ();
int foreach_dev_do (dev_act_func func);

#endif
