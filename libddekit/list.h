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

#ifndef __LIST_H__
#define __LIST_H__

#include <stddef.h>

struct list 
{
  struct list *next, *prev;
};

void entry_init (struct list *entry);
void add_entry_head (struct list *head, struct list *entry);
void add_entry_end (struct list *head, struct list *entry);
struct list *remove_entry_head (struct list *head);
struct list *remove_entry_end (struct list *head);
void remove_entry (struct list *entry);

#define LIST_HEADER(head) struct list head = {&head, &head}
#define EMPTY_LIST(head) ((head)->next == (head))
#define LIST_ENTRY(entry, type, field) ((type *) (((char *) entry) - offsetof (type, field)))

#endif
