/*
 * Linux memory allocation.
 * 
 * Copyright (C) 1996 The University of Utah and the Computer Systems
 * Laboratory at the University of Utah (CSL)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *      Author: Shantanu Goel, University of Utah CSL
 *
 */

#include <string.h>
#include <error.h>
#include <stdio.h>
#include <assert.h>
#include "mach_U.h"
#include <hurd.h>
#include <cthreads.h>

#include "util.h"
#include "vm_param.h"

#include "ddekit/panic.h"

#define debug ddekit_debug

extern int printf (const char *, ...);

/* Amount of memory to reserve for Linux memory allocator.
   We reserve 64K chunks to stay within DMA limits.
   Increase MEM_CHUNKS if the kernel is running out of memory.  */
#define MEM_CHUNK_SIZE	(64 * 1024)
#define MEM_CHUNKS	7
#define MEM_CHUNKS_TOTAL (MEM_CHUNKS + 5)

/* round up the size at alignment of page size. */
#define ROUND_UP(size) ((size) + __vm_page_size - 1) & (~(__vm_page_size - 1))

/* Mininum amount that linux_kmalloc will allocate.  */
#define MIN_ALLOC	12

#ifndef NBPW
#define NBPW		32
#endif

/* Memory block header.  */
struct blkhdr
{
  unsigned short free;	/* 1 if block is free */
  unsigned short size;	/* size of block */
};

/* This structure heads a page allocated by linux_kmalloc.  */
struct pagehdr
{
  unsigned size;		/* size (multiple of PAGE_SIZE) */
  struct pagehdr *next;	/* next header in list */
};

/* This structure describes a memory chunk.  */
struct chunkhdr
{
  vm_address_t start;	/* start address */
  vm_address_t pstart;	/* start physical address */
  vm_address_t end;		/* end address */
  unsigned long bitmap;	/* busy/free bitmap of pages */
};

static unsigned long __get_free_pages (unsigned long order, int dma);
static void free_pages (unsigned long addr, unsigned long order);

static struct mutex mem_lock = MUTEX_INITIALIZER;

/* Chunks from which pages are allocated. 
 * The extra slots are used to hold the huge chunks (> MEM_CHUNKS_SIZE)
 * which are allocated by the user. */
static struct chunkhdr pages_free[MEM_CHUNKS_TOTAL];

/* Memory list maintained by linux_kmalloc.  */
static struct pagehdr *memlist;

static mach_port_t priv_host;

/* Some statistics.  */
int num_block_coalesce = 0;
int num_page_collect = 0;
int linux_mem_avail;

int virt_to_phys (vm_address_t addr)
{
  int i;

  for (i = 0; i < MEM_CHUNKS_TOTAL; i++)
    {
      if (pages_free[i].start <= addr && pages_free[i].end > addr)
	return addr - pages_free[i].start + pages_free[i].pstart;
    }
  debug ("an address not in any chunks.");
  return -1;
}

int phys_to_virt (vm_address_t addr)
{
#define CHUNK_SIZE(chunk) ((chunk)->end - (chunk)->start)
  int i;

  for (i = 0; i < MEM_CHUNKS_TOTAL; i++)
    {
      if (pages_free[i].pstart <= addr
	  && pages_free[i].pstart + CHUNK_SIZE (pages_free + i) > addr)
	return addr - pages_free[i].pstart + pages_free[i].start;
    }
  debug ("an address not in any chunks.");
  return -1;
}

