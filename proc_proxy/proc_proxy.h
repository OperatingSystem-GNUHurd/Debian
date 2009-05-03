/*    Copyright (C) 2008 Free Software Foundation, Inc.
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

#ifndef PROC_PROXY_H
#define PROC_PROXY_H

#include <stdio.h>

#include <hurd.h>
#include <hurd/ports.h>

#ifdef DEBUG

#define debug(format, ...)					\
  do								\
    {								\
      char buf [1024];						\
      snprintf (buf, sizeof (buf), "%s: %s", __func__, format);	\
      fprintf (stderr, buf, ## __VA_ARGS__);			\
    }								\
  while (0)

#else

#define debug(format, ...) do {} while (0)

#endif

struct vproc
{
  struct port_info p_pi;
  task_t task_id;
  /* The actual port to the proc server for the process. */
  process_t proc;
  hurd_ihash_locp_t p_task_hashloc;
};

typedef struct vproc *vpstruct_t;

static inline void
process_drop (vpstruct_t p)
{
  if (p)
    ports_port_deref (p);
}

vpstruct_t reqport_find (mach_port_t port);
vpstruct_t find_task (task_t task);
mach_port_t my_get_reply_port (void);

#endif
