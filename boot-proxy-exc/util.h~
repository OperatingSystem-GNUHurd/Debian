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

#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <syslog.h>

#include <mach.h>
#include <hurd.h>

#define DEBUG 0
#define INFO 1
#define LOGLEVEL INFO

extern FILE *logfile;

#define LOG_START() do					\
{							\
  logfile = fopen("boot.log", "a");			\
  assert (logfile != NULL);				\
} while (0)

#define LOG_END() fclose (logfile)

#define output(format, ...) do				\
{							\
  char buf[1024];                                       \
  snprintf (buf, 1024, "boot: %s: %s\r\n", __func__, format);       \
  fprintf (stderr , buf, ## __VA_ARGS__);		\
  fflush (stderr);					\
} while (0)

#if DEBUG >= LOGLEVEL
#define debug(format, ...) do				\
{							\
  char buf[1024];                                       \
  snprintf (buf, 1024, "boot: %s: %s\r\n", __func__, format);       \
  fprintf (logfile , buf, ## __VA_ARGS__);		\
  fflush (logfile);					\
} while (0)
#else
#define debug(format, ...) do {} while (0)
#endif

#if INFO >= LOGLEVEL
#define info output
#else
#define info(format, ...) do {} while (0)
#endif

#if ERROR >= LOGLEVEL
#define logerror(format, ...) do				\
{							\
  char buf[1024];                                       \
  snprintf (buf, 1024, "boot ERROR: %s: %s\r\n", __func__, format);       \
  fprintf (stderr , buf, ## __VA_ARGS__);		\
  fflush (stderr);					\
} while (0)
#else
#define logerror(format, ...) do {} while (0)
#endif

#define print_port_type(port) do				    \
{								    \
  mach_port_type_t ptype;					    \
  error_t err =	mach_port_type (mach_task_self (), port, &ptype);   \
  if (err)							    \
    debug ("mach_port_type: %s", strerror (err));		    \
  else if (ptype & MACH_PORT_TYPE_SEND)				    \
    debug ("port %d has send right", port);			    \
  else if (ptype & MACH_PORT_TYPE_RECEIVE)			    \
    debug ("port %d has receive right", port);			    \
  else if (ptype & MACH_PORT_TYPE_SEND_ONCE)			    \
    debug ("port %d has send once right", port);		    \
  else if (ptype & MACH_PORT_TYPE_PORT_SET)			    \
    debug ("port %d is port set", port);			    \
  else if (ptype & MACH_PORT_TYPE_DEAD_NAME)			    \
    debug ("port %d is a dead name", port);			    \
  else								    \
    debug ("the type of port %d is %d", port, ptype);		    \
  mach_port_urefs_t refs;					    \
  err = mach_port_get_refs (mach_task_self (), port,	    \
			    MACH_PORT_RIGHT_SEND, &refs);	    \
  if (!err)							    \
    debug ("port send right references: %d", refs);		    \
} while (0)

#endif