/* Initialize the Linux memory allocator.  */
void
linux_kmem_init ()
{
  int i, j;
  error_t err;

  err = get_privileged_ports (&priv_host, NULL);
  if (err)
    error (2, err, "get_privileged_ports");

  for (i = 0; i < MEM_CHUNKS; i++)
    {
      error_t err;

      /* Allocate memory.  */
      err = vm_dma_buff_alloc (priv_host, mach_task_self (),
			       MEM_CHUNK_SIZE, &pages_free[i].start,
			       &pages_free[i].pstart);
      if (err)
	abort ();

      assert (pages_free[i].start);

      pages_free[i].end = pages_free[i].start + MEM_CHUNK_SIZE;
      assert (pages_free[i].pstart + MEM_CHUNK_SIZE <= 16 * 1024 * 1024);

      /* Initialize free page bitmap.  */
      pages_free[i].bitmap = 0;
      j = MEM_CHUNK_SIZE >> PAGE_SHIFT;
      while (--j >= 0)
	pages_free[i].bitmap |= 1 << j;
    }

  /* Initialize the space for extra slots. */
  memset (pages_free + i, 0,
	  sizeof (pages_free[0]) * (MEM_CHUNKS_TOTAL - MEM_CHUNKS));

  linux_mem_avail = (MEM_CHUNKS * MEM_CHUNK_SIZE) >> PAGE_SHIFT;
}

/* Return the number by which the page size should be
   shifted such that the resulting value is >= SIZE.  */
static unsigned long
get_page_order (int size)
{
  unsigned long order;

  for (order = 0; (PAGE_SIZE << order) < size; order++)
    ;
  return order;
}

#ifdef LINUX_DEV_DEBUG
static void
check_page_list (int line)
{
  unsigned size;
  struct pagehdr *ph;
  struct blkhdr *bh;

  for (ph = memlist; ph; ph = ph->next)
    {
      if ((int) ph & PAGE_MASK)
	panic ("%s:%d: page header not aligned", __FILE__, line);

      size = 0;
      bh = (struct blkhdr *) (ph + 1);
      while (bh < (struct blkhdr *) ((void *) ph + ph->size))
	{
	  size += bh->size + sizeof (struct blkhdr);
	  bh = (void *) (bh + 1) + bh->size;
	}

      if (size + sizeof (struct pagehdr) != ph->size)
	panic ("%s:%d: memory list destroyed", __FILE__, line);
    }
}
#else
#define check_page_list(line)
#endif

/* Merge adjacent free blocks in the memory list.  */
static void
coalesce_blocks ()
{
  struct pagehdr *ph;
  struct blkhdr *bh, *bhp, *ebh;

  num_block_coalesce++;

  for (ph = memlist; ph; ph = ph->next)
    {
      bh = (struct blkhdr *) (ph + 1);
      ebh = (struct blkhdr *) ((void *) ph + ph->size);
      while (1)
	{
	  /* Skip busy blocks.  */
	  while (bh < ebh && !bh->free)
	    bh = (struct blkhdr *) ((void *) (bh + 1) + bh->size);
	  if (bh == ebh)
	    break;

	  /* Merge adjacent free blocks.  */
	  while (1)
	    {
	      bhp = (struct blkhdr *) ((void *) (bh + 1) + bh->size);
	      if (bhp == ebh)
		{
		  bh = bhp;
		  break;
		}
	      if (!bhp->free)
		{
		  bh = (struct blkhdr *) ((void *) (bhp + 1) + bhp->size);
		  break;
		}
	      bh->size += bhp->size + sizeof (struct blkhdr);
	    }
	}
    }
}

/* Allocate SIZE bytes of memory.
   The PRIORITY parameter specifies various flags
   such as DMA, atomicity, etc.  It is not used by Mach.  */
