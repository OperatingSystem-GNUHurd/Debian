/*
 * \brief   Memory subsystem
 * \author  Thomas Friebel <tf13@os.inf.tu-dresden.de>
 * \author  Christian Helmuth <ch12@os.inf.tu-dresden.de>
 * \date    2006-11-03
 *
 * The memory subsystem provides the backing store for DMA-able memory via
 * large malloc and slabs.
 *
 * FIXME check thread-safety and add locks where appropriate
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <error.h>
#include <string.h>
#include <sys/mman.h>

#include "mach_U.h"
#include "libhurd-slab/slab.h"

#include "util.h"
#include "ddekit/memory.h"
#include "ddekit/panic.h"
#include "ddekit/pgtab.h"

#define CACHE_LINE_SIZE 32

/****************
 ** Page cache **
 ****************/

/* FIXME revisit two-linked-lists approach - we could get rid of the
 * pcache_free lists, because these descriptors can also be allocated
 * whenever the need arises. (XXX: Check how this hurts performance.)
 */

/*****************************************************************************
  DDEKit maintains a list of free pages that can be used for growing
  existing DDEKit slabs. We distinguish between 2 types of pages: the ones
  that have been allocated from physically contiguous dataspaces and the the
  ones that have not.

  For each type of cache, we maintain two lists: a list of free pages that
  can be used to grow a cache (pcache_used) and a list of free pcache entries
  that can be used to store pages when they get freed (pcache_free). The
  reason for this is that by using these two lists, cache operations can be
  done atomically using cmpxchg.

  The function ddekit_slab_setup_page_cache() is used to tune the number of
  cached pages.
 ******************************************************************************/

/* page cache to minimize allocations from external servers */
struct ddekit_pcache
{
	struct ddekit_pcache *next;
	void *page;
	int contig;
};

/**
 * Setup page cache for all slabs
 *
 * \param pages  maximal number of memory pages
 *
 * If the maximal number of unused pages is exceeded, subsequent deallocation
 * will be freed at the memory server. This page cache caches pages from all
 * slabs.
 */
void ddekit_slab_setup_page_cache(unsigned pages)
{
	UNIMPL;
}

/*******************************
 ** Slab cache implementation **
 *******************************/

/* ddekit slab facilitates l4slabs */
struct ddekit_slab
{
  struct hurd_slab_space space;
};

/**
 * Allocate object in slab
 */
void *ddekit_slab_alloc(struct ddekit_slab * slab)
{
	void *buffer;
	error_t err = hurd_slab_alloc (&slab->space, &buffer);
	return err ? NULL : buffer;
}


/**
 * Free object in slab
 */
void  ddekit_slab_free(struct ddekit_slab * slab, void *objp)
{
	hurd_slab_dealloc (&slab->space, objp);
}


/**
 * Store user pointer in slab cache
 */
void  ddekit_slab_set_data(struct ddekit_slab * slab, void *data)
{
#if 0
	l4slab_set_data(&slab->cache, data);
#endif
	UNIMPL;
}


/**
 * Read user pointer from slab cache
 */
void *ddekit_slab_get_data(struct ddekit_slab * slab)
{
#if 0
	return l4slab_get_data(&slab->cache);
#endif
	UNIMPL;
	return NULL;
}


/**
 * Destroy slab cache
 *
 * \param slab  pointer to slab cache structure
 */
void  ddekit_slab_destroy (struct ddekit_slab * slab)
{
	hurd_slab_free ((hurd_slab_space_t) slab);
}

error_t allocate_buffer (void *hook, size_t size, void **ptr)
{
	*ptr = ddekit_large_malloc (size);
	if (*ptr == NULL)
	  return ENOMEM;
	return 0;
}

error_t deallocate_buffer (void *hook, void *buffer, size_t size)
{
	ddekit_large_free (buffer);
	return 0;
}

/**
 * Initialize slab cache
 *
 * \param size          size of cache objects
 * \param contiguous    make this slab use physically contiguous memory
 *
 * \return pointer to new slab cache or 0 on error
 */
struct ddekit_slab * ddekit_slab_init(unsigned size, int contiguous)
{
	struct ddekit_slab * slab;
	error_t err;

	if (contiguous)
	  err = hurd_slab_create (size, CACHE_LINE_SIZE, allocate_buffer,
				  deallocate_buffer, NULL, NULL, NULL,
				  (hurd_slab_space_t *) &slab);
	else
	  /* If the object isn't used by DMA,
	   * we can use all default settings. */
	  err = hurd_slab_create (size, 0, NULL, NULL, NULL, NULL, NULL,
				  (hurd_slab_space_t *) &slab);
	if (err)
	  {
	    error (2, err, "hurd_slab_create");
	  }
	return slab;
}


/**********************************
 ** Large block memory allocator **
 **********************************/

/**
 * Free large block of memory
 *
 * This is no useful for allocation < page size.
 *
 * TODO The freed memory can be cached and will be still accessible (
 * no page fault when accessed). I hope it won't caused any troubles.
 */
void ddekit_large_free(void *objp)
{
  int err;
  int size = ddekit_pgtab_get_size (objp);
  ddekit_pgtab_clear_region (objp, 0);
  err = munmap (objp, size);
  if (err < 0)
    error (0, errno, "munmap");
}


/**
 * Allocate large block of memory
 *
 * This is no useful for allocation < page size.
 */
void *ddekit_large_malloc(int size)
{
  error_t err;
  vm_address_t vstart, pstart;
  extern mach_port_t priv_host;

  /* Allocate memory.  */
  err = vm_dma_buff_alloc (priv_host, mach_task_self (),
			   size, &vstart, &pstart);
  if (err)
    {
      error (0, err, "vm_dma_buff_alloc");
      vstart = 0;
    }
  else
    ddekit_pgtab_set_region_with_size ((void *) vstart, pstart, size, 0);

  return (void *) vstart;
}


/**
 * Allocate large block of memory (special interface)
 *
 * This is no useful for allocation < page size.
 *
 * FIXME implementation missing...
 */
void *ddekit_contig_malloc(unsigned long size,
                           unsigned long low, unsigned long high,
                           unsigned long alignment, unsigned long boundary)
{
#if 0
	void *res;
	int err;
	int pages;
	l4_size_t tmp;
	l4dm_mem_addr_t dm_paddr;

	size  = l4_round_page(size);
	pages = size >> L4_PAGESHIFT;

	res = l4dm_mem_allocate_named(size, L4DM_CONTIGUOUS | L4DM_PINNED | L4RM_MAP | L4RM_LOG2_ALIGNED);

	if (res) {
		/* check if res meets low/high/alignment/boundary
		 * and panic if it is not the case
		 * XXXY
		 */

		/* get physical address */
		err = l4dm_mem_phys_addr(res, 1, &dm_paddr, 1, &tmp);
		if (err != 1)
			ddekit_debug("contigmalloc: error getting physical address of new page!\n");
	
		/* set PTE */
		ddekit_set_ptes(res, dm_paddr.addr, pages, PTE_TYPE_CONTIG);
	}

	return res;
#else
	ddekit_debug("%s: not implemented\n", __func__);
	return 0;
#endif
}
