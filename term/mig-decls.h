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

#ifndef __TERM_MIG_DECLS_H__
#define __TERM_MIG_DECLS_H__

#include <hurd/ports.h>

#include "term.h"

/* Called by server stub functions.  */

static inline struct port_info * __attribute__ ((unused))
begin_using_ctty_port (mach_port_t port)
{
  return ports_lookup_port (term_bucket, port, cttyid_class);
}

static inline void __attribute__ ((unused))
end_using_ctty (struct port_info *p)
{
  if (p)
    ports_port_deref (p);
}

#endif /* __TERM_MIG_DECLS_H__ */
