/* 
   Copyright (C) 2008 Free Software Foundation, Inc.
   Written by Zheng Da.

   This file is part of the GNU Hurd.

   The GNU Hurd is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   The GNU Hurd is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the GNU Hurd; see the file COPYING.  If not, write to
   the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <fcntl.h>
#include <mach.h>
#include <hurd.h>
#include <hurd/ports.h>
#include <hurd/netfs.h>
#include <device/device.h>

#include "ethernet.h"
#include "vdev.h"
#include "ourdevice_S.h"
#include "notify_S.h"
#include "bpf_impl.h"
#include "netfs_impl.h"
#include "util.h"

extern struct port_bucket *port_bucket;
extern struct port_class *vdev_portclass;
extern struct port_class *other_portclass;
extern struct port_info *notify_pi;

/* Implementation of device interface */
kern_return_t
ds_xxx_device_set_status (device_t device, dev_flavor_t flavor,
			  dev_status_t status, size_t statu_cnt)
{
  struct vether_device *vdev = ports_lookup_port (port_bucket, device,
						  vdev_portclass);
  if (vdev == NULL)
    return D_NO_SUCH_DEVICE;
  ports_port_deref (vdev);
  return D_INVALID_OPERATION;
}

kern_return_t
ds_xxx_device_get_status (device_t device, dev_flavor_t flavor,
			  dev_status_t status, size_t *statuscnt)
{
  struct vether_device *vdev = ports_lookup_port (port_bucket, device,
						  vdev_portclass);
  if (vdev == NULL)
    return D_NO_SUCH_DEVICE;
  ports_port_deref (vdev);
  return D_INVALID_OPERATION;
}

kern_return_t
ds_xxx_device_set_filter (device_t device, mach_port_t rec,
			  int pri, filter_array_t filt, size_t len)
{
  struct vether_device *vdev = ports_lookup_port (port_bucket, device,
						  vdev_portclass);
  if (vdev == NULL)
    return D_NO_SUCH_DEVICE;
  ports_port_deref (vdev);
  return D_INVALID_OPERATION;
}

/*
 * This function is currently running in the multithread environment,
 * it should be protected by locks.
 */
kern_return_t
ds_device_open (mach_port_t master_port, mach_port_t reply_port,
		mach_msg_type_name_t reply_portPoly,
		dev_mode_t mode, dev_name_t name, mach_port_t *device, 
	 	mach_msg_type_name_t *devicetype)
{
  struct vether_device *dev;
  int openstat; 
  int right_mode = 1;
  struct protid *pi = ports_lookup_port (netfs_port_bucket, master_port, 0);
  if (pi == NULL)
    return D_NO_SUCH_DEVICE;

  /* If the virtual device hasn't been created yet,
   * create it now. */
  if (pi->po->np->nn->ln == NULL)
    {
      extern struct port_bucket *port_bucket;
      extern struct port_class *vdev_portclass;
      extern struct stat underlying_node_stat;
      static int ino_count = 0;
      /* Create a new light node (virtual device). */
      struct lnode *ln = (struct lnode *) add_vdev (pi->po->np->nn->name,
						    sizeof (*ln),
						    vdev_portclass,
						    port_bucket);
      if (ln == NULL)
	{
	  ports_port_deref (pi);
	  return D_NO_MEMORY;
	}
      memset (&ln->st, 0, sizeof (ln->st));
      ln->st.st_ino = ++ino_count;
      ln->st.st_mode = S_IFCHR | (underlying_node_stat.st_mode & ~S_IFMT);
      ln->st.st_ctime = ln->st.st_mtime = ln->st.st_atime = time (NULL);
      fshelp_touch (&ln->st, TOUCH_ATIME|TOUCH_MTIME|TOUCH_CTIME,
		    multiplexer_maptime);
      pi->po->np->nn->ln = ln;
    }

  dev = (struct vether_device *) pi->po->np->nn->ln;
  /* check the mode */
  openstat = pi->po->openstat;
  if (mode & D_READ && !(openstat & O_READ))
    right_mode = 0;
  if (mode & D_WRITE && !(openstat & O_WRITE))
    right_mode = 0;
  ports_port_deref (pi);

  if (dev)
    {
      if (!right_mode)
	return EBADF;
      *device = dev->dev_port;
      *devicetype = MACH_MSG_TYPE_MAKE_SEND;
      return 0;
    }
  return D_NO_SUCH_DEVICE;
}

kern_return_t
ds_device_close (device_t device)
{
  return 0;
}

