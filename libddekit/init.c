/**
 * The functions regarding DDE/BSD initialization are found here.
 *
 * \author Thomas Friebel <tf13@os.inf.tu-dresden.de>
 */
#include "ddekit/thread.h"

void ddekit_init(void)
{
	extern void linux_kmem_init ();
	extern int log_init ();
	extern void interrupt_init ();

	ddekit_init_threads();
	linux_kmem_init ();
	log_init ();
	interrupt_init ();
}

