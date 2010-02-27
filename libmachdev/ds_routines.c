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

int create_device_port (int size, void *result)
{
  return ports_create_port (dev_class, port_bucket,
			    size, result);
}

void mach_device_init()
{
	int i;

	port_bucket = ports_create_bucket ();
	dev_class = ports_create_class (0, 0);

	for (i = 0; i < NUM_EMULATION; i++) {
		emulation_list[i]->init();
	}
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
