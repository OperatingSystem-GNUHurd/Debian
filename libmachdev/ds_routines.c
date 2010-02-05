/*
 * Mach Operating System
 * Copyright (c) 1993,1991,1990,1989 Carnegie Mellon University
 * All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 * Carnegie Mellon requests users of this software to return to
 *
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 *	Author: David B. Golub, Carnegie Mellon University
 *	Date: 	3/89
 */

/*
 * Mach device server routines (i386at version).
 *
 * Copyright (c) 1996 The University of Utah and
 * the Computer Systems Laboratory at the University of Utah (CSL).
 * All rights reserved.
 *
 * Permission to use, copy, modify and distribute this software is hereby
 * granted provided that (1) source code retains these copyright, permission,
 * and disclaimer notices, and (2) redistributions including binaries
 * reproduce the notices in supporting documentation, and (3) all advertising
 * materials mentioning features or use of this software display the following
 * acknowledgement: ``This product includes software developed by the
 * Computer Systems Laboratory at the University of Utah.''
 *
 * THE UNIVERSITY OF UTAH AND CSL ALLOW FREE USE OF THIS SOFTWARE IN ITS "AS
 * IS" CONDITION.  THE UNIVERSITY OF UTAH AND CSL DISCLAIM ANY LIABILITY OF
 * ANY KIND FOR ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 * CSL requests users of this software to return to csl-dist@cs.utah.edu any
 * improvements that they make and grant CSL redistribution rights.
 *
 *      Author: Shantanu Goel, University of Utah CSL
 */

#include <stdio.h>
#include <string.h>
#include <error.h>

#include <hurd.h>
#include <mach.h>
#include <cthreads.h>

#include "vm_param.h"
#include "device_reply_U.h"
#include "io_req.h"
#include "dev_hdr.h"
#include "util.h"
#include "queue.h"

static struct port_bucket *port_bucket;
static struct port_class *dev_class;

extern struct device_emulation_ops linux_net_emulation_ops;

#define NUM_EMULATION (sizeof (emulation_list) / sizeof (emulation_list[0]))

/* List of emulations.  */
static struct device_emulation_ops *emulation_list[] =
{
  &linux_net_emulation_ops,
};

boolean_t is_master_device (mach_port_t port);

static inline void
mach_device_deallocate (void *device)
{
  ports_port_deref (device);
}

static inline void
mach_device_reference (mach_device_t device)
{
  ports_port_ref (device);
}

static inline emul_device_t
mach_convert_port_to_device (device_t device)
{
  mach_device_t dev = ports_lookup_port (port_bucket, device, dev_class);
  if (dev == NULL)
    return NULL;

  return &dev->dev;
}

static inline void *
device_to_pi (emul_device_t device)
{
  return ((void *) device) - (int) &((mach_device_t) 0)->dev;
}

/*
 * What follows is the interface for the native Mach devices.
 */

static inline mach_port_t
mach_convert_device_to_port (mach_device_t device)
{
  if (device == NULL)
    return MACH_PORT_NULL;

  // TODO I have to somehow dereference it when it is called at the first time.
  return ports_get_right (device);
}

/* Implementation of device interface */
kern_return_t 
ds_xxx_device_set_status (device_t device, dev_flavor_t flavor,
			  dev_status_t status, size_t statu_cnt)
{
  return D_INVALID_OPERATION;
}

kern_return_t
ds_xxx_device_get_status (device_t device, dev_flavor_t flavor,
			  dev_status_t status, size_t *statuscnt)
{
  return D_INVALID_OPERATION;
}

kern_return_t
ds_xxx_device_set_filter (device_t device, mach_port_t rec,
			  int pri, filter_array_t filt, size_t len)
{
  return D_INVALID_OPERATION;
}

io_return_t
ds_device_intr_notify (mach_port_t master_port, int irq,
		       int id, mach_port_t receive_port)
{
  return D_INVALID_OPERATION;
}

