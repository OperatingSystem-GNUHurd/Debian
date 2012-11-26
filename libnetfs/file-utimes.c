/* 
   Copyright (C) 1995, 1996, 1999 Free Software Foundation, Inc.
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
netfs_S_file_utimes (struct protid *user,
		     time_value_t atimein,
		     time_value_t mtimein)
{
  struct timespec atime, mtime;
  error_t err;

  if (atimein.microseconds != -1)
    {
      atime.tv_sec = atimein.seconds;
      atime.tv_nsec = atimein.microseconds * 1000;
    }
  
  if (mtimein.microseconds != -1)
    {
      mtime.tv_sec = mtimein.seconds;
      mtime.tv_nsec = mtimein.microseconds * 1000;
    }
  
  if (!user)
    return EOPNOTSUPP;
  
  pthread_mutex_lock (&user->po->np->lock);
  err = netfs_attempt_utimes (user->user, user->po->np, 
			      atimein.microseconds != -1 ? &atime : 0, 
			      mtimein.microseconds != -1 ? &mtime : 0);
  pthread_mutex_unlock (&user->po->np->lock);
  return err;
}
