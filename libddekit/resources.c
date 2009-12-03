#include <sys/io.h>

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

/** Request a memory region.
 *
 * \return vaddr virtual address of memory region
 * \return 0	 success
 * \return -1	 error
 */
int ddekit_request_mem(ddekit_addr_t start, ddekit_addr_t count, ddekit_addr_t *vaddr) {
#if 0
	ddekit_addr_t v;

	v = l4io_request_mem_region(start, count, 0);
	if (v) {
		*vaddr = v;
		return 0;
	} else
		return -1;
#endif
	return -1;
}

/** Release memory region.
 *
 * \return 0 success
 * \return <0 error
 */
int ddekit_release_mem(ddekit_addr_t start, ddekit_addr_t count) {
#if 0
	return l4io_release_mem_region(start, count);
#endif
	return -1;
}
