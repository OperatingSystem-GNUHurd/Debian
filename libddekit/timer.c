#include <error.h>
#include <maptime.h>
#include <cthreads.h>
#include <stdio.h>

#include "ddekit/lock.h"
#include "ddekit/memory.h"
#include "ddekit/assert.h"
#include "ddekit/semaphore.h"
#include "ddekit/timer.h"

#define	__DEBUG	0

volatile struct mapped_time_value *mapped_time;
long long root_jiffies;

/* Just to remind BjoernD of what this is:
 * HZ = clock ticks per second
 * jiffies = clock ticks counter.
 *
 * So, if someone schedules a timeout to expire in 2 seconds,
 * this expires date will be in jiffies + 2 * HZ.
 */

typedef struct _timer
{
	struct _timer      *next;
	void               (*fn)(void *);
	void               *args;
	unsigned long 	   expires;
	int                id;
} ddekit_timer_t;


static ddekit_timer_t  *timer_list          = NULL; ///< list of pending timers
static ddekit_lock_t   timer_lock;		    ///< lock to access timer_list
static ddekit_thread_t *timer_thread_ddekit = NULL; ///< ddekit ID of timer thread
static ddekit_sem_t    *notify_semaphore    = NULL; ///< timer thread's wait semaphore

static int timer_id_ctr = 0;

static void dump_list(char *msg)
{
#if __DEBUG
	ddekit_timer_t *l = timer_list;

	ddekit_printf("-=-=-=-= %s =-=-=-\n", msg);
	while (l) {
		ddekit_printf("-> %d (%lld)\n", l->id, l->expires);
		l = l->next;
	}
	ddekit_printf("-> NULL\n");
	ddekit_printf("-=-=-=-=-=-=-=-\n");
#endif
}

unsigned long fetch_jiffies ()
{
  struct timeval tv;
  long long j;

  if (mapped_time == NULL)
    ddekit_init_timers ();
  maptime_read (mapped_time, &tv);

  j = (long long) tv.tv_sec * HZ + ((long long) tv.tv_usec * HZ) / 1000000;
  return j - root_jiffies;
}

/** Notify the timer thread there is a new timer at the beginning of the
 *  timer list.
 */
static inline void __notify_timer_thread(void)
{
	/* Do not notify if there is no timer thread.
	 * XXX: Perhaps we should better assert that there is a timer
	 *      thread before allowing users to add a timer.
	 */
	if (timer_thread_ddekit == NULL)
		return;

	ddekit_sem_up(notify_semaphore);
}


int ddekit_add_timer(void (*fn)(void *), void *args, unsigned long timeout)
{
	ddekit_timer_t *it;
	ddekit_timer_t *t = ddekit_simple_malloc(sizeof(ddekit_timer_t));

	Assert(t);

	/* fill in values */
	t->fn      = fn;
	t->args    = args;
	t->expires = timeout;
	t->next    = NULL;

	ddekit_lock_lock (&timer_lock);

	t->id         = timer_id_ctr++;

	/* the easy case... */
	if (timer_list == NULL || timer_list->expires >= t->expires) {
		t->next    = timer_list;
		timer_list = t;
	}
	else { /* find where to insert */
		it = timer_list;
		while (it->next && it->next->expires < t->expires)
			it = it->next;

		if (it->next) { /* insert somewhere in the middle */
			t->next  = it->next;
			it->next = t;
		}
		else /* we append */
			it->next = t;
	}

	/* 
	 * if we modified the first entry of the list, it is
	 * necessary to notify the timer thread.
	 */
	if (t == timer_list) {
		Assert(timer_thread_ddekit);
		__notify_timer_thread();
	}

	ddekit_lock_unlock (&timer_lock);

	dump_list("after add");
	
	return t->id;
}


int ddekit_del_timer(int timer)
{
	ddekit_timer_t *it, *it_next;
	int ret = -1;

	ddekit_lock_lock (&timer_lock);

	/* no timer? */
	if (!timer_list) {
		ret = -2;
		goto out;
	}

	/* removee is first item, simply delete it */
	if (timer_list->id == timer) {
		it = timer_list->next;
		ret = timer_list->id;
		ddekit_simple_free(timer_list);
		timer_list = it;

		/*
		 * We do not notify the timer thread here to save IPCs. The
		 * thread will wakeup later and finally detect that there is
		 * no timer pending anymore.
		 */
		goto out;
	}

	it = timer_list;
	it_next = it->next;

	/* more difficult if removee is somewhere in
	 * the middle of the list
	 */
	while (it_next) {
		if (it_next->id == timer) {
			it->next = it->next->next;
			ret = it_next->id;
			ddekit_simple_free(it_next);
			goto out;
		}
		it = it->next;
		it_next = it->next;
	}

out:
	ddekit_lock_unlock (&timer_lock);

	dump_list("after del");
	
	return ret;
}