kern_return_t
ds_device_write (device_t device, mach_port_t reply_port,
		 mach_msg_type_name_t reply_type, dev_mode_t mode,
		 recnum_t recnum, io_buf_ptr_t data, size_t datalen,
		 int *bytes_written)
{
  kern_return_t ret = 0;
  struct vether_device *vdev = ports_lookup_port (port_bucket, device,
						  vdev_portclass);
  if (vdev == NULL)
    {
      vm_deallocate (mach_task_self (), data, datalen);
      return D_NO_SUCH_DEVICE;
    }
  /* The packet is forwarded to all virtual interfaces and
   * the interface which the multiplexer connects to. */
  broadcast_pack (data, datalen, vdev);
  *bytes_written = datalen;
  if(ether_port != MACH_PORT_NULL)
    ret = device_write (ether_port, mode , recnum ,
			data, datalen, bytes_written);
  /* The data in device_write() is transmifered out of line,
   * so the server-side function has to deallocate it. */
  vm_deallocate (mach_task_self (), data, datalen);
  ports_port_deref (vdev);
  return ret;
}

kern_return_t
ds_device_write_inband (device_t device, mach_port_t reply_port,
			mach_msg_type_name_t reply_type, dev_mode_t mode,
			recnum_t recnum, io_buf_ptr_inband_t data,
			size_t datalen, int *bytes_written)
{
  struct vether_device *vdev = ports_lookup_port (port_bucket, device,
						  vdev_portclass);
  if (vdev == NULL)
    return D_NO_SUCH_DEVICE;
  ports_port_deref (vdev);
  return D_INVALID_OPERATION;
}

kern_return_t
ds_device_read (device_t device, mach_port_t reply_port,
		mach_msg_type_name_t reply_type, dev_mode_t mode,
		recnum_t recnum, int bytes_wanted,
		io_buf_ptr_t *data, size_t *datalen)
{
  struct vether_device *vdev = ports_lookup_port (port_bucket, device,
						  vdev_portclass);
  if (vdev == NULL)
    return D_NO_SUCH_DEVICE;
  ports_port_deref (vdev);
  return D_INVALID_OPERATION;
}

kern_return_t
ds_device_read_inband (device_t device, mach_port_t reply_port,
		       mach_msg_type_name_t reply_type, dev_mode_t mode,
		       recnum_t recnum, int bytes_wanted,
		       io_buf_ptr_inband_t data, size_t *datalen)
{
  struct vether_device *vdev = ports_lookup_port (port_bucket, device,
						  vdev_portclass);
  if (vdev == NULL)
    return D_NO_SUCH_DEVICE;
  ports_port_deref (vdev);
  return D_INVALID_OPERATION;
}

kern_return_t
ds_device_map (device_t device, vm_prot_t prot, vm_offset_t offset,
	       vm_size_t size, memory_object_t *pager, int unmap)
{
  struct vether_device *vdev = ports_lookup_port (port_bucket, device,
						  vdev_portclass);
  if (vdev == NULL)
    return D_NO_SUCH_DEVICE;
  ports_port_deref (vdev);
  return D_INVALID_OPERATION;
}

kern_return_t
ds_device_set_status (device_t device, dev_flavor_t flavor,
		      dev_status_t status, size_t statuslen)
{
  struct vether_device *vdev = ports_lookup_port (port_bucket, device,
						  vdev_portclass);
  if (vdev == NULL)
    return D_NO_SUCH_DEVICE;
  ports_port_deref (vdev);
  return D_INVALID_OPERATION;
}

kern_return_t
ds_device_get_status (device_t device, dev_flavor_t flavor,
		      dev_status_t status, size_t *statuslen)
{
  extern io_return_t dev_getstat (struct vether_device *, dev_flavor_t,
				  dev_status_t, natural_t *);
  kern_return_t ret = 0;
  struct vether_device *vdev = ports_lookup_port (port_bucket, device,
						  vdev_portclass);
  if (vdev == NULL)
    return D_NO_SUCH_DEVICE;
  if(ether_port != MACH_PORT_NULL)
    ret = device_get_status (ether_port, flavor, status, statuslen);
  else 
    ret = dev_getstat (vdev, flavor, status, statuslen);
  ports_port_deref (vdev);
  return ret;
}

kern_return_t
ds_device_set_filter (device_t device, mach_port_t receive_port,
		      int priority, filter_array_t filter, size_t filterlen)
{
  mach_port_t tmp;
  kern_return_t err;
  struct vether_device *vdev = ports_lookup_port (port_bucket, device,
						  vdev_portclass);
  if (vdev == NULL)
    return D_NO_SUCH_DEVICE;
  err = mach_port_request_notification (mach_task_self (), receive_port, 
					MACH_NOTIFY_DEAD_NAME, 0,
					ports_get_right (notify_pi), 
					MACH_MSG_TYPE_MAKE_SEND_ONCE, &tmp);
  if (err != KERN_SUCCESS)
    goto out;
  if (tmp != MACH_PORT_NULL)
    mach_port_deallocate (mach_task_self (), tmp);
  err = net_set_filter (&vdev->port_list, receive_port,
			priority, filter, filterlen);
out:
  ports_port_deref (vdev);
  return err;
}
