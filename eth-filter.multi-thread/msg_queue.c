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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "msg_queue.h"
#include "filter.h"
#include "util.h"

static struct filter_msg *queue_head;
static struct filter_msg *queue_tail;
static int queue_len = 0;

static pthread_mutex_t queuelock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t condition_cond = PTHREAD_COND_INITIALIZER;

void
queue_msg (struct filter_msg *msg)
{
  msg->next = NULL;
  // TODO what kind of lock do I need?
  pthread_mutex_lock (&queuelock);
  if (queue_head == NULL)
    {
      assert (queue_tail == NULL);
      assert (queue_len == 0);
      /* When the queue is empty. */
      queue_head = msg;
      queue_tail = msg;
    }
  else
    {
      queue_tail->next = msg;
      queue_tail = msg;
    }
  queue_len++;
  debug ("queue a message, queue length: %d.\n", queue_len);
  pthread_mutex_unlock (&queuelock);
//  queue_wakeup ();
}

struct filter_msg *
dequeue_msg ()
{
  struct filter_msg *msg;

  pthread_mutex_lock (&queuelock);
  if (queue_head == NULL)
    {
      assert (queue_tail == NULL);
      assert (queue_len == 0);
      msg = NULL;
    }
  else
    {
      msg = queue_head;
      queue_head = msg->next;
      if (queue_head == NULL)
	queue_tail = NULL;
      queue_len--;
    }
  debug ("dequeue a message, the queue length: %d.\n", queue_len);
  pthread_mutex_unlock (&queuelock);

  return msg;
}

/* It can be called when the translator exits. */
void
queue_flush ()
{
  struct filter_msg *msg;
  struct filter_msg *tmp;

  pthread_mutex_lock (&queuelock);
  msg = queue_head;
  queue_head = queue_tail = NULL;
  queue_len = 0;
  pthread_mutex_unlock (&queuelock);

  while (msg)
    {
      tmp = msg;
      msg = msg->next;
      if (tmp->destroy)
	tmp->destroy (tmp);
      free (tmp);
    }
}

void
filter_write_destroy (struct filter_msg *msg)
{
  struct filter_write_msg *write_msg = (struct filter_write_msg *) msg;

  free (write_msg->data);
}

void filter_deliver_destroy (struct filter_msg *msg)
{
  struct filter_deliver_msg *deliver_msg = (struct filter_deliver_msg *) msg;

  free (deliver_msg->net_msg);
}

kern_return_t
queue_write (char *data, size_t datalen, mach_port_t reply_port,
	     mach_msg_type_name_t reply_type, dev_mode_t mode,
	     recnum_t recnum, struct proxy *proxy)
{
  /* Make a copy of the data */
  char *new_data;
  struct filter_msg *msg;
  struct filter_write_msg *write_msg;

  if (queue_len >= MAX_QUEUE)
    return D_NO_MEMORY;
  
  new_data = (char *) malloc (datalen);
  if (new_data == NULL)
    return D_NO_MEMORY;

  memcpy (new_data, data, datalen);

  write_msg = (struct filter_write_msg *) malloc (sizeof (*write_msg));
  if (write_msg == NULL)
    {
      free (new_data);
      return D_NO_MEMORY;
    }
  msg = (struct filter_msg *) write_msg;

  msg->proxy = proxy;
  msg->forward = filter_device_write;
  msg->destroy = filter_write_destroy;

  write_msg->data = new_data;
  write_msg->datalen = datalen;
  write_msg->reply_port = reply_port;
  write_msg->reply_type = reply_type;
  write_msg->mode = mode;
  write_msg->recnum = recnum;

  queue_msg (msg);
  return D_SUCCESS;
}

int
queue_deliver (struct net_rcv_msg *msg, struct proxy *proxy)
{
  struct net_rcv_msg *msg_copy;
  struct filter_deliver_msg *deliver_msg;

  if (queue_len >= MAX_QUEUE)
    return D_NO_MEMORY;

  /* Make a copy of the data */
  msg_copy = (struct net_rcv_msg *) malloc (sizeof (*msg_copy));
  if (msg_copy == NULL)
    return D_NO_MEMORY;

  deliver_msg = (struct filter_deliver_msg *) malloc (sizeof (*deliver_msg));
  if (deliver_msg == NULL)
    {
      free (msg_copy);
      return D_NO_MEMORY;
    }

  *msg_copy = *msg;

  deliver_msg->msg.proxy = proxy;
  deliver_msg->msg.forward = filter_deliver;
  deliver_msg->msg.destroy = filter_deliver_destroy;

  deliver_msg->net_msg = msg_copy;

  queue_msg ((struct filter_msg *)deliver_msg);
  return D_SUCCESS;
}

void
queue_empty_wait ()
{
  pthread_mutex_lock (&condition_mutex);
//  debug ("queue length is %d\n", queue_len);
  while (queue_head == NULL)
    {
      debug ("thread waits for a signal.\n");
      pthread_cond_wait (&condition_cond, &condition_mutex);
    }
  pthread_mutex_unlock (&condition_mutex);
}

void
queue_wakeup ()
{
  if (queue_head)
    {
      debug ("wake up a thread.\n");
      pthread_cond_signal (&condition_cond);
    }
}

