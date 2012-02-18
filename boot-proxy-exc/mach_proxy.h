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

#ifndef PROXY_H
#define PROXY_H

#include <mach.h>
#include <hurd.h>
#include <hurd/ports.h>

#include "list.h"

struct task_info
{
  struct port_info task;
  task_t task_port;
  struct port_info *exc_pi;
  volatile mach_port_t user_exc_port;
  struct mutex lock;
  /* The linked list is used to keep the order of tasks */
  struct list list;
};

struct thread_info
{
  struct port_info thread;
  thread_t thread_port;
};

typedef int (*task_callback_t) (struct task_info *);

extern struct port_bucket *port_bucket;
extern struct port_class *task_portclass;
extern struct port_class *exc_portclass;
extern struct port_class *thread_portclass;

int create_pseudo_task (task_t real_task, task_t *ret_pseudo_task);
void clean_pseudo_task (void *pi);
void foreach_task (task_callback_t callback);

#define UNTESTED

#endif