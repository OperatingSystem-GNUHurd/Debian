/*
   Copyright (C) 1995, 1996, 2001, 2007 Free Software Foundation, Inc.

   Written by Michael I. Bushnell, p/BSG.

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

#ifndef _CONSOLE_PRIV_H
#define _CONSOLE_PRIV_H

#include <hurd/hurd_types.h>

#include <hurd/netfs.h>

static inline struct protid * __attribute__ ((unused))
begin_using_protid_port (file_t port)
{
  return ports_lookup_port (netfs_port_bucket, port, netfs_protid_class);
}

static inline void __attribute__ ((unused))
end_using_protid_port (struct protid *cred)
{
  if (cred)
    ports_port_deref (cred);
}
#endif
