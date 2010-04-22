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

#include <mach.h>

#include "ddekit/pgtab.h"
#include "util.h"
#include "config.h"

/*****************************
 ** Page-table facility API **
 *****************************/

/**
 * Get physical address for virtual address
 *
 * \param virtual  virtual address
 * \return physical address or 0
 */
ddekit_addr_t ddekit_pgtab_get_physaddr(const void *virtual)
{
	extern int virt_to_phys (vm_address_t addr);
	return virt_to_phys ((vm_address_t) virtual);
}

/**
 * Get virtual address for physical address
 *
 * \param physical  physical address
 * \return virtual address or 0
 */
ddekit_addr_t ddekit_pgtab_get_virtaddr(const ddekit_addr_t physical)
{
	extern int phys_to_virt (vm_address_t addr);
	return phys_to_virt (physical);
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

//TODO
int ddekit_pgtab_get_size(const void *virtual)
{
#if 0
	/* find pgtab object */
	struct pgtab_object *p = l4rm_get_userptr(virtual);
	if (!p) {
		/* XXX this is verbose */
		LOG_Error("no virt->phys mapping for %p", virtual);
		return -1;
	}

	return p->size;
#endif
	UNIMPL;
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
	UNIMPL;
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
	UNIMPL;
}

void ddekit_pgtab_set_region_with_size(void *virt, ddekit_addr_t phys, int size, int type)
{
	UNIMPL;
}