kern_return_t
ds_pci_write_config (mach_port_t master_port, char bus, char device_fn,
		     char where, pci_config_data_t data,
		     mach_msg_type_number_t dataCnt)
{
  return D_INVALID_OPERATION;
}

kern_return_t
ds_pci_read_config (mach_port_t master_port, char bus, char device_fn,
		    char where, int bytes_wanted, pci_config_data_t result,
		    mach_msg_type_number_t *resultCnt)
{
  return D_INVALID_OPERATION;
}

kern_return_t
ds_pci_find_device (mach_port_t master_port, short vendor, short device_id,
		    short index, short *bus, char *device_fn)
{
  return D_INVALID_OPERATION;
}

kern_return_t
ds_pci_present (mach_port_t master_port)
{
  return D_INVALID_OPERATION;
}

kern_return_t
ds_device_irq_enable (mach_port_t master_port,
		      int irq, char status)
{
  return D_INVALID_OPERATION;
}

io_return_t
ds_device_open (mach_port_t open_port, mach_port_t reply_port,
		mach_msg_type_name_t reply_port_type, dev_mode_t mode,
		char *name, device_t *devp)
{
  int i;
  io_return_t err;

  /* Open must be called on the master device port.  */
  if (!is_master_device (open_port))
    return D_INVALID_OPERATION;

  /* There must be a reply port.  */
  if (! MACH_PORT_VALID (reply_port))
    {
      fprintf (stderr, "ds_* invalid reply port\n");
      return MIG_NO_REPLY;
    }

  /* Call each emulation's open routine to find the device.  */
  for (i = 0; i < NUM_EMULATION; i++)
    {
      err = (*emulation_list[i]->open) (reply_port, reply_port_type,
					mode, name, devp);
      if (err != D_NO_SUCH_DEVICE)
	break;
    }

  return err;
}

io_return_t
ds_device_close (device_t dev)
{
  emul_device_t device;
  io_return_t ret;

  /* Refuse if device is dead or not completely open.  */
  if (dev == MACH_PORT_NULL)
    return D_NO_SUCH_DEVICE;

  device = mach_convert_port_to_device (dev);
  ret = (device->emul_ops->close
	 ? (*device->emul_ops->close) (device->emul_data)
	 : D_SUCCESS);
  mach_device_deallocate (device_to_pi (device));

  ports_port_deref (device_to_pi (device));
  return ret;
}

io_return_t
ds_device_write (device_t dev, mach_port_t reply_port,
		 mach_msg_type_name_t reply_port_type, dev_mode_t mode,
		 recnum_t recnum, io_buf_ptr_t data, unsigned int count,
		 int *bytes_written)
{
  emul_device_t device;
  io_return_t ret;

  /* Refuse if device is dead or not completely open.  */
  if (dev == MACH_PORT_NULL)
    return D_NO_SUCH_DEVICE;

  if (data == 0)
    return D_INVALID_SIZE;

  device = mach_convert_port_to_device (dev);

  if (! device->emul_ops->write)
    {
      ports_port_deref (device_to_pi (device));
      return D_INVALID_OPERATION;
    }

  ret = (*device->emul_ops->write) (device->emul_data, reply_port,
				    reply_port_type, mode, recnum,
				    data, count, bytes_written);
  ports_port_deref (device_to_pi (device));

  return ret;
}

io_return_t
ds_device_write_inband (device_t dev, mach_port_t reply_port,
			mach_msg_type_name_t reply_port_type,
			dev_mode_t mode, recnum_t recnum,
			io_buf_ptr_inband_t data, unsigned count,
			int *bytes_written)
{
  emul_device_t device;
  io_return_t ret;

  /* Refuse if device is dead or not completely open.  */
  if (dev == MACH_PORT_NULL)
    return D_NO_SUCH_DEVICE;

  if (data == 0)
    return D_INVALID_SIZE;

  device = mach_convert_port_to_device (dev);

  if (! device->emul_ops->write_inband)
    {
      ports_port_deref (device_to_pi (device));
      return D_INVALID_OPERATION;
    }

  ret = (*device->emul_ops->write_inband) (device->emul_data, reply_port,
					   reply_port_type, mode, recnum,
					   data, count, bytes_written);
  ports_port_deref (device_to_pi (device));

  return ret;
}