/** Check whether a timer with a given ID is still pending.
 *
 * \param timer	Timer ID to check for.
 * \return 0 if not pending
 *         1 if timer is pending
 */
int ddekit_timer_pending(int timer)
{
	ddekit_timer_t *t = NULL;
	int r = 0;

	ddekit_lock_lock (&timer_lock);

	t = timer_list;
	while (t) {
		if (t->id == timer) {
			r = 1;
			break;
		}
		t = t->next;
	}

	ddekit_lock_unlock (&timer_lock);

	return r;
}


/** Get the next timer function to run.
 *
 * \return NULL	    if no timer is to be run now
 * 		   != NULL  next timer to execute
 */
static ddekit_timer_t *get_next_timer(void)
{
	ddekit_timer_t *t = NULL;

	/* This function must be called with the timer_lock held. */
	Assert(ddekit_lock_owner (&timer_lock) == (int) timer_thread_ddekit);

	if (timer_list &&
	   (timer_list->expires <= jiffies)) {
		t = timer_list;
		timer_list = timer_list->next;
	}

	return t;
}

enum
{
	DDEKIT_TIMEOUT_NEVER = 0xFFFFFFFF,
};

/** Let the timer thread sleep for a while. 
 *
 * \param	to	timeout in msec
 *
 * \return	1 if IPC timed out
 * 			0 if message received -> recalc timeout
 */
static inline int __timer_sleep(unsigned to)
{
	int err = 0;
	
	ddekit_lock_unlock (&timer_lock);

	if (to == DDEKIT_TIMEOUT_NEVER) {
		ddekit_sem_down(notify_semaphore);
	}
	else {
#if 0
		ddekit_printf("Going to sleep for %lu µs (%lu ms)\n", to * 1000, to);
#endif
		err = ddekit_sem_down_timed(notify_semaphore, to);
#if 0
		ddekit_printf("err: %x\n", err);
#endif
	}

	ddekit_lock_lock (&timer_lock);

	return (err ? 1 : 0);
}


static void ddekit_timer_thread(void *arg)
{
	notify_semaphore = ddekit_sem_init(0);
#if 0
	l4thread_set_prio(l4thread_myself(), 0x11);
#endif

//	l4thread_started(0);

	ddekit_lock_lock (&timer_lock);
	while (1) {
		ddekit_timer_t *timer = NULL;
		unsigned long  to     = DDEKIT_TIMEOUT_NEVER;

		if (timer_list) {
#if 0
			int jdiff = timer_list->expires - jiffies;
			ddekit_printf("\033[31mscheduling new timeout.\033[0m\n");
			ddekit_printf("\033[31mjiffies diff = %ld (%d s)\033[0m\n", jdiff, jdiff/HZ);
#endif
			to = (timer_list->expires - jiffies) * 1000000 / HZ;
			to /= 1000;
		}

		__timer_sleep(to);

		while ((timer = get_next_timer()) != NULL) {
			ddekit_lock_unlock (&timer_lock);
			//ddekit_printf("doing timer fn @ %p\n", timer->fn);
			timer->fn(timer->args);
			ddekit_simple_free(timer);
			ddekit_lock_lock (&timer_lock);
		}
	}
	// TODO how is the thread terminated?
}

ddekit_thread_t *ddekit_get_timer_thread()
{
	return timer_thread_ddekit;
}


void ddekit_init_timers(void)
{
  error_t err;
  struct timeval tp;
  static boolean_t initialized = FALSE;

  if (initialized)
    return;

  initialized = TRUE;
  err = maptime_map (0, 0, &mapped_time);
  if (err)
    error (2, err, "cannot map time device");

  maptime_read (mapped_time, &tp);

  root_jiffies = (long long) tp.tv_sec * HZ
    + ((long long) tp.tv_usec * HZ) / 1000000;

  ddekit_lock_init (&timer_lock);
  timer_thread_ddekit = ddekit_thread_create (ddekit_timer_thread,
					      NULL, "ddekit_timer");
}
