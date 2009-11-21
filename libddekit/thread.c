#include <stdio.h>
#include <string.h>
#include <cthreads.h>
#include <time.h>
#include <error.h>

#include "ddekit/semaphore.h"
#include "list.h"
#include "ddekit/thread.h"

#define DDEKIT_THREAD_STACK_SIZE 0x2000 /* 8 KB */

static struct ddekit_slab *ddekit_stack_slab = NULL;

struct _ddekit_private_data {
	struct list list;
	condition_t sleep_cond;
	/* point to the thread who has the private data. */
	struct ddekit_thread *thread;
	mach_msg_header_t wakeupmsg;
	
}

struct ddekit_thread {
	struct cthread thread;
};

struct ddekit_sem
{
	spin_lock_t lock;
	/* A list of thread waiting for the semaphore. */
	struct list head;
	int value;
};

/* Prepare a wakeup message.  */
static error_t _create_wakeupmsg (struct _ddekit_private_data *data)
{
	kern_return_t err;

	/* Build wakeup message.  */
	data->wakeupmsg.msgh_bits = MACH_MSGH_BITS (MACH_MSG_TYPE_COPY_SEND, 0);
	data->wakeupmsg.msgh_size = 0;

	err = mach_port_allocate (mach_task_self (), MACH_PORT_RIGHT_RECEIVE,
				  &data->wakeupmsg.msgh_remote_port);
	if (err)
	  return EAGAIN;

	data->wakeupmsg.msgh_local_port = MACH_PORT_NULL;
	data->wakeupmsg.msgh_seqno = 0;
	data->wakeupmsg.msgh_id = 0;

	err = mach_port_insert_right (mach_task_self (),
				      data->wakeupmsg.msgh_remote_port,
				      data->wakeupmsg.msgh_remote_port,
				      MACH_MSG_TYPE_MAKE_SEND);
	if (err) {
		mach_port_destroy (mach_task_self (),
				   data->wakeupmsg.msgh_remote_port);
		return EAGAIN;
	}

	return 0;
}

static void setup_thread (cthread_t *t, const char *name) {
	error_t err;
	struct _ddekit_private_data *private_data;

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

	private_data = (struct _ddekit_private_data *)
	  ddekit_simple_malloc (sizeof (*private_data));

	private_data->sleep_cond = condition_alloc ();
	condition_init (private_data->sleep_cond);

	private_data->list = {&private_data->list, &private_data->list};
	private_data->thread = t;

	err = _create_wakeupmsg (private_data);
	// TODO I need to change this.
	assert_perror (err);

	cthread_set_ldata (t, private_data);
}

ddekit_thread_t *ddekit_thread_setup_myself(const char *name) {
	ddekit_thread_t *td = ddekit_thread_myself();

	setup_thread (&td->thread, name);
	return td;
}

ddekit_thread_t *ddekit_thread_create(void (*fun)(void *), void *arg, const char *name) {
	ddekit_thread_t *td;

	// TODO I should let the thread suspend
	// before initialization is completed.
	td = (ddekit_thread_t *) cthread_fork (fun, arg);
	cthread_detach (&td->thread);
	setup_thread (&td->thread, name);
	return td;
}

ddekit_thread_t *ddekit_thread_myself(void) {
	return (ddekit_thread_t *) cthread_self ();
}

void ddekit_thread_set_data(ddekit_thread_t *thread, void *data) {
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
	struct _ddekit_private_data *data = cthread_ldata (cthread_self ());

	// TODO condition_wait cannot guarantee that the thread is 
	// woke up by another thread, maybe by signals.
	// Does it matter here?
	condition_wait (data->sleep_cond, lock);
}

void  dekit_thread_wakeup(ddekit_thread_t *td) {
	struct _ddekit_private_data *data = cthread_ldata (cthread_self ());

	condition_signal (data->sleep_cond);
}

