#include <stdlib.h>
#include <error.h>
#include <sys/io.h>
#include <mach.h>
#include <hurd.h>
#include <device/device.h>

#include "ddekit/resources.h"

#include "config.h"

int ddekit_request_dma(int nr) {
#if 0
	return l4io_request_dma(nr);
#endif
	return -1;
}

int ddekit_release_dma(int nr) {
#if 0
	return l4io_release_dma(nr);
#endif
	return -1;
}

/** Request an IO region
 *
 * \return 0 	success
 * \return -1   error
 */
int ddekit_request_io(ddekit_addr_t start, ddekit_addr_t count) {
	return ioperm (start, count, 1);
}

/** Release an IO region.
 *
 * \return 0 	success
 * \return <0   error
 */
int ddekit_release_io(ddekit_addr_t start, ddekit_addr_t count) {
	return ioperm (start, count, 0);
}

/* Get a memory object for the whole physical address space. */
static memory_object_t get_physmem_object(void)
{
	mach_port_t master_device_port;
	mach_port_t iopl_dev;
	memory_object_t obj;
	kern_return_t err;

	/* first open master device port */
	err = get_privileged_ports(NULL, &master_device_port);
	if (err)
		error(2, err, "get_physmem_object() can't get master device port");

	/* now use it to open iopl device */
	err = device_open(master_device_port, VM_PROT_READ | VM_PROT_WRITE, "mem", &iopl_dev);
	if (err)
		error(2, err, "get_physmem_object() can't open iopl device");

	mach_port_deallocate(mach_task_self(), master_device_port);

	/* now use that to get a memory object for the physical address space */
	err = device_map(iopl_dev, D_READ|D_WRITE, 0, ~0, &obj, 0);
	if (err)
		error(2, err, "get_physmem_object() can't obtain memory object");

	mach_port_deallocate(mach_task_self(), iopl_dev);

	return obj;
}

/** Request a memory region.
 *
 * \return vaddr virtual address of memory region
 * \return 0	 success
 * \return -1	 error
 */
int ddekit_request_mem(ddekit_addr_t start, ddekit_addr_t count, ddekit_addr_t *vaddr) {
	memory_object_t iopl_mem;
	kern_return_t err;
	*vaddr = 0;

	iopl_mem = get_physmem_object();

	err = vm_map(mach_task_self(),
		     (vm_address_t *) vaddr,
		     count,
		     0,     /* mask */
		     1,  /* anywhere */
		     iopl_mem,
		     (vm_offset_t)start,
		     0, /* copy on write */
		     VM_PROT_READ|VM_PROT_WRITE,
		     VM_PROT_READ|VM_PROT_WRITE,
		     VM_INHERIT_SHARE);
	mach_port_deallocate(mach_task_self(),iopl_mem);
	if( err )
	{
		error(0, err, "mmio_map(): can't vm_map");
		return -1;
	}
	return 0;
}

/** Release memory region.
 *
 * \return 0 success
 * \return <0 error
 */
int ddekit_release_mem(ddekit_addr_t start, ddekit_addr_t count) {
	kern_return_t err = vm_deallocate(mach_task_self(), start, count);
	if( err )
	{
		error(0, err, "mmio_unmap(): can't dealloc mmio space");
		return -1;
	}
	return 0;
}

long ddekit_random ()
{
	return random ();
}
