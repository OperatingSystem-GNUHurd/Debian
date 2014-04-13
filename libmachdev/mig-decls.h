/*
   Copyright (C) 2014 Free Software Foundation, Inc.
   Written by Justus Winter.

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
   along with the GNU Hurd.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef __LIBMACHDEV_MIG_DECLS_H__
#define __LIBMACHDEV_MIG_DECLS_H__

#include <hurd/ports.h>
#include "dev_hdr.h"

extern struct port_bucket *device_bucket;
extern struct port_class *dev_class;

/* Called by server stub functions.  */

static inline struct mach_device * __attribute__ ((unused))
begin_using_device_port (mach_port_t port)
{
  return ports_lookup_port (device_bucket, port, dev_class);
}

static inline void __attribute__ ((unused))
end_using_device (struct mach_device *p)
{
  if (p)
    ports_port_deref (p);
}

#endif /* __LIBMACHDEV_MIG_DECLS_H__ */
