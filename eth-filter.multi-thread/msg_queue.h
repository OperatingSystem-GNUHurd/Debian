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

#ifndef MSG_QUEUE_H
#define MSG_QUEUE_H

#include <hurd.h>
#include <mach.h>

#define MAX_QUEUE 1000

struct proxy;

struct filter_msg 
{
  struct proxy *proxy;
  struct filter_msg *next;

  int (*forward) (struct filter_msg *);
  void (*destroy) (struct filter_msg *);
};

struct filter_write_msg
{
  struct filter_msg msg;

  char *data;
  size_t datalen;
  dev_mode_t mode;
  recnum_t recnum;
  mach_port_t reply_port;
  mach_msg_type_name_t reply_type;
};

struct filter_deliver_msg 
{
  struct filter_msg msg;
  
  struct net_rcv_msg *net_msg;
};

void queue_msg (struct filter_msg *msg);
struct filter_msg * dequeue_msg ();
void queue_flush ();
kern_return_t queue_write (char *data, size_t datalen, mach_port_t reply_port,
	     mach_msg_type_name_t reply_type, dev_mode_t mode,
	     recnum_t recnum, struct proxy *proxy);
int queue_deliver (struct net_rcv_msg *msg, struct proxy *proxy);
void queue_wakeup ();
void queue_empty_wait ();

#endif 
