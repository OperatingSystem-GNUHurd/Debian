#include <cthreads.h>

#include "ddekit/lock.h"

#define DDEKIT_DEBUG_LOCKS 0

struct ddekit_lock {
	struct mutex lock;
};

void _ddekit_lock_init(struct ddekit_lock **mtx) {
	*mtx = (struct ddekit_lock *) mutex_alloc ();
	mutex_init (*mtx);
}

void _ddekit_lock_deinit(struct ddekit_lock **mtx) {
	mutex_free (*mtx);
	*mtx = NULL;
}

void _ddekit_lock_lock(struct ddekit_lock **mtx) {
	mutex_lock (&(*mtx)->lock);
}

/* returns 0 on success, != 0 if it would block */
int _ddekit_lock_try_lock(struct ddekit_lock **mtx) {
	return !mutex_try_lock (&(*mtx)->lock);
}

void _ddekit_lock_unlock(struct ddekit_lock **mtx) {
	mutex_unlock (&(*mtx)->lock);
}


int _ddekit_lock_owner(struct ddekit_lock **mtx) {
	return (int)l4lock_owner(&(*mtx)->lock);
}

