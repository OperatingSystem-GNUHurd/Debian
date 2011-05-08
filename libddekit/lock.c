#include <cthreads.h>

#include "ddekit/lock.h"
#include "ddekit/memory.h"

#define DDEKIT_DEBUG_LOCKS 0

struct ddekit_lock {
	struct mutex lock;
	cthread_t helder;
};

void _ddekit_lock_init(struct ddekit_lock **mtx) {
	struct ddekit_lock *lock;

	lock = (struct ddekit_lock *) ddekit_simple_malloc (sizeof *lock);
	mutex_init (&lock->lock);
	lock->helder = NULL;
	*mtx = lock;
}

void _ddekit_lock_deinit(struct ddekit_lock **mtx) {
	mutex_free (*mtx);
	*mtx = NULL;
}

void _ddekit_lock_lock(struct ddekit_lock **mtx) {
	mutex_lock (&(*mtx)->lock);
	(*mtx)->helder = cthread_self ();
}

/* returns 0 on success, != 0 if it would block */
int _ddekit_lock_try_lock(struct ddekit_lock **mtx) {
	if (mutex_try_lock (&(*mtx)->lock)) { /* lock succeessfully */
		(*mtx)->helder = cthread_self ();
		return 0;
	}
	return -1;
}

void _ddekit_lock_unlock(struct ddekit_lock **mtx) {
	// TODO I wonder if it can cause any trouble.
	(*mtx)->helder = NULL;
	mutex_unlock (&(*mtx)->lock);
}


int _ddekit_lock_owner(struct ddekit_lock **mtx) {
	/* The return value is the address of the holder.
	 * I hope it will be OK. At least, it is OK
	 * for the current implementation of DDE Linux/BSD */
	return (int) (*mtx)->helder;
}