void *
linux_kmalloc (unsigned int size, int priority)
{
  int order, coalesced = 0;
  struct pagehdr *ph;
  struct blkhdr *bh, *new_bh;

  if (size < MIN_ALLOC)
    size = MIN_ALLOC;
  else
    size = (size + sizeof (int) - 1) & ~(sizeof (int) - 1);

  mutex_lock (&mem_lock);

  if (size > (MEM_CHUNK_SIZE - sizeof (struct pagehdr)
	      - sizeof (struct blkhdr)))
    {
      error_t err;
      int i;

      /* Find an extra slot. */
      for (i = MEM_CHUNKS; i < MEM_CHUNKS_TOTAL; i++)
	if (pages_free[i].end == 0)
	  break;

      // TODO use a dynamically allocated memory to
      // record directly allocated large memory.
      assert (i < MEM_CHUNKS_TOTAL);
      size = ROUND_UP (size);
      err = vm_dma_buff_alloc (priv_host, mach_task_self (), size,
			       &pages_free[i].start, &pages_free[i].pstart);
      if (!err)
	pages_free[i].end = pages_free[i].start + size;
      mutex_unlock (&mem_lock);
      fprintf (stderr, "allocate %d bytes at (virt: %x, phys: %x), slot %d\n",
	      size, pages_free[i].start, pages_free[i].pstart, i);

      return err ? NULL : (void *) pages_free[i].start;
    }

again:
  check_page_list (__LINE__);

  /* Walk the page list and find the first free block with size
     greater than or equal to the one required.  */
  for (ph = memlist; ph; ph = ph->next)
    {
      bh = (struct blkhdr *) (ph + 1);
      while (bh < (struct blkhdr *) ((void *) ph + ph->size))
	{
	  if (bh->free && bh->size >= size)
	    {
	      bh->free = 0;
	      if (bh->size - size >= MIN_ALLOC + sizeof (struct blkhdr))
		{
		  /* Split the current block and create a new free block.  */
		  new_bh = (void *) (bh + 1) + size;
		  new_bh->free = 1;
		  new_bh->size = bh->size - size - sizeof (struct blkhdr);
		  bh->size = size;
		}

	      check_page_list (__LINE__);

	      mutex_unlock (&mem_lock);
	      return bh + 1;
	    }
	  bh = (void *) (bh + 1) + bh->size;
	}
    }

  check_page_list (__LINE__);

  /* Allocation failed; coalesce free blocks and try again.  */
  if (!coalesced)
    {
      coalesce_blocks ();
      coalesced = 1;
      goto again;
    }

  /* Allocate more pages.  */
  order = get_page_order (size
			  + sizeof (struct pagehdr)
			  + sizeof (struct blkhdr));
  ph = (struct pagehdr *) __get_free_pages (order, ~0UL);
  if (!ph)
    {
      mutex_unlock (&mem_lock);
      return NULL;
    }

  ph->size = PAGE_SIZE << order;
  ph->next = memlist;
  memlist = ph;
  bh = (struct blkhdr *) (ph + 1);
  bh->free = 0;
  bh->size = ph->size - sizeof (struct pagehdr) - sizeof (struct blkhdr);
  if (bh->size - size >= MIN_ALLOC + sizeof (struct blkhdr))
    {
      new_bh = (void *) (bh + 1) + size;
      new_bh->free = 1;
      new_bh->size = bh->size - size - sizeof (struct blkhdr);
      bh->size = size;
    }

  check_page_list (__LINE__);

  mutex_unlock (&mem_lock);
  return bh + 1;
}

/* Free memory P previously allocated by linux_kmalloc.  */
void
linux_kfree (void *p)
{
  struct blkhdr *bh;
  struct pagehdr *ph;
  int i;

  assert (((int) p & (sizeof (int) - 1)) == 0);

  mutex_lock (&mem_lock);
  
  for (i = MEM_CHUNKS; i < MEM_CHUNKS_TOTAL; i++)
    {
      if ((vm_address_t) p == pages_free[i].start)
	{
	  // TODO I think the page cannot be deallocated.
	  vm_deallocate (mach_task_self (), (vm_address_t) p,
			 pages_free[i].end - pages_free[i].start);
	  memset (pages_free + i, 0, sizeof (pages_free[i]));
	  mutex_unlock (&mem_lock);
	  return;
	}
    }

  check_page_list (__LINE__);

  for (ph = memlist; ph; ph = ph->next)
    if (p >= (void *) ph && p < (void *) ph + ph->size)
      break;

  assert (ph);

  bh = (struct blkhdr *) p - 1;

  assert (!bh->free);
  assert (bh->size >= MIN_ALLOC);
  assert ((bh->size & (sizeof (int) - 1)) == 0);

  bh->free = 1;

  check_page_list (__LINE__);

  mutex_unlock (&mem_lock);
}

