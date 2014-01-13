/* demuxer.c - Message demuxer for console client library.
   Copyright (C) 2002 Free Software Foundation, Inc.
   Written by Marcus Brinkmann.

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

#include "cons.h"

int
cons_demuxer (mach_msg_header_t *inp, mach_msg_header_t *outp)
{
  int cons_fs_notify_server (mach_msg_header_t *inp, mach_msg_header_t *outp);

  return (cons_fs_notify_server (inp, outp));
}
