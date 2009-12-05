/*
 * \brief   Hardware-interrupt subsystem
 * \author  Thomas Friebel <tf13@os.inf.tu-dresden.de>
 * \author  Christian Helmuth <ch12@os.inf.tu-dresden.de>
 * \date    2007-01-22
 *
 * FIXME could intloop_param freed after startup?
 * FIXME use consume flag to indicate IRQ was handled
 */

#include "ddekit/interrupt.h"

#include <stdio.h>

#define DEBUG_INTERRUPTS  0

#define MAX_INTERRUPTS   32

#define BLOCK_IRQ         0

/*
 * Internal type for interrupt loop parameters
 */
struct intloop_params
{
	unsigned          irq;       /* irq number */
	int               shared;    /* irq sharing supported? */
	void(*thread_init)(void *);  /* thread initialization */
	void(*handler)(void *);      /* IRQ handler function */
	void             *priv;      /* private token */ 
	ddekit_sem_t     *started;

	int               start_err;
};

static struct
{
	int               handle_irq; /* nested irq disable count */
	ddekit_lock_t    *irqlock;
	ddekit_thread_t  *irq_thread; /* thread ID for detaching from IRQ later on */
	boolean_t        thread_exit;
	thread_t         mach_thread;
} ddekit_irq_ctrl[MAX_INTERRUPTS];


static void ddekit_irq_exit_fn(l4thread_t thread, void *data)
{
	// TODO we can remove the port for delivery of interrupt explicitly,
	// though it cannot cause any harm even if we don't remove it.
}

/**
 * Interrupt service loop
 *
 */
static void intloop(void *arg)
{
	kern_return_t ret;
	struct intloop_params *params = arg;
	mach_port_t      delivery_port;
	int my_index;

	my_index = params->irq;
	ddekit_irq_ctrl[my_index]->mach_thread = mach_thread_self ();
	ret = thread_priority (mach_thread_self (), DDEKIT_IRQ_PRIO, 0);
	if (ret)
		error (0, ret, "thread_priority");

	// TODO I should give another parameter to show whether
	// the interrupt can be shared.
	ret = device_intr_notify (master_device, dev->irq,
				  dev->dev_id, delivery_port,
				  MACH_MSG_TYPE_MAKE_SEND);
	if (!ret) {
		/* inform thread creator of error */
		/* XXX does omega0 error code have any meaning to DDEKit users? */
		params->start_err = ret;
		ddekit_sem_up(params->started);
		return NULL;
	}

#if 0
	/* 
	 * Setup an exit fn. This will make sure that we clean up everything,
	 * before shutting down an IRQ thread.
	 */
	if (l4thread_on_exit(&exit_fn, (void *)my_index) < 0)
		ddekit_panic("Could not set exit handler for IRQ fn.");
#endif

	/* after successful initialization call thread_init() before doing anything
	 * else here */
	if (params->thread_init) params->thread_init(params->priv);

	/* save handle + inform thread creator of success */
	params->start_err = 0;
	ddekit_sem_up(params->started);

	int irq_server (mach_msg_header_t *inp, mach_msg_header_t *outp) {
		mach_irq_notification_t *irq_header = (mach_irq_notification_t *) inp;

		if (inp->msgh_id != MACH_NOTIFY_IRQ)
			return 0;

		/* It's an interrupt not for us. It shouldn't happen. */
		if (irq_header->irq != params->irq) {
			LOG("We get interrupt %d, %d is expected",
			    irq_header->irq, params->irq);
			return 1;
		}

		/* only call registered handler function, if IRQ is not disabled */
		ddekit_lock_lock (&ddekit_irq_ctrl[my_index].irqlock);
		if (ddekit_irq_ctrl[my_index].handle_irq > 0) {
			LOGd(DEBUG_INTERRUPTS, "IRQ %x, handler %p", my_index,params->handler);
			params->handler(params->priv);
		}
		else
			LOGd(DEBUG_INTERRUPTS, "not handling IRQ %x, because it is disabled.", my_index);

		//  ((mig_reply_header_t *) outp)->RetCode = MIG_NO_REPLY;

		if (ddekit_irq_ctrl[irq].thread_exit) {
			ddekit_lock_unlock (&ddekit_irq_ctrl[my_index].irqlock);
			ddekit_thread_exit();
			return 1;
		}
		ddekit_lock_unlock (&ddekit_irq_ctrl[my_index].irqlock);
		return 1;
	}

	mach_msg_server (int_server, 0, delivery_port);
}


