/* 
   Copyright (C) 2009 Free Software Foundation, Inc.
   Written by Zheng Da.

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

/* This file implements a double linked list. */

#include "list.h"

void entry_init (struct list *entry)
{
  entry->next = entry;
  entry->prev = entry;
}

void add_entry_head (struct list *head, struct list *entry)
{
  entry->next = head->next;
  head->next->prev = entry;
  head->next = entry;
  entry->prev = head;
}

void add_entry_end (struct list *head, struct list *entry)
{
  entry->next = head;
  entry->prev = head->prev;
  head->prev->next = entry;
  head->prev = entry;
}

void remove_entry (struct list *entry)
{
  entry->next->prev = entry->prev;
  entry->prev->next = entry->next;
}

