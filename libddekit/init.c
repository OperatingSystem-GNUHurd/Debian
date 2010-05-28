/**
 * The functions regarding DDE/BSD initialization are found here.
 *
 * \author Thomas Friebel <tf13@os.inf.tu-dresden.de>
 */
#include <error.h>
#include <mach.h>
#include <hurd.h>

#include "ddekit/thread.h"

mach_port_t priv_host;

void ddekit_init(void)
{
	extern void linux_kmem_init ();
	extern int log_init ();
	extern void interrupt_init ();
	extern int pgtab_init ();
	error_t err;

	err = get_privileged_ports (&priv_host, NULL);
	if (err)
		error (2, err, "get_privileged_ports");

	ddekit_init_threads();
	pgtab_init ();
	log_init ();
	interrupt_init ();
}

