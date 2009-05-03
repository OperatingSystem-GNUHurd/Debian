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

#include <string.h>

#include "util.h"
#include "mach_proxy.h"

LIST_HEADER (tasks_head);

int create_pseudo_task (task_t real_task, task_t *ret_pseudo_task)
{
  /* the first task is the kernel task. */
  struct task_info *task_pi;
  task_t pseudo_task;
  error_t err;

  err = ports_create_port (task_portclass, port_bucket,
			   sizeof (struct task_info), &task_pi);
  if (err)
    return err;

  pseudo_task = ports_get_right (task_pi);
  mach_port_insert_right (mach_task_self (), pseudo_task, pseudo_task,
			  MACH_MSG_TYPE_MAKE_SEND);
  ports_port_deref (task_pi);

  task_pi->task_port = real_task;
  entry_init (&task_pi->list);
  add_entry_end (&tasks_head, &task_pi->list);
  /* 0 is used by kernel task */
  if (ret_pseudo_task)
    *ret_pseudo_task = pseudo_task;

  err = task_set_kernel_port (real_task, pseudo_task);
  if (err) 
    {
      debug ("fail to set the kernel port: %s", strerror (err));
      ports_destroy_right (task_pi);
    }

  return err;
}

void clean_pseudo_task (void *pi)
{
  struct task_info *task = pi;
  debug ("remove a pseudo task from the list");
  remove_entry (&task->list);
}
