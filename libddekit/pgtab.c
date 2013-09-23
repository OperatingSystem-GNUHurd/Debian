/*
 * \brief   Virtual page-table facility
 * \author  Thomas Friebel <tf13@os.inf.tu-dresden.de>
 * \author  Christian Helmuth <ch12@os.inf.tu-dresden.de>
 * \date    2006-11-01
 *
 * This implementation uses l4rm (especially the AVL tree and userptr) to
 * manage virt->phys mappings. Each mapping region is represented by one
 * pgtab_object that is kept in the l4rm region userptr.
 *
 * For this to work, dataspaces must be attached to l4rm regions!
 */

#include <unistd.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <mach.h>
#include <pthread.h>

#include "ddekit/pgtab.h"
#include "util.h"
#include "config.h"

/* A structure of recording a region of memory. */
struct entry
{
	void *virtual;
	ddekit_addr_t physical;
	int size;
	int type;
};

static struct entry *regions;
/* The number of memory regions in the array REGIONS */
static int num_regions;
/* The size of the array REGIONS */
static int capability;
static pthread_mutex_t lock;
#define INIT_SIZE 128

/*****************************
 ** Page-table facility API **
 *****************************/

static struct entry *get_entry_from_phys (const ddekit_addr_t phys)
{
	int i;

	for (i = 0; i < num_regions; i++)
	{
		if (regions[i].physical <= phys
		    && regions[i].physical + regions[i].size > phys)
			return &regions[i];
	}
	return NULL;
}

static struct entry *get_entry_from_virt (const ddekit_addr_t virt)
{
	int i;

	for (i = 0; i < num_regions; i++)
	{
		if ((ddekit_addr_t) regions[i].virtual <= virt
		    && ((ddekit_addr_t) regions[i].virtual)
		    + regions[i].size > virt)
			return &regions[i];
	}
	return NULL;
}

/**
 * Get physical address for virtual address
 *
 * \param virtual  virtual address
 * \return physical address or 0
 */
ddekit_addr_t ddekit_pgtab_get_physaddr(const void *virtual)
{
	struct entry *e;
	pthread_mutex_lock (&lock);
	e = get_entry_from_virt ((ddekit_addr_t) virtual);
	if (e)
	{
		ddekit_addr_t phys = e->physical + (virtual - e->virtual);
		pthread_mutex_unlock (&lock);
		return phys;
	}
	pthread_mutex_unlock (&lock);

	ddekit_printf ("a virtual address %p doesn't exist.\n", virtual);
	return -1;
}

/**
 * Get virtual address for physical address
 *
 * \param physical  physical address
 * \return virtual address or 0
 */
ddekit_addr_t ddekit_pgtab_get_virtaddr(const ddekit_addr_t physical)
{
	struct entry *e;

	pthread_mutex_lock (&lock);
	e = get_entry_from_phys (physical);
	if (e)
	{
		ddekit_addr_t virt = (ddekit_addr_t) e->virtual 
			+ (physical - e->physical);
		pthread_mutex_unlock (&lock);
		return virt;
	}
	pthread_mutex_unlock (&lock);


	ddekit_printf ("a physical address %p doesn't exist.\n", physical);
	return -1;
}

// TODO
int ddekit_pgtab_get_type(const void *virtual)
{
#if 0
	/* find pgtab object */
	struct pgtab_object *p = l4rm_get_userptr(virtual);
	if (!p) {
		/* XXX this is verbose */
		LOG_Error("no virt->phys mapping for %p", virtual);
		return -1;
	}

	return p->type;
#endif
	UNIMPL;
	return 0;
}

int ddekit_pgtab_get_size(const void *virtual)
{
	struct entry *e;
	pthread_mutex_lock (&lock);
	e = get_entry_from_virt ((ddekit_addr_t) virtual);
	if (e)
	{
		int size = e->size;
		pthread_mutex_unlock (&lock);
		return size;
	}
	pthread_mutex_unlock (&lock);
	return 0;
}


/**
 * Clear virtual->physical mapping for VM region
 *
 * \param virtual   virtual start address for region
 * \param type      pgtab type for region
 */
void ddekit_pgtab_clear_region(void *virtual, int type)
{
	struct entry *e;

	pthread_mutex_lock (&lock);
	e = get_entry_from_virt ((ddekit_addr_t) virtual);
	if (e)
	{
		*e = regions[num_regions - 1];
		num_regions--;
	}
	pthread_mutex_unlock (&lock);
}


/**
 * Set virtual->physical mapping for VM region
 *
 * \param virtual   virtual start address for region
 * \param physical  physical start address for region
 * \param pages     number of pages in region
 * \param type      pgtab type for region
 */
void ddekit_pgtab_set_region(void *virtual, ddekit_addr_t physical, int pages, int type)
{
	ddekit_pgtab_set_region (virtual, physical, pages * getpagesize (), type);
}

void ddekit_pgtab_set_region_with_size(void *virt, ddekit_addr_t phys, int size, int type)
{
	pthread_mutex_lock (&lock);
	if (num_regions == capability)
	{
		capability *= 2;
		regions = realloc (regions, capability * sizeof (struct entry));
		if (regions == NULL)
			error (2, errno, "realloc");
	}
	regions[num_regions].virtual = virt;
	regions[num_regions].physical = phys;
	regions[num_regions].size = size;
	regions[num_regions].type = type;
	num_regions++;
	pthread_mutex_unlock (&lock);
}

int pgtab_init ()
{
	capability = INIT_SIZE;
	regions = malloc (sizeof (struct entry) * capability);
	pthread_mutex_init (&lock, NULL);
	return 0;
}
