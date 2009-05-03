/* boot_script.c support functions for running in a Mach user task.
   Copyright (C) 2001 Free Software Foundation, Inc.

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
#include <string.h>
#include <mach.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <error.h>
#include <hurd.h>
#include <hurd/ports.h>
#include "boot_script.h"
#include "mach_proxy.h"
#include "util.h"

void *
boot_script_malloc (unsigned int size)
{
  return malloc (size);
}

void
boot_script_free (void *ptr, unsigned int size)
{
  free (ptr);
}

int
boot_script_task_create (struct cmd *cmd)
{
  task_t pseudo_task = 0;
  error_t err = task_create (mach_task_self (), 0, &cmd->task);
  if (err)
    {
      error (0, err, "%s: task_create", cmd->path);
      return BOOT_SCRIPT_MACH_ERROR;
    }
  debug ("creating task: %s, task port: %d", cmd->path, cmd->task);
  create_pseudo_task (cmd->task, &pseudo_task);
  /* now it is the pseudo task port. */
  cmd->task = pseudo_task;
  err = task_suspend (cmd->task);
  if (err)
    {
      error (0, err, "%s: task_suspend", cmd->path);
      return BOOT_SCRIPT_MACH_ERROR;
    }
  return 0;
}

int
boot_script_task_resume (struct cmd *cmd)
{
  debug ("resume task %s: %d", cmd->path, cmd->task);
  error_t err = task_resume (cmd->task);
  if (err)
    {
      error (0, err, "%s: task_resume", cmd->path);
      return BOOT_SCRIPT_MACH_ERROR;
    }
  return 0;
}

int
boot_script_prompt_task_resume (struct cmd *cmd)
{
  char xx[5];

  printf ("Hit return to resume %s...", cmd->path);
  fgets (xx, sizeof xx, stdin);

  return boot_script_task_resume (cmd);
}

void
boot_script_free_task (task_t task, int aborting)
{
  if (aborting)
    task_terminate (task);
  /* the task port will still be used,
   * so we cannot deallocate the task port here.*/
}

int
boot_script_insert_right (struct cmd *cmd, mach_port_t port, mach_port_t *name)
{
  error_t err = mach_port_insert_right (cmd->task,
					port, port, MACH_MSG_TYPE_COPY_SEND);
  if (err)
    {
      error (0, err, "%s: mach_port_insert_right", cmd->path);
      return BOOT_SCRIPT_MACH_ERROR;
    }
  *name = port;
  return 0;
}

int
boot_script_insert_task_port (struct cmd *cmd, task_t task, mach_port_t *name)
{
  return boot_script_insert_right (cmd, task, name);
}