io_return_t
ds_device_read (device_t dev, mach_port_t reply_port,
		mach_msg_type_name_t reply_port_type, dev_mode_t mode,
		recnum_t recnum, int count, io_buf_ptr_t *data,
		unsigned *bytes_read)
{
  emul_device_t device;
  io_return_t ret;

  /* Refuse if device is dead or not completely open.  */
  if (dev == MACH_PORT_NULL)
    return D_NO_SUCH_DEVICE;

  device = mach_convert_port_to_device (dev);

  if (! device->emul_ops->read)
    {
      ports_port_deref (device_to_pi (device));
      return D_INVALID_OPERATION;
    }

  ret = (*device->emul_ops->read) (device->emul_data, reply_port,
				   reply_port_type, mode, recnum,
				   count, data, bytes_read);
  ports_port_deref (device_to_pi (device));
  return ret;
}

io_return_t
ds_device_read_inband (device_t dev, mach_port_t reply_port,
		       mach_msg_type_name_t reply_port_type, dev_mode_t mode,
		       recnum_t recnum, int count, char *data,
		       unsigned *bytes_read)
{
  emul_device_t device;
  io_return_t ret;

  /* Refuse if device is dead or not completely open.  */
  if (dev == MACH_PORT_NULL)
    return D_NO_SUCH_DEVICE;

  device = mach_convert_port_to_device (dev);

  if (! device->emul_ops->read_inband)
    {
      ports_port_deref (device_to_pi (device));
      return D_INVALID_OPERATION;
    }

  ret = (*device->emul_ops->read_inband) (device->emul_data, reply_port,
					  reply_port_type, mode, recnum,
					  count, data, bytes_read);
  ports_port_deref (device_to_pi (device));
  return ret;
}

io_return_t
ds_device_set_status (device_t dev, dev_flavor_t flavor,
		      dev_status_t status, mach_msg_type_number_t status_count)
{
  emul_device_t device;
  io_return_t ret;

  /* Refuse if device is dead or not completely open.  */
  if (dev == MACH_PORT_NULL)
    return D_NO_SUCH_DEVICE;

  device = mach_convert_port_to_device (dev);

  if (! device->emul_ops->set_status)
    {
      ports_port_deref (device_to_pi (device));
      return D_INVALID_OPERATION;
    }

  ret = (*device->emul_ops->set_status) (device->emul_data, flavor,
					 status, status_count);
  ports_port_deref (device_to_pi (device));
  return ret;
}

io_return_t
ds_device_get_status (device_t dev, dev_flavor_t flavor, dev_status_t status,
		      mach_msg_type_number_t *status_count)
{
  emul_device_t device;
  io_return_t ret;

  /* Refuse if device is dead or not completely open.  */
  if (dev == MACH_PORT_NULL)
    return D_NO_SUCH_DEVICE;

  device = mach_convert_port_to_device (dev);

  if (! device->emul_ops->get_status)
    {
      ports_port_deref (device_to_pi (device));
      return D_INVALID_OPERATION;
    }

  ret = (*device->emul_ops->get_status) (device->emul_data, flavor,
					 status, status_count);
  ports_port_deref (device_to_pi (device));
  return ret;
}

io_return_t
ds_device_set_filter (device_t dev, mach_port_t receive_port, int priority,
		      filter_t *filter, unsigned filter_count)
{
  emul_device_t device;
  io_return_t ret;

  /* Refuse if device is dead or not completely open.  */
  if (dev == MACH_PORT_NULL)
    return D_NO_SUCH_DEVICE;

  device = mach_convert_port_to_device (dev);

  if (! device->emul_ops->set_filter)
    {
      ports_port_deref (device_to_pi (device));
      return D_INVALID_OPERATION;
    }

  ret = (*device->emul_ops->set_filter) (device->emul_data, receive_port,
					 priority, filter, filter_count);
  ports_port_deref (device_to_pi (device));
  return ret;
}

