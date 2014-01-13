/* 
   Copyright (C) 1995, 1996 Free Software Foundation, Inc.
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

#include "netfs.h"
#include "fs_S.h"

error_t
netfs_S_file_set_size (struct protid *user,
		       off_t size)
{
  error_t err;
  
  if (!user)
    return EOPNOTSUPP;
  else if (size < 0)
    return EINVAL;
  
  pthread_mutex_lock (&user->po->np->lock);
  err = netfs_attempt_set_size (user->user, user->po->np, size);
  pthread_mutex_unlock (&user->po->np->lock);
  return err;
}
