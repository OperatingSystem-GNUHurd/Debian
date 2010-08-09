#include "local.h"

#include <linux/kernel.h>

/* IRQ lock reference counter */
static atomic_t      _refcnt   = ATOMIC_INIT(0);
static ddekit_lock_t cli_lock;

/* Check whether IRQs are currently disabled.
 *
 * This is the case, if flags is greater than 0.
 */

int raw_irqs_disabled_flags(unsigned long flags)
{
	return ((int)flags > 0);
}

/* If it does lock operation successfully, return > 0. Otherwise, 0. */
static int nested_lock(ddekit_lock_t lock)
{
	int do_lock = 0;

	if (ddekit_lock_try_lock(&lock)) {  /* if we cannot lock */
		/* check who hold the lock. */
		if (_ddekit_lock_owner(&lock) != (int) ddekit_thread_myself()) {
			/* Someone else holds the lock,
			 * or by the time I try to lock again,
			 * the person has release the lock. */
			ddekit_lock_lock(&lock);
			do_lock = 1;
		}
		/* If I hold the lock myself, I don't need to worry
		 * the lock will be released somewhere before I do it. */
	}
	else
		do_lock = 2;

	return do_lock;
}

unsigned long fake_local_irq_disable_flags(void)
{
	return atomic_add_return (1, &_refcnt) - 1;
}

void fake_local_irq_enable(void)
{
	atomic_set(&_refcnt, 0);
}

void fake_local_irq_restore(unsigned long flags)
{
	atomic_set(&_refcnt, flags);
}

/* Store the current flags state.
 *
 * This is done by returning the current refcnt.
 *
 * XXX: Up to now, flags was always 0 at this point and
 *      I assume that this is always the case. Prove?
 */
unsigned long __raw_local_save_flags(void)
{
	unsigned long flags;
	int do_lock = 0;

	if (cli_lock == NULL)
		ddekit_lock_init_unlocked(&cli_lock);
	/* It's important to do lock here. 
	 * Otherwise, a thread might not get correct flags. */
	do_lock = nested_lock(cli_lock);
	flags = (unsigned long)atomic_read(&_refcnt);
	if (do_lock)
		ddekit_lock_unlock(&cli_lock);
	return flags;
}

/* Restore IRQ state. */
void raw_local_irq_restore(unsigned long flags)
{
	Assert(cli_lock != NULL);
	atomic_set(&_refcnt, flags);
	if (flags == 0)
		ddekit_lock_unlock(&cli_lock);
}

/* Disable IRQs by grabbing the IRQ lock. */
void raw_local_irq_disable(void)
{
	struct ddekit_thread *helder;
	int is_print = 0;

	if (cli_lock == NULL)
		ddekit_lock_init_unlocked(&cli_lock);

	nested_lock(cli_lock);
	atomic_inc(&_refcnt);
}

/* Unlock the IRQ lock until refcnt is 0. */
void raw_local_irq_enable(void)
{
	Assert(cli_lock != NULL);
	atomic_set(&_refcnt, 0);
	ddekit_lock_unlock(&cli_lock);
}


void raw_safe_halt(void)
{
	WARN_UNIMPL;
}


void halt(void)
{
	WARN_UNIMPL;
}

/* These functions are empty for DDE. Every DDE thread is a separate
 * "virtual" CPU. Therefore there is no need to en/disable bottom halves.
 */
void local_bh_disable(void) {}
void __local_bh_enable(void) {}
void _local_bh_enable(void) {}
void local_bh_enable(void) {}
