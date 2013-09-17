/* fsys_get_children

   Copyright (C) 2013 Free Software Foundation, Inc.

   Written by Justus Winter <4winter@informatik.uni-hamburg.de>

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
   along with the GNU Hurd.  If not, see <http://www.gnu.org/licenses/>.  */

#include "priv.h"
#include "fsys_S.h"

#include <argz.h>

/* Return any active translators bound to nodes of the receiving
   filesystem.  CHILDREN is an argz vector containing file names
   relative to the root of the receiving translator.  */
error_t
diskfs_S_fsys_get_children (fsys_t server,
			    mach_port_t reply,
			    mach_msg_type_name_t replyPoly,
			    char **children,
			    mach_msg_type_number_t *children_len)
{
  error_t err;

  struct protid *cred = ports_lookup_port (diskfs_port_bucket,
					   server,
					   diskfs_protid_class);
  if (! cred)
    return EOPNOTSUPP;

  /* check_access performs the same permission check as is normally
     done, i.e. it checks that all but the last path components are
     executable by the requesting user and that the last component is
     readable.	*/
  error_t check_access (const char *path)
  {
    error_t err;
    char *elements = NULL;
    size_t elements_len = 0;

    err = argz_create_sep (path, '/', &elements, &elements_len);
    if (err)
      return err;

    struct node *dp = diskfs_root_node;

    for (char *entry = elements;
	 entry;
	 entry = argz_next (elements, elements_len, entry))
      {
	struct node *next;
	err = diskfs_lookup (dp, entry, LOOKUP, &next, NULL, cred);

	if (dp != diskfs_root_node)
	  diskfs_nput (dp);

	if (err)
	  return err;

	dp = next;
      }

    err = fshelp_access (&dp->dn_stat, S_IRUSR, cred->user);
    diskfs_nput (dp);
    return err;
  }


  char *c = NULL;
  size_t c_len = 0;

  err = fshelp_get_active_translators (&c, &c_len, check_access);
  if (err)
    goto errout;

  err = iohelp_return_malloced_buffer (c, c_len, children, children_len);
  if (err)
    goto errout;

  c = NULL; /* c was freed by iohelp_return_malloced_buffer. */

 errout:
  free (c);
  return err;
}
