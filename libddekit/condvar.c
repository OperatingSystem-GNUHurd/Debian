/**
 * Unchecked (no BSD invariants) condition variable implementation for
 * dde-internal use. Written from scratch.
 *
 * \author Thomas Friebel <tf13@os.inf.tu-dresden.de>
 */
#include <cthreads.h>

#include "ddekit/condvar.h"

struct ddekit_condvar {
	struct condition cond;
};

ddekit_condvar_t *ddekit_condvar_init() {
	ddekit_condvar_t *cvp;

	cvp = condition_alloc ();
	condition_init (cvp);

	return cvp;
}

void ddekit_condvar_wait(ddekit_condvar_t *cvp, ddekit_lock_t *mp) {
	/* This isn't nice. The encapsulation is broken.
	 * TODO I can merge the two files condvar.c and lock.c. */
	condition_wait (&cvp->cond, (struct mutex *) mp);
}

int ddekit_condvar_wait_timed(ddekit_condvar_t *cvp,
			      ddekit_lock_t *mp, int timo) {
	// TODO currently just let it like this.
	ddekit_condvar_wait (cvp, mp);
	return 0;
}

void ddekit_condvar_signal(ddekit_condvar_t *cvp)
{
	condition_signal (&cvp->cond);
}

void ddekit_condvar_broadcast(ddekit_condvar_t *cvp) {
	condition_broadcast (&cvp->cond);
}
