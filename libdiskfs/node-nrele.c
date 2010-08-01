/* 
   Copyright (C) 1999 Free Software Foundation, Inc.
   Written by Thomas Bushnell, BSG.

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

#include "priv.h"

/* Release a hard reference on NP.  If NP is locked by anyone, then
   this cannot be the last hard reference (because you must hold a
   hard reference in order to hold the lock).  If this is the last
   hard reference and there are no links, then request soft references
   to be dropped.  */
void
diskfs_nrele (struct node *np)
{
  int tried_drop_softrefs = 0;

 loop:
  spin_lock (&diskfs_node_refcnt_lock);
  assert (np->references);
  np->references--;
  if (np->references + np->light_references == 0)
    {
      mutex_lock (&np->lock);
      diskfs_drop_node (np);
    }
  else if (np->references == 0)
    {
      mutex_lock (&np->lock);
      spin_unlock (&diskfs_node_refcnt_lock);
      diskfs_lost_hardrefs (np);
      if (!np->dn_stat.st_nlink && !tried_drop_softrefs)
	{
	  /* Same issue here as in nput; see that for explanation */
	  spin_lock (&diskfs_node_refcnt_lock);
	  np->references++;
	  spin_unlock (&diskfs_node_refcnt_lock);

	  diskfs_try_dropping_softrefs (np);
	  tried_drop_softrefs = 1;

	  /* Now we can drop the reference back... */
	  mutex_unlock (&np->lock);
	  goto loop;
	}
      mutex_unlock (&np->lock);
    }
  else
    spin_unlock (&diskfs_node_refcnt_lock);
}