/**
 * Attach to hardware interrupt
 *
 * \param irq          IRQ number to attach to
 * \param shared       set to 1 if interrupt sharing is supported; set to 0
 *                     otherwise
 * \param thread_init  called just after DDEKit internal init and before any
 *                     other function
 * \param handler      IRQ handler for interrupt irq
 * \param priv         private token (argument for thread_init and handler)
 *
 * \return pointer to interrupt thread created
 */
ddekit_thread_t *ddekit_interrupt_attach(int irq, int shared,
                                         void(*thread_init)(void *),
                                         void(*handler)(void *), void *priv)
{
	struct intloop_params *params;
	ddekit_thread_t *thread;
	char thread_name[10];

	/* We cannot attach the interrupt to the irq which has been used. */
	if (ddekit_irq_ctrl[irq].irq_thread)
	  return NULL;

	/* initialize info structure for interrupt loop */
	params = ddekit_simple_malloc(sizeof(*params));
	if (!params) return NULL;

	// TODO make sure irq is 0-15 instead of 1-16.
	params->irq         = irq;
	params->thread_init = thread_init;
	params->handler     = handler;
	params->priv        = priv;
	params->started     = ddekit_sem_init(0);
	params->start_err   = 0;
	params->shared      = shared;

	/* construct name */
	snprintf(thread_name, 10, "irq%02X", irq);

	/* allocate irq */
	/* create interrupt loop thread */
	thread = ddekit_thread_create(intloop, params, thread_name);
	if (!thread) {
		ddekit_simple_free(params);
		return NULL;
	}

	ddekit_irq_ctrl[irq].handle_irq = 1; /* IRQ nesting level is initially 1 */
	ddekit_irq_ctrl[irq].irq_thread = thread;
	ddekit_lock_init_unlocked (&ddekit_irq_ctrl[irq].irqlock);
	ddekit_irq_ctrl[irq].thread_exit = FALSE;


	/* wait for intloop initialization result */
	ddekit_sem_down(params->started);
	ddekit_sem_deinit(params->started);
	if (params->start_err) {
		ddekit_simple_free(params);
		return NULL;
	}

	return thread;
}

/**
 * Detach from interrupt by disabling it and then shutting down the IRQ
 * thread.
 */
void ddekit_interrupt_detach(int irq)
{
	ddekit_interrupt_disable(irq);
	ddekit_lock_lock (&ddekit_irq_ctrl[irq].irqlock);
	if (ddekit_irq_ctrl[irq].handle_irq == 0) {
		ddekit_irq_ctrl[irq].thread_exit = TRUE;
		ddekit_irq_ctrl[irq].irq_thread = NULL;

		/* If the irq thread is waiting for interrupt notification
		 * messages, thread_abort() can force it to return.
		 * I hope this ugly trick can work. */
		thread_abort (ddekit_irq_ctrl[irq].mach_thread);
	}
	ddekit_lock_unlock (&ddekit_irq_ctrl[irq].irqlock);
}


void ddekit_interrupt_disable(int irq)
{
	if (ddekit_irq_ctrl[irq].irqlock) {
		ddekit_lock_lock (&ddekit_irq_ctrl[irq].irqlock);
		--ddekit_irq_ctrl[irq].handle_irq;
		ddekit_lock_unlock (&ddekit_irq_ctrl[irq].irqlock);
	}
}


void ddekit_interrupt_enable(int irq)
{
	if (ddekit_irq_ctrl[irq].irqlock) {
		ddekit_lock_lock (&ddekit_irq_ctrl[irq].irqlock);
		++ddekit_irq_ctrl[irq].handle_irq;
		ddekit_lock_unlock (&ddekit_irq_ctrl[irq].irqlock);
	}
}