io_return_t
ds_device_map (device_t dev, vm_prot_t prot, vm_offset_t offset,
	       vm_size_t size, mach_port_t *pager, boolean_t unmap)
{
  /* Refuse if device is dead or not completely open.  */
  if (dev == MACH_PORT_NULL)
    return D_NO_SUCH_DEVICE;

  return D_INVALID_OPERATION;
}

boolean_t
ds_open_done(ior)
	register io_req_t	ior;
{
	kern_return_t		result;
	register mach_device_t	device;

	device = ior->io_device;
	result = ior->io_error;

	if (result != D_SUCCESS) {
	    /*
	     * Open failed.  Deallocate port and device.
	     */
//	    dev_port_remove(device);
//	    ipc_port_dealloc_kernel(device->port);

	    device_lock(device);
	    device->state = DEV_STATE_INIT;
	    if (device->io_wait) {
		device->io_wait = FALSE;
//		thread_wakeup((event_t)device);
	    }
	    device_unlock(device);

//	    mach_device_deallocate(device);
	    device = MACH_DEVICE_NULL;
	}
	else {
	    /*
	     * Open succeeded.
	     */
	    device_lock(device);
	    device->state = DEV_STATE_OPEN;
	    device->open_count = 1;
	    if (device->io_wait) {
		device->io_wait = FALSE;
//		thread_wakeup((event_t)device);
	    }
	    device_unlock(device);

	    /* donate device reference to get port */
	}
	/*
	 * Must explicitly convert device to port, since
	 * device_reply interface is built as 'user' side
	 * (thus cannot get translation).
	 */
	if (MACH_PORT_VALID(ior->io_reply_port)) {
		(void) ds_device_open_reply(ior->io_reply_port,
					    ior->io_reply_port_type,
					    result,
					    mach_convert_device_to_port(device));
	} 
//	else
//		mach_device_deallocate(device);

	mach_device_deallocate (device);
	return (TRUE);
}

boolean_t ds_read_done(ior)
	io_req_t	ior;
{
	vm_offset_t		start_data, end_data;
	vm_offset_t		start_sent, end_sent;
	register vm_size_t	size_read;

	if (ior->io_error)
	    size_read = 0;
	else
	    size_read = ior->io_count - ior->io_residual;

	start_data  = (vm_offset_t)ior->io_data;
	end_data    = start_data + size_read;

	start_sent  = (ior->io_op & IO_INBAND) ? start_data :
						trunc_page(start_data);
	end_sent    = (ior->io_op & IO_INBAND) ?
		start_data + ior->io_alloc_size : round_page(end_data);

	/*
	 * Zero memory that the device did not fill.
	 */
	if (start_sent < start_data)
	    memset((char *)start_sent, 0, start_data - start_sent);
	if (end_sent > end_data)
	    memset((char *)end_data, 0, end_sent - end_data);


	/*
	 * Touch the data being returned, to mark it dirty.
	 * If the pages were filled by DMA, the pmap module
	 * may think that they are clean.
	 */
	{
	    register vm_offset_t	touch;
	    register int		c;

	    for (touch = start_sent; touch < end_sent; touch += PAGE_SIZE) {
		c = *(volatile char *)touch;
		*(volatile char *)touch = c;
	    }
	}

	/*
	 * Send the data to the reply port - this
	 * unwires and deallocates it.
	 */
	if (ior->io_op & IO_INBAND) {
	    (void)ds_device_read_reply_inband(ior->io_reply_port,
					      ior->io_reply_port_type,
					      ior->io_error,
					      (char *) start_data,
					      size_read);
	} else {
//	    vm_map_copy_t copy;
//	    kern_return_t kr;
//
//	    kr = vm_map_copyin_page_list(kernel_map, start_data,
//					 size_read, TRUE, TRUE,
//					 &copy, FALSE);
//
//	    if (kr != KERN_SUCCESS)
//		panic("read_done: vm_map_copyin_page_list failed");

	    (void)ds_device_read_reply(ior->io_reply_port,
				       ior->io_reply_port_type,
				       ior->io_error,
				       (char *) start_data,
				       size_read);
	}

	/*
	 * Free any memory that was allocated but not sent.
	 */
	if (ior->io_count != 0) {
	    if (ior->io_op & IO_INBAND) {
		if (ior->io_alloc_size > 0)
		    free (ior->io_data);
//		    zfree(io_inband_zone, (vm_offset_t)ior->io_data);
	    } else {
		register vm_offset_t	end_alloc;

		end_alloc = start_sent + round_page(ior->io_alloc_size);
		if (end_alloc > end_sent)
		  vm_deallocate(mach_task_self (),
				end_sent,
				end_alloc - end_sent);
	    }
	}

	mach_device_deallocate(ior->io_device);

	return (TRUE);
}

