/*
 * \brief   Hardware-interrupt subsystem
 * \author  Thomas Friebel <tf13@os.inf.tu-dresden.de>
 * \author  Christian Helmuth <ch12@os.inf.tu-dresden.de>
 * \date    2007-01-22
 *
 * FIXME could intloop_param freed after startup?
 * FIXME use consume flag to indicate IRQ was handled
 */

#include <stdio.h>
#include <error.h>
#include <mach.h>
#include <hurd.h>

#include "ddekit/interrupt.h"
#include "ddekit/semaphore.h"
#include "ddekit/printf.h"
#include "ddekit/memory.h"

#include "device_U.h"

#define DEBUG_INTERRUPTS  0

#define MAX_INTERRUPTS   32

#define BLOCK_IRQ         0

#define MACH_NOTIFY_IRQ 100

typedef struct
{
  mach_msg_header_t irq_header;
  mach_msg_type_t   irq_type;
  int		    irq;
} mach_irq_notification_t;

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
	int              handle_irq; /* nested irq disable count */
	ddekit_lock_t    irqlock;
	ddekit_thread_t  *irq_thread; /* thread ID for detaching from IRQ later on */
	boolean_t        thread_exit;
	thread_t         mach_thread;
} ddekit_irq_ctrl[MAX_INTERRUPTS];

static mach_port_t master_device;
static mach_port_t master_host;

/**
 * Interrupt service loop
 *
 */
static void intloop(void *arg)
{
	kern_return_t ret;
	struct intloop_params *params = arg;
	mach_port_t delivery_port;
	mach_port_t pset, psetcntl;
	int my_index;

	ret = mach_port_allocate (mach_task_self (), MACH_PORT_RIGHT_RECEIVE,
				  &delivery_port);
	if (ret)
	  error (2, ret, "mach_port_allocate");

	my_index = params->irq;
	ddekit_irq_ctrl[my_index].mach_thread = mach_thread_self ();
	ret = thread_get_assignment (mach_thread_self (), &pset);
	if (ret)
		error (0, ret, "thread_get_assignment");
	ret = host_processor_set_priv (master_host, pset, &psetcntl);
	if (ret)
		error (0, ret, "host_processor_set_priv");
	thread_max_priority (mach_thread_self (), psetcntl, 0);
	ret = thread_priority (mach_thread_self (), DDEKIT_IRQ_PRIO, 0);
	if (ret)
		error (0, ret, "thread_priority");

	// TODO the flags for shared irq should be indicated by params->shared.
	// Be careful. For now, we must use shared irq.
	// Otherwise, the interrupt handler cannot be installed in the kernel.
	ret = device_intr_notify (master_device, params->irq,
				  0, 0x04000000, delivery_port,
				  MACH_MSG_TYPE_MAKE_SEND);
	ddekit_printf ("device_intr_notify returns %d\n", ret);
	if (ret) {
		/* inform thread creator of error */
		/* XXX does omega0 error code have any meaning to DDEKit users? */
		params->start_err = ret;
		ddekit_sem_up(params->started);
		ddekit_printf ("cannot install irq %d\n", params->irq);
		return;
	}
	device_irq_enable (master_device, params->irq, TRUE);

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

		((mig_reply_header_t *) outp)->RetCode = MIG_NO_REPLY;
		if (inp->msgh_id != MACH_NOTIFY_IRQ)
			return 0;

		/* It's an interrupt not for us. It shouldn't happen. */
		if (irq_header->irq != params->irq) {
			ddekit_printf ("We get interrupt %d, %d is expected",
				       irq_header->irq, params->irq);
			return 1;
		}

		/* only call registered handler function, if IRQ is not disabled */
		ddekit_lock_lock (&ddekit_irq_ctrl[my_index].irqlock);
		if (ddekit_irq_ctrl[my_index].handle_irq > 0) {
			params->handler(params->priv);
			/* If the irq has been disabled by the linux device,
			 * we don't need to reenable the real one. */
			device_irq_enable (master_device, my_index, TRUE);
		}
		else
			ddekit_printf ("not handling IRQ %x, because it is disabled.",
				       my_index);

		if (ddekit_irq_ctrl[my_index].thread_exit) {
			ddekit_lock_unlock (&ddekit_irq_ctrl[my_index].irqlock);
			ddekit_thread_exit();
			return 1;
		}
		ddekit_lock_unlock (&ddekit_irq_ctrl[my_index].irqlock);
		return 1;
	}

	mach_msg_server (irq_server, 0, delivery_port);
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

void interrupt_init ()
{
	
	error_t err;

	err = get_privileged_ports (&master_host, &master_device);
	if (err)
		error (1, err, "get_privileged_ports");
}
