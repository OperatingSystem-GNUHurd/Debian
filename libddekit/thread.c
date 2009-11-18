#include <stdio.h>
#include <string.h>
#include <cthreads.h>
#include <time.h>
#include <error.h>

#include "ddekit/thread.h"

#define DDEKIT_THREAD_STACK_SIZE 0x2000 /* 8 KB */

static struct ddekit_slab *ddekit_stack_slab = NULL;

struct ddekit_thread {
	struct cthread thread;
};

static void setup_thread (cthread_t *t, const char *name) {
	if (name) {
		const char *cpy = NULL;

		cpy = malloc (strlen (name) + 1);
		if (cpy == NULL)
			error (0, 0, "fail to allocate memory");
		else 
			strcpy (cpy, name);

		cthread_set_name (t, name);
	}

	/*
	 * ldata isn't used by cthread. Since cthread isn't exposed to 
	 * the user of this library. It's very safe to store
	 * the condition variable in ldata.
	 */
	sleep_cond = condition_alloc ();
	condition_init (sleep_cond);
	cthread_set_ldata (t, sleep_cond);
}

ddekit_thread_t *ddekit_thread_setup_myself(const char *name) {
	ddekit_thread_t *td = ddekit_thread_myself();

	setup_thread (&td->thread, name);
	return td;
}

ddekit_thread_t *ddekit_thread_create(void (*fun)(void *), void *arg, const char *name) {
	ddekit_thread_t *td;
	condition_t sleep_cond;

	// TODO not very sure whether I should let the thread suspend
	// before initialization is completed.
	td = (ddekit_thread_t *) cthread_fork (fun, arg);
	setup_thread (&td->thread, name);
	return td;
}

ddekit_thread_t *ddekit_thread_myself(void) {
	return (ddekit_thread_t *) cthread_self ();
}

void ddekit_thread_set_data(ddekit_thread_t *thread, void *data) {
	// TODO not very sure whether I should call cthread_set_ldata
	// or cthread_set_data.
	cthread_set_data ((cthread_t) thread, data);
}

void ddekit_thread_set_my_data(void *data) {
	ddekit_thread_set_data(ddekit_thread_myself(), data);
}

void *ddekit_thread_get_data(ddekit_thread_t *thread) {
	return cthread_data ((cthread_t) thread);
}

void *ddekit_thread_get_my_data() {
	return ddekit_thread_get_data(ddekit_thread_myself());
}

void ddekit_thread_msleep(unsigned long msecs) {
	int ret;
	struct timespec rgt;

	rgt.tv_sec = (time_t) (msecs / 1000);
	rgt.tv_nsec = (msecs % 1000) * 1000 * 1000;
	ret = nanosleep (&rgt , NULL);
	if (ret < 0)
		error (0, errno, "nanosleep");
}

void ddekit_thread_usleep(unsigned long usecs) {
	int ret;
	struct timespec rgt;

	rgt.tv_sec = (time_t) (usecs / 1000 / 1000);
	rgt.tv_nsec = (usecs % (1000 * 1000)) * 1000;
	ret = nanosleep (&rgt , NULL);
	if (ret < 0)
		error (0, errno, "nanosleep");
}


void ddekit_thread_nsleep(unsigned long nsecs) {
	int ret;
	struct timespec rgt;

	rgt.tv_sec = (time_t) (nsecs / 1000 / 1000 / 1000);
	rgt.tv_nsec = nsecs % (1000 * 1000 * 1000);
	ret = nanosleep (&rgt , NULL);
	if (ret < 0)
		error (0, errno, "nanosleep");
}

void ddekit_thread_sleep(ddekit_lock_t *lock) {
	ddekit_thread_t *td;
	condition_t sleep_cond;

	td = ddekit_thread_myself();
	sleep_cond = ddekit_thread_get_data (&td->thread);

	mutex_lock (lock);
	// TODO condition_wait cannot guarantee that the thread is 
	// woke up by another thread, maybe by signals.
	// Does it matter here?
	condition_wait (sleep_cond, lock);
	mutex_unlock (lock);
}

void  ddekit_thread_wakeup(ddekit_thread_t *td) {
	ddekit_thread_t *td;
	condition_t sleep_cond;

	td = ddekit_thread_myself();
	sleep_cond = ddekit_thread_get_data (&td->thread);

	condition_signal (sleep_cond);
}

void  ddekit_thread_exit() {
	const char *name;
	condition_t sleep_cond;
	cthread_t t = cthread_self ();

	// TODO I hope I don't need a lock to protect ldata and name.

	/* I have to free the sleep condition variable
	 * before the thread exits. */
	sleep_cond = cthread_ldata (t);
	cthread_set_ldata (t, NULL);
	condition_free (sleep_cond);

	name = cthread_name (t);
	cthread_set_name (t, NULL);
	free (name);

	cthread_exit (0);
}

const char *ddekit_thread_get_name(ddekit_thread_t *thread) {
	return cthread_name ((cthread_t) thread);
}

void ddekit_thread_schedule(void)
{
	cthread_yield();
}

void ddekit_yield(void)
{
	cthread_yield();
}

void ddekit_init_threads() {
	// TODO maybe the name has already been set.
	cthread_set_name (cthread_self (), "main");
}
