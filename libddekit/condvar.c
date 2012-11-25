/**
 * Unchecked (no BSD invariants) condition variable implementation for
 * dde-internal use. Written from scratch.
 *
 * \author Thomas Friebel <tf13@os.inf.tu-dresden.de>
 */
#include <pthread.h>

#include "ddekit/memory.h"
#include "ddekit/condvar.h"

struct ddekit_condvar {
	pthread_cond_t cond;
};

ddekit_condvar_t *ddekit_condvar_init() {
	ddekit_condvar_t *cvp;

	cvp = ddekit_simple_malloc (sizeof (*cvp));

	if (cvp == NULL)
		return NULL;

	pthread_cond_init (&cvp->cond, NULL);
	return cvp;
}

void ddekit_condvar_deinit(ddekit_condvar_t *cvp) {
	ddekit_simple_free (cvp);
}

void ddekit_condvar_wait(ddekit_condvar_t *cvp, ddekit_lock_t *mp) {
	/* This isn't nice. The encapsulation is broken.
	 * TODO I can merge the two files condvar.c and lock.c. */
	pthread_cond_wait (&cvp->cond, (pthread_mutex_t *) *mp);
}

int ddekit_condvar_wait_timed(ddekit_condvar_t *cvp,
			      ddekit_lock_t *mp, int timo) {
	// TODO currently just let it like this.
	ddekit_condvar_wait (cvp, mp);
	return 0;
}

void ddekit_condvar_signal(ddekit_condvar_t *cvp)
{
	pthread_cond_signal (&cvp->cond);
}

void ddekit_condvar_broadcast(ddekit_condvar_t *cvp) {
	pthread_cond_broadcast (&cvp->cond);
}