/*
 * Allocate wired-down memory for device read.
 */
kern_return_t device_read_alloc(ior, size)
	register io_req_t	ior;
	register vm_size_t	size;
{
	vm_offset_t		addr;
	kern_return_t		kr;

	/*
	 * Nothing to do if no data.
	 */
	if (ior->io_count == 0)
	    return (KERN_SUCCESS);

	if (ior->io_op & IO_INBAND) {
	    ior->io_data = (io_buf_ptr_t) malloc(sizeof(io_buf_ptr_inband_t));
	    ior->io_alloc_size = sizeof(io_buf_ptr_inband_t);
	} else {
	    size = round_page(size);
	    kr = vm_allocate (mach_task_self (), &addr, size, TRUE);
//	    kr = kmem_alloc(kernel_map, &addr, size);
	    if (kr != KERN_SUCCESS)
		return (kr);

	    ior->io_data = (io_buf_ptr_t) addr;
	    ior->io_alloc_size = size;
	}

	return (KERN_SUCCESS);
}

struct thread_wait
{
  struct condition cond;
  struct mutex mutex;
  int v;
};

static struct thread_wait io_done_wait;

void thread_wait_init (struct thread_wait *t)
{
  mutex_init (&t->mutex);
  condition_init (&t->cond);
  t->v = 0;
}

void thread_block (struct thread_wait *t)
{
  mutex_lock (&t->mutex);
  t->v = 1;
  while (t->v)
    hurd_condition_wait (&t->cond, &t->mutex);
  mutex_unlock (&t->mutex);
}

void thread_wakeup (struct thread_wait *t)
{
  mutex_lock (&t->mutex);
  t->v = 0;
  condition_signal (&t->cond);
  mutex_unlock (&t->mutex);
}

queue_head_t		io_done_list;
struct mutex		io_done_list_lock;

#define	splio	splsched	/* XXX must block ALL io devices */

void iodone(ior)
	register io_req_t	ior;
{
	/*
	 * If this ior was loaned to us, return it directly.
	 */
	if (ior->io_op & IO_LOANED) {
		(*ior->io_done)(ior);
		return;
	}
	/*
	 * If !IO_CALL, some thread is waiting for this.  Must lock
	 * structure to interlock correctly with iowait().  Else can
	 * toss on queue for io_done thread to call completion.
	 */
	// TODO need a lock here?
//	s = splio();
	if ((ior->io_op & IO_CALL) == 0) {
	    ior_lock(ior);
	    ior->io_op |= IO_DONE;
	    ior->io_op &= ~IO_WANTED;
	    ior_unlock(ior);
//	    thread_wakeup((event_t)ior);
	} else {
	    ior->io_op |= IO_DONE;
	    mutex_lock (&io_done_list_lock);
	    enqueue_tail(&io_done_list, (queue_entry_t)ior);
	    thread_wakeup (&io_done_wait);
//	    thread_wakeup((event_t)&io_done_list);
	    mutex_unlock (&io_done_list_lock);
	}
//	splx(s);
}