void  ddekit_thread_exit() {
	const char *name;
	struct _ddekit_private_data *data;
	cthread_t t = cthread_self ();

	// TODO I hope I don't need a lock to protect ldata and name.

	/* I have to free the sleep condition variable
	 * before the thread exits. */
	data = cthread_ldata (t);
	cthread_set_ldata (t, NULL);
	condition_free (data->sleep_cond);
	ddekit_simple_free (data);

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

/* Block THREAD.  */
static error_t _timedblock (struct _ddekit_private_data *data,
			    const struct timespec *abstime)
{
	error_t err;
	mach_msg_header_t msg;
	mach_msg_timeout_t timeout;
	struct timeval now;

	/* We have an absolute time and now we have to convert it to a
	   relative time.  Arg.  */

	err = gettimeofday(&now, NULL);
	assert (! err);

	if (now.tv_sec > abstime->tv_sec
	    || (now.tv_sec == abstime->tv_sec
		&& now.tv_usec > ((abstime->tv_nsec + 999) / 1000)))
		return ETIMEDOUT;

	timeout = (abstime->tv_sec - now.tv_sec) * 1000;

	if (((abstime->tv_nsec + 999) / 1000) >= now.tv_usec)
		timeout -= (((abstime->tv_nsec + 999) / 1000)
			    - now.tv_usec + 999) / 1000;
	else
		/* Need to do a carry.  */
		timeout -= 1000 + ((abstime->tv_nsec + 999999) / 1000000)
		  - (now.tv_usec + 999) / 1000;

	err = mach_msg (&msg, MACH_RCV_MSG | MACH_RCV_TIMEOUT, 0,
			sizeof msg, data->wakeupmsg.msgh_remote_port,
			timeout, MACH_PORT_NULL);
	if (err == EMACH_RCV_TIMED_OUT)
		return ETIMEDOUT;

	assert_perror (err);
	return 0;
}

/* Block THREAD.  */
static void _block (struct _ddekit_private_data *data)
{
	mach_msg_header_t msg;
	error_t err;

	err = mach_msg (&msg, MACH_RCV_MSG, 0, sizeof msg,
			data->wakeupmsg.msgh_remote_port,
			MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	assert_perror (err);
}

static int _sem_timedwait_internal (sem_t *restrict sem,
				    const struct timespec *restrict timeout)
{
	struct ddekit_private_data *self_private_data;

	spin_lock (&sem->lock);
	if (sem->value > 0) {
	        /* Successful down.  */
		sem->value --;
		spin_unlock (&sem->__lock);
		return 0;
	}

	if (timeout && (timeout->tv_nsec < 0
			|| timeout->tv_nsec >= 1000000000)) {
		errno = EINVAL;
		return -1;
	}

	/* Add ourselves to the queue.  */
	self_private_data = cthread_ldata (cthread_self ());

	add_entry_head (&sem->head, (struct list *) self_private_data);
	spin_unlock (&sem->lock);

	/* Block the thread.  */
	if (timeout) {
		error_t err;

		err = _timedblock (self_private_data, timeout);
		if (err) {
		  /* We timed out.  We may need to disconnect ourself from the
		     waiter queue.

		     FIXME: What do we do if we get a wakeup message before we
		     disconnect ourself?  It may remain until the next time we
		     block.  */
			assert (err == ETIMEDOUT);

			spin_lock (&sem->lock);
			remove_entry ((struct list *) self_private_data);
			spin_unlock (&sem->lock);

			errno = err;
			return -1;
		}
	}
	else
		_block (self_private_data);

	return 0;
}

/* Wakeup THREAD.  */
static void _thread_wakeup (struct _ddekit_private_data *data)
{
	error_t err;

	err = mach_msg (&data->wakeupmsg, MACH_SEND_MSG,
			sizeof (data->wakeupmsg), 0, MACH_PORT_NULL,
			MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	assert_perror (err);
}

ddekit_sem_t *ddekit_sem_init(int value) {
	ddekit_sem_t *sem =
	  (ddekit_sem_t *) ddekit_simple_malloc (sizeof (*sem));

	sem->lock = SPIN_LOCK_INITIALIZER;
	sem->head = {&sem->head, &sem->head};
	sem->value = value;
	return sem;
}

void ddekit_sem_deinit(ddekit_sem_t *sem) {
	if (!EMPTY_ENTRY (&sem->head)) {
		error (0, EBUSY, "ddekit_sem_deinit");
	}
	else
		ddekit_simple_free(sem);
}

void ddekit_sem_down(ddekit_sem_t *sem) {
	_sem_timedwait_internal (sem, NULL);
}

/* returns 0 on success, != 0 when it would block */
int  ddekit_sem_down_try(ddekit_sem_t *sem) {
	spin_lock (&sem->lock);
	if (sem->value > 0) {
		/* Successful down.  */
		sem->value --;
		spin_unlock (&sem->lock);
		return 0;
	}
	spin_unlock (&sem->lock);

	return -1;
}

/* returns 0 on success, != 0 on timeout */
int  ddekit_sem_down_timed(ddekit_sem_t *sem, int timo) {
	/* wait for up to timo milliseconds */
	struct timespec timeout;

	timeout.tv_sec = timo / 1000;
	timeout.tv_nsec = (timo % 1000) * 1000 * 1000;
	return __sem_timedwait_internal (sem, &timeout);
}

void ddekit_sem_up(ddekit_sem_t *sem) {
	struct _ddekit_thread_data *wakeup;

	spin_lock (&sem->lock);
	if (sem->value > 0) {
		/* Do a quick up.  */
		assert (EMPTY_LIST (&sem->head));
		sem->value ++;
		spin_unlock (&sem->lock);
		return 0;
	}

	if (EMPTY_LIST (&sem->head)) {
		/* No one waiting.  */
		sem->value = 1;
		spin_unlock (&sem->lock);
		return 0;
	}

	/* Wake someone up.  */

	/* First dequeue someone.  */
	wakeup = (struct _ddekit_private_data *) remove_entry_end (&sem->head);

	/* Then drop the lock and transfer control.  */
	spin_unlock (&sem->lock);
	if (wakeup) 
		_thread_wakeup (wakeup);

	return 0;
}