/* Free any pages that are not in use.
   Called by __get_free_pages when pages are running low.  */
static void
collect_kmalloc_pages ()
{
  struct blkhdr *bh;
  struct pagehdr *ph, **prev_ph;

  check_page_list (__LINE__);

  coalesce_blocks ();

  check_page_list (__LINE__);

  ph = memlist;
  prev_ph = &memlist;
  while (ph)
    {
      bh = (struct blkhdr *) (ph + 1);
      if (bh->free && (void *) (bh + 1) + bh->size == (void *) ph + ph->size)
	{
	  *prev_ph = ph->next;
	  free_pages ((unsigned long) ph, get_page_order (ph->size));
	  ph = *prev_ph;
	}
      else
	{
	  prev_ph = &ph->next;
	  ph = ph->next;
	}
    }

  check_page_list (__LINE__);
}

/* Allocate ORDER + 1 number of physically contiguous pages.
   PRIORITY and DMA are not used in Mach.
   NOTE: mem_lock has been held.

   XXX: This needs to be dynamic.  To do that we need to make
   the Mach page manipulation routines interrupt safe and they
   must provide machine dependant hooks.  */
unsigned long
__get_free_pages (unsigned long order, int dma)
{
  int i, pages_collected = 0;
  unsigned bits, off, j, len;

  assert ((PAGE_SIZE << order) <= MEM_CHUNK_SIZE);

  /* Construct bitmap of contiguous pages.  */
  bits = 0;
  j = 0;
  len = 0;
  while (len < (PAGE_SIZE << order))
    {
      bits |= 1 << j++;
      len += PAGE_SIZE;
    }

again:

  /* Search each chunk for the required number of contiguous pages.  */
  for (i = 0; i < MEM_CHUNKS; i++)
    {
      off = 0;
      j = bits;
      while (MEM_CHUNK_SIZE - off >= (PAGE_SIZE << order))
	{
	  if ((pages_free[i].bitmap & j) == j)
	    {
	      pages_free[i].bitmap &= ~j;
	      linux_mem_avail -= order + 1;
	      return pages_free[i].start + off;
	    }
	  j <<= 1;
	  off += PAGE_SIZE;
	}
    }

  /* Allocation failed; collect kmalloc and buffer pages
     and try again.  */
  if (!pages_collected)
    {
      num_page_collect++;
      collect_kmalloc_pages ();
      pages_collected = 1;
      goto again;
    }

  printf ("%s:%d: __get_free_pages: ran out of pages\n", __FILE__, __LINE__);

  return 0;
}

/* Free ORDER + 1 number of physically
   contiguous pages starting at address ADDR.  */
void
free_pages (unsigned long addr, unsigned long order)
{
  int i;
  unsigned bits, len, j;

  assert ((addr & PAGE_MASK) == 0);

  for (i = 0; i < MEM_CHUNKS; i++)
    if (addr >= pages_free[i].start && addr < pages_free[i].end)
      break;

  assert (i < MEM_CHUNKS);

  /* Contruct bitmap of contiguous pages.  */
  len = 0;
  j = 0;
  bits = 0;
  while (len < (PAGE_SIZE << order))
    {
      bits |= 1 << j++;
      len += PAGE_SIZE;
    }
  bits <<= (addr - pages_free[i].start) >> PAGE_SHIFT;

  mutex_lock (&mem_lock);

  assert ((pages_free[i].bitmap & bits) == 0);

  pages_free[i].bitmap |= bits;
  linux_mem_avail += order + 1;
  mutex_unlock (&mem_lock);
}