void wakeup_io_done_thread ()
{
	thread_wakeup (&io_done_wait);
}

void io_done_thread_continue()
{
	for (;;) {
	    extern void free_skbuffs ();
	    register io_req_t	ior;

	    free_skbuffs ();
	    mutex_lock(&io_done_list_lock);
	    while ((ior = (io_req_t)dequeue_head(&io_done_list)) != 0) {
		mutex_unlock(&io_done_list_lock);

		if ((*ior->io_done)(ior)) {
		    /*
		     * IO done - free io_req_elt
		     */
		    io_req_free(ior);
		}
		/* else routine has re-queued it somewhere */

		mutex_lock(&io_done_list_lock);
	    }

//	    assert_wait(&io_done_list, FALSE);
	    mutex_unlock(&io_done_list_lock);
//	    counter(c_io_done_thread_block++);
//	    thread_block(io_done_thread_continue);
	    thread_block (&io_done_wait);
	}
}


void
wire_thread()
{
  kern_return_t   kr;
  mach_port_t priv_host_port;

  kr = get_privileged_ports (&priv_host_port, NULL);
  if (kr != KERN_SUCCESS)
    panic("get privileged port: %d", kr);

  kr = thread_wire(priv_host_port,
		   mach_thread_self(),
		   TRUE);
  if (kr != KERN_SUCCESS)
    panic("wire_thread: %d", kr);
}

void
thread_set_own_priority (int priority)
{
  kern_return_t   kr;
  mach_port_t priv_host_port;
  mach_port_t pset, psetcntl;

  kr = get_privileged_ports (&priv_host_port, NULL);
  if (kr != KERN_SUCCESS)
    panic("get privileged port: %d", kr);

  kr = thread_get_assignment (mach_thread_self (), &pset);
  if (kr != KERN_SUCCESS)
    panic("thread get assignment: %d", kr);
  kr = host_processor_set_priv (priv_host_port, pset, &psetcntl);
  if (kr != KERN_SUCCESS)
    panic("processor set priv: %d", kr);
  kr = thread_max_priority (mach_thread_self (), psetcntl, 0);
  if (kr != KERN_SUCCESS)
    panic("set thread max priority: %d", kr);
  kr = thread_priority (mach_thread_self (), 0, FALSE);
  if (kr != KERN_SUCCESS)
    panic("set thread priority: %d", kr);
}

static any_t io_done_thread(any_t unused)
{
	/*
	 * Set thread privileges and highest priority.
	 */
//	current_thread()->vm_privilege = TRUE;
//	stack_privilege(current_thread());
	wire_thread ();

	thread_set_own_priority(0);

	io_done_thread_continue();
	/*NOTREACHED*/
	return 0;
}

int create_device_port (int size, void *result)
{
  return ports_create_port (dev_class, port_bucket,
			    size, result);
}

void mach_device_init()
{
	int i;

	queue_init(&io_done_list);
	mutex_init (&io_done_list_lock);
	thread_wait_init (&io_done_wait);

	port_bucket = ports_create_bucket ();
	dev_class = ports_create_class (0, 0);

	for (i = 0; i < NUM_EMULATION; i++) {
		emulation_list[i]->init();
	}

	cthread_detach (cthread_fork (io_done_thread, 0));
}

static int
demuxer (mach_msg_header_t *inp, mach_msg_header_t *outp)
{
  int ret;
  extern int device_server (mach_msg_header_t *, mach_msg_header_t *);
  extern int notify_server (mach_msg_header_t *, mach_msg_header_t *);
  ret = device_server (inp, outp) || notify_server (inp, outp);
  return ret;
}

void ds_server()
{
  /* Launch.  */
  do
    {
      ports_manage_port_operations_one_thread (port_bucket, demuxer, 0);
    } while (1);
}
