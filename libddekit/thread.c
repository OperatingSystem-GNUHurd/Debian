#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <error.h>
#include <mach.h>
#include <hurd.h>
#include <sys/time.h>
#include <assert.h>

#include "ddekit/memory.h"
#include "ddekit/semaphore.h"
#include "ddekit/condvar.h"
#include "list.h"
#include "ddekit/thread.h"

#define DDEKIT_THREAD_STACK_SIZE 0x2000 /* 8 KB */

//static struct ddekit_slab *ddekit_stack_slab = NULL;

struct _ddekit_private_data {
	struct list list;
	ddekit_condvar_t *sleep_cond;
	/* point to the thread who has the private data. */
	struct ddekit_thread *thread;
	mach_msg_header_t wakeupmsg;
};

struct ddekit_thread {
	pthread_t thread;
	char *name;
	struct _ddekit_private_data *private;
	void *user;
};

struct ddekit_sem
{
	pthread_spinlock_t lock;
	/* A list of thread waiting for the semaphore. */
	struct list head;
	int value;
};

static __thread struct ddekit_thread *thread_self;

static void _thread_cleanup ()
{
	mach_port_destroy (mach_task_self (),
			   thread_self->private->wakeupmsg.msgh_remote_port);
	ddekit_condvar_deinit (thread_self->private->sleep_cond);
	ddekit_simple_free (thread_self->private);
	ddekit_simple_free (thread_self->name);
	ddekit_simple_free (thread_self);
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

		t->name = cpy;
	}

	private_data = (struct _ddekit_private_data *)
	  ddekit_simple_malloc (sizeof (*private_data));

	private_data->sleep_cond = ddekit_condvar_init ();
	private_data->list.prev = &private_data->list;
	private_data->list.next = &private_data->list;
	private_data->thread = t;

	err = _create_wakeupmsg (private_data);
	if (err)
	  error (1, err, "_create_wakeupmsg");

	t->private = private_data;
}

ddekit_thread_t *ddekit_thread_setup_myself(const char *name) {
	ddekit_thread_t *td = (ddekit_thread_t *) malloc (sizeof (*td));
	setup_thread (td, name);
	thread_self = td;
	return td;
}

typedef struct
{
  void (*fun)(void *);
  void *arg;
  struct ddekit_thread *td;
  pthread_cond_t cond;
  pthread_mutex_t lock;
  int status;
} priv_arg_t;

static void* _priv_fun (void *arg)
{
  priv_arg_t *priv_arg = arg;
  thread_self = priv_arg->td;
  /* We wait until the initialization of the thread is finished. */
  pthread_mutex_lock (&priv_arg->lock);
  while (!priv_arg->status)
    pthread_cond_wait (&priv_arg->cond, &priv_arg->lock);
  pthread_mutex_unlock (&priv_arg->lock);

  priv_arg->fun(priv_arg->arg);
  free (priv_arg->arg);
  _thread_cleanup ();
  return NULL;
}

ddekit_thread_t *ddekit_thread_create(void (*fun)(void *), void *arg, const char *name) {
	ddekit_thread_t *td = (ddekit_thread_t *) malloc (sizeof (*td));
	setup_thread (td, name);

	priv_arg_t *priv_arg = (priv_arg_t *) malloc (sizeof (*priv_arg));
	priv_arg->fun = fun;
	priv_arg->arg = arg;
	priv_arg->td = td;
	pthread_cond_init (&priv_arg->cond, NULL);
	pthread_mutex_init (&priv_arg->lock, NULL);
	priv_arg->status = 0;

	pthread_create (&td->thread, NULL, _priv_fun, priv_arg);
	pthread_detach (td->thread);

	/* Tell the new thread that initialization has been finished. */
	pthread_mutex_lock (&priv_arg->lock);
	priv_arg->status = 1;
	pthread_cond_signal (&priv_arg->cond);
	pthread_mutex_unlock (&priv_arg->lock);

	return td;
}

ddekit_thread_t *ddekit_thread_myself(void) {
	return thread_self;
}

void ddekit_thread_set_data(ddekit_thread_t *thread, void *data) {
	thread->user = data;
}

