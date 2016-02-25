/*
   Copyright (C) 1999, 2001 Free Software Foundation, Inc.
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

/* Unlock node NP and release a hard reference; if this is the last
   hard reference and there are no links to the file then request
   weak references to be dropped.  */
void
diskfs_nput (struct node *np)
{
  struct references result;

  /* While we call the diskfs_try_dropping_softrefs, we need to hold
     one reference.  We use a weak reference for this purpose, which
     we acquire by demoting our hard reference to a weak one.  */
  refcounts_demote (&np->refcounts, &result);

  if (result.hard == 0)
    {
      /* This is our cue that something akin to "last process closes file"
	 in the POSIX.1 sense happened, so make sure any pending node time
	 updates now happen in a timely fashion.  */
      diskfs_set_node_times (np);
      diskfs_lost_hardrefs (np);
      if (!np->dn_stat.st_nlink)
	{
	  if (np->sockaddr != MACH_PORT_NULL)
	    {
	      mach_port_deallocate (mach_task_self (), np->sockaddr);
	      np->sockaddr = MACH_PORT_NULL;
	    }

	  /* There are no links.  If there are soft references that
	     can be dropped, we can't let them postpone deallocation.
	     So attempt to drop them.  But that's a user-supplied
	     routine, which might result in further recursive calls to
	     the ref-counting system.  This is not a problem, as we
	     hold a weak reference ourselves. */
	  diskfs_try_dropping_softrefs (np);
	}
    }

  /* Finally get rid of our reference.  */
  refcounts_deref_weak (&np->refcounts, &result);

  if (result.hard == 0 && result.weak == 0)
    diskfs_drop_node (np);
  else
    pthread_mutex_unlock (&np->lock);
}
