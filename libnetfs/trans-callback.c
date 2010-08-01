/* Callback functions for starting translators

   Copyright (C) 1995,96,97,2001,02 Free Software Foundation, Inc.

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

#include "priv.h"
#include <fcntl.h>

/* Callback function needed for calls to fshelp_fetch_root.  See
   <hurd/fshelp.h> for the interface description.  */
static error_t
_netfs_translator_callback1_fn (void *cookie1, void *cookie2,
				 uid_t *uid, gid_t *gid,
				 char **argz, size_t *argz_len)
{
  error_t err;
  struct node *np = cookie1;

  if (! (np->nn_stat.st_mode & S_IPTRANS))
    return ENOENT;

  err = netfs_get_translator (np, argz, argz_len);
  if (err)
    {
      assert (err != EOPNOTSUPP);
      return err;
    }

  *uid = np->nn_stat.st_uid;
  *gid = np->nn_stat.st_gid;

  return 0;
}

/* Callback function needed for calls to fshelp_fetch_root.  See
   <hurd/fshelp.h> for the interface description.  */
static error_t
_netfs_translator_callback2_fn (void *cookie1, void *cookie2, int flags,
				mach_port_t *underlying,
				mach_msg_type_name_t *underlying_type)
{
  error_t err;
  struct protid *cred;
  struct node *node = cookie1;
  struct iouser *user;

  err = iohelp_create_simple_iouser (&user, node->nn_stat.st_uid,
				   node->nn_stat.st_gid);
  if (err)
    return err;

  cred = netfs_make_protid (netfs_make_peropen (node, flags, cookie2),
			    user);
  if (cred)
    {
      *underlying = ports_get_right (cred);
      *underlying_type = MACH_MSG_TYPE_MAKE_SEND;
      ports_port_deref (cred);
      return 0;
    }
  else
    return errno;
}

fshelp_fetch_root_callback1_t _netfs_translator_callback1 =
  _netfs_translator_callback1_fn;
fshelp_fetch_root_callback2_t _netfs_translator_callback2 =
  _netfs_translator_callback2_fn;