void ddekit_thread_set_my_data(void *data) {
	ddekit_thread_set_data(ddekit_thread_myself(), data);
}

void *ddekit_thread_get_data(ddekit_thread_t *thread) {
	return thread->user;
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
	// TODO pthread_cond_wait cannot guarantee that the thread is 
	// woke up by another thread, maybe by signals.
	// Does it matter here?
	// If it does, use pthread_hurd_cond_wait_np.
	ddekit_condvar_wait (thread_self->private->sleep_cond, lock);
}

void  ddekit_thread_wakeup(ddekit_thread_t *td) {
	if (td->private == NULL)
		return;
	ddekit_condvar_signal (td->private->sleep_cond);
}

void  ddekit_thread_exit() {
	_thread_cleanup ();
	pthread_exit (NULL);
}

const char *ddekit_thread_get_name(ddekit_thread_t *thread) {
	return thread->name;
}

void ddekit_thread_schedule(void)
{
	swtch_pri (0);
}

void ddekit_yield(void)
{
	swtch_pri (0);
}

void ddekit_init_threads() {
	ddekit_thread_setup_myself ("main");
}

/**********************************************************************
 *			    semaphore
 **********************************************************************/

/* Block THREAD.  */
static error_t _timedblock (struct _ddekit_private_data *data,
			    const int timeout)
{
	error_t err;
	mach_msg_header_t msg;

	assert (timeout > 0);

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
				    const int timeout)
{
	pthread_spin_lock (&sem->lock);
	if (sem->value > 0) {
	        /* Successful down.  */
		sem->value --;
		pthread_spin_unlock (&sem->lock);
		return 0;
	}

	if (timeout < 0) {
		pthread_spin_unlock (&sem->lock);
		errno = EINVAL;
		return -1;
	}

	/* Add ourselves to the queue.  */
	add_entry_head (&sem->head, &thread_self->private->list);
	pthread_spin_unlock (&sem->lock);

	/* Block the thread.  */
	if (timeout) {
		error_t err;

		err = _timedblock (thread_self->private, timeout);
		if (err) {
		  /* We timed out.  We may need to disconnect ourself from the
		     waiter queue.

		     FIXME: What do we do if we get a wakeup message before we
		     disconnect ourself?  It may remain until the next time we
		     block.  */
			assert (err == ETIMEDOUT);

			pthread_spin_lock (&sem->lock);
			remove_entry (&thread_self->private->list);
			pthread_spin_unlock (&sem->lock);

			errno = err;
			return -1;
		}
	}
	else
		_block (thread_self->private);

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

	sem->lock = PTHREAD_SPINLOCK_INITIALIZER;
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
	_sem_timedwait_internal (sem, 0);
}

/* returns 0 on success, != 0 when it would block */
int  ddekit_sem_down_try(ddekit_sem_t *sem) {
	pthread_spin_lock (&sem->lock);
	if (sem->value > 0) {
		/* Successful down.  */
		sem->value --;
		pthread_spin_unlock (&sem->lock);
		return 0;
	}
	pthread_spin_unlock (&sem->lock);

	return -1;
}

/* returns 0 on success, != 0 on timeout */
int  ddekit_sem_down_timed(ddekit_sem_t *sem, int timo) {
	/* wait for up to timo milliseconds */
	return _sem_timedwait_internal (sem, timo);
}

void ddekit_sem_up(ddekit_sem_t *sem) {
	struct _ddekit_private_data *wakeup;

	pthread_spin_lock (&sem->lock);
	if (sem->value > 0) {
		/* Do a quick up.  */
		assert (EMPTY_LIST (&sem->head));
		sem->value ++;
		pthread_spin_unlock (&sem->lock);
		return;
	}

	if (EMPTY_LIST (&sem->head)) {
		/* No one waiting.  */
		sem->value = 1;
		pthread_spin_unlock (&sem->lock);
		return;
	}

	/* Wake someone up.  */

	/* First dequeue someone.  */
	wakeup = LIST_ENTRY (remove_entry_end (&sem->head),
			     struct _ddekit_private_data, list);

	/* Then drop the lock and transfer control.  */
	pthread_spin_unlock (&sem->lock);
	if (wakeup) 
		_thread_wakeup (wakeup);
}

