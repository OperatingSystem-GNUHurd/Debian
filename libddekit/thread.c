#include <stdio.h>
#include <string.h>
#include <cthreads.h>
#include <time.h>
#include <error.h>
#include <hurd.h>
#include <sys/time.h>
#include <assert.h>

#include "ddekit/memory.h"
#include "ddekit/semaphore.h"
#include "list.h"
#include "ddekit/thread.h"

#define DDEKIT_THREAD_STACK_SIZE 0x2000 /* 8 KB */

//static struct ddekit_slab *ddekit_stack_slab = NULL;

struct _ddekit_private_data {
	struct list list;
	condition_t sleep_cond;
	/* point to the thread who has the private data. */
	struct ddekit_thread *thread;
	mach_msg_header_t wakeupmsg;
};

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

static struct mutex global_lock = MUTEX_INITIALIZER;

static void _thread_cleanup ()
{
	const char *name;
	struct _ddekit_private_data *data;
	cthread_t t = cthread_self ();

	/* I have to free the sleep condition variable
	 * before the thread exits. */
	mutex_lock (&global_lock);
	data = cthread_ldata (t);
	cthread_set_ldata (t, NULL);
	mutex_unlock (&global_lock);
	mach_port_destroy (mach_task_self (),
			   data->wakeupmsg.msgh_remote_port);
	condition_free (data->sleep_cond);
	ddekit_simple_free (data);

	name = cthread_name (t);
	cthread_set_name (t, NULL);
	ddekit_simple_free ((char *) name);
}

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

static void setup_thread (struct ddekit_thread *t, const char *name) {
	error_t err;
	struct _ddekit_private_data *private_data;

	if (name) {
		char *cpy = NULL;

		cpy = ddekit_simple_malloc (strlen (name) + 1);
		if (cpy == NULL)
			error (0, 0, "fail to allocate memory");
		else 
			strcpy (cpy, name);

		cthread_set_name (&t->thread, name);
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

	private_data->list.prev = &private_data->list;
	private_data->list.next = &private_data->list;
	private_data->thread = t;

	err = _create_wakeupmsg (private_data);
	if (err)
	  error (1, err, "_create_wakeupmsg");

	mutex_lock (&global_lock);
	cthread_set_ldata (&t->thread, private_data);
	mutex_unlock (&global_lock);
}

ddekit_thread_t *ddekit_thread_setup_myself(const char *name) {
	ddekit_thread_t *td = ddekit_thread_myself();

	setup_thread (td, name);
	return td;
}

typedef struct
{
  void (*fun)(void *);
  void *arg;
  struct condition cond;
  struct mutex lock;
  int status;
} priv_arg_t;

static void* _priv_fun (void *arg)
{
  priv_arg_t *priv_arg = arg;
  /* We wait until the initialization of the thread is finished. */
  mutex_lock (&priv_arg->lock);
  while (!priv_arg->status)
    condition_wait (&priv_arg->cond, &priv_arg->lock);
  mutex_unlock (&priv_arg->lock);

  priv_arg->fun(priv_arg->arg);
  _thread_cleanup ();
  return NULL;
}

ddekit_thread_t *ddekit_thread_create(void (*fun)(void *), void *arg, const char *name) {
	ddekit_thread_t *td;
	priv_arg_t *priv_arg = (priv_arg_t *) malloc (sizeof (*priv_arg));

	priv_arg->fun = fun;
	priv_arg->arg = arg;
	condition_init (&priv_arg->cond);
	mutex_init (&priv_arg->lock);
	priv_arg->status = 0;

	td = (ddekit_thread_t *) cthread_fork (_priv_fun, priv_arg);
	cthread_detach (&td->thread);
	setup_thread (td, name);

	/* Tell the new thread that initialization has been finished. */
	mutex_lock (&priv_arg->lock);
	priv_arg->status = 1;
	cond_signal (&priv_arg->cond);
	mutex_unlock (&priv_arg->lock);

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
	struct _ddekit_private_data *data;
	
	mutex_lock (&global_lock);
	data= cthread_ldata (cthread_self ());
	mutex_unlock (&global_lock);

	// TODO condition_wait cannot guarantee that the thread is 
	// woke up by another thread, maybe by signals.
	// Does it matter here?
	condition_wait (data->sleep_cond, (struct mutex *) *lock);
}

void  ddekit_thread_wakeup(ddekit_thread_t *td) {
	struct _ddekit_private_data *data;
	
	mutex_lock (&global_lock);
	data = cthread_ldata (&td->thread);
	mutex_unlock (&global_lock);

	if (data == NULL)
		return;
	condition_signal (data->sleep_cond);
}

void  ddekit_thread_exit() {
	_thread_cleanup ();
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
	ddekit_thread_setup_myself ("main");
}

/**********************************************************************
 *			    semaphore
 **********************************************************************/

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

static int _sem_timedwait_internal (ddekit_sem_t *restrict sem,
				    const struct timespec *restrict timeout)
{
	struct _ddekit_private_data *self_private_data;

	spin_lock (&sem->lock);
	if (sem->value > 0) {
	        /* Successful down.  */
		sem->value --;
		spin_unlock (&sem->lock);
		return 0;
	}

	if (timeout && (timeout->tv_nsec < 0
			|| timeout->tv_nsec >= 1000000000)) {
		errno = EINVAL;
		return -1;
	}

	/* Add ourselves to the queue.  */
	mutex_lock (&global_lock);
	self_private_data = cthread_ldata (cthread_self ());
	mutex_unlock (&global_lock);

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
	sem->head.prev = &sem->head;
	sem->head.next = &sem->head;
	sem->value = value;
	return sem;
}

void ddekit_sem_deinit(ddekit_sem_t *sem) {
	if (!EMPTY_LIST (&sem->head)) {
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
	return _sem_timedwait_internal (sem, &timeout);
}

void ddekit_sem_up(ddekit_sem_t *sem) {
	struct _ddekit_private_data *wakeup;

	spin_lock (&sem->lock);
	if (sem->value > 0) {
		/* Do a quick up.  */
		assert (EMPTY_LIST (&sem->head));
		sem->value ++;
		spin_unlock (&sem->lock);
		return;
	}

	if (EMPTY_LIST (&sem->head)) {
		/* No one waiting.  */
		sem->value = 1;
		spin_unlock (&sem->lock);
		return;
	}

	/* Wake someone up.  */

	/* First dequeue someone.  */
	wakeup = LIST_ENTRY (remove_entry_end (&sem->head),
			     struct _ddekit_private_data, list);

	/* Then drop the lock and transfer control.  */
	spin_unlock (&sem->lock);
	if (wakeup) 
		_thread_wakeup (wakeup);
}

