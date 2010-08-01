/* Append current command line arguments

   Copyright (C) 1996 Free Software Foundation

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

#include "priv.h"

/* Append to the malloced string *ARGZ of length *ARGZ_LEN a NUL-separated
   list of the arguments to this translator.  */
error_t
trivfs_append_args (struct trivfs_control *fsys,
		    char **argz, size_t *argz_len)
{
  return EOPNOTSUPP;
}
