/*
   Copyright (C) 1995, 1996, 1998, 1999, 2000, 2002, 2007, 2008
   Free Software Foundation, Inc.

   Written by Zheng Da 

   Based on pfinet/ethernet.c, written by Michael I. Bushnell, p/BSG.

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

#include <string.h>
#include <error.h>
#include <assert.h>
#include <net/if.h>
#include <sys/ioctl.h>

#include <hurd/ports.h>
#include <device/device.h>
#include <device/net_status.h>

#include "ethernet.h"
#include "vdev.h"
#include "util.h"

#define ETH_HLEN 14

static struct port_info *readpt;

/* Port for writing message to the real network interface. */
mach_port_t ether_port;
/* Port for receiving messages from the interface. */
static mach_port_t readptname;

/* The BPF instruction allows IP and ARP packets */
static struct bpf_insn ether_filter[] =
{
    {NETF_IN|NETF_BPF, /* Header. */ 0, 0, 0},
    {40, 0, 0, 12},
    {21, 1, 0, 2054},
    {21, 0, 1, 2048},
    {6, 0, 0, 1500},
    {6, 0, 0, 0}
};
static int ether_filter_len = sizeof (ether_filter) / sizeof (short);

int ethernet_demuxer (mach_msg_header_t *inp,
		      mach_msg_header_t *outp)
{
  struct net_rcv_msg *msg = (struct net_rcv_msg *) inp;

  if (inp->msgh_id != NET_RCV_MSG_ID)
    return 0;

  broadcast_msg (msg);
  /* The data from the underlying network is inside the message,
   * so we don't need to deallocate the data. */
  return 1;
}

int set_promisc (char *dev_name, mach_port_t ether_port, int is_promisc)
{
#ifndef NET_FLAGS
#define NET_FLAGS (('n'<<16) + 4)
#endif
  short flags;
  int ret;
  size_t count;

  debug ("set_promisc is called, is_promisc: %d\n", is_promisc);
  count = sizeof (flags);
  ret = device_get_status (ether_port, NET_FLAGS, (dev_status_t) &flags, 
                           &count);
  if (ret) 
    {
      error (0, ret, "device_get_status");  
      return -1;
    }  
  if (is_promisc)
    flags |= IFF_PROMISC;
  else
    flags &= ~IFF_PROMISC;
  ret = device_set_status(ether_port, NET_FLAGS, (dev_status_t) &flags,
                          sizeof (flags));
  if (ret) 
    {
      error (0, ret, "device_set_status");
      return -1;
    }  
  return 0;  
} 

int ethernet_open (char *dev_name, device_t master_device, 
		   struct port_bucket *etherport_bucket,
		   struct port_class *etherreadclass)
{
  error_t err;

  assert (ether_port == MACH_PORT_NULL);

  err = ports_create_port (etherreadclass, etherport_bucket,
			   sizeof (struct port_info), &readpt);
  if (err)
    error (2, err, "ports_create_port");
  readptname = ports_get_right (readpt);
  mach_port_insert_right (mach_task_self (), readptname, readptname,
			  MACH_MSG_TYPE_MAKE_SEND);

  mach_port_set_qlimit (mach_task_self (), readptname, MACH_PORT_QLIMIT_MAX);

  err = device_open (master_device, D_WRITE | D_READ, "eth", &ether_port);
  mach_port_deallocate (mach_task_self (), master_device);
  if (err)
    error (2, err, "device_open: %s", dev_name);

  err = device_set_filter (ether_port, ports_get_right (readpt),
			   MACH_MSG_TYPE_MAKE_SEND, 0,
			   (unsigned short *)ether_filter, ether_filter_len);
  if (err)
    error (2, err, "device_set_filter: %s", dev_name);

  set_promisc (dev_name, ether_port, 1);
  return 0;
}

int ethernet_close (char *dev_name)
{
  set_promisc (dev_name, ether_port, 0);
  return 0;
}

