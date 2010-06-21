/*
 * Linux block driver support.
 *
 * Copyright (C) 1996 The University of Utah and the Computer Systems
 * Laboratory at the University of Utah (CSL)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *	Author: Shantanu Goel, University of Utah CSL
 */

#include <ctype.h>
#include <stdio.h>
#include <unistd.h>

#include "mach_U.h"

#include <mach.h>
#include <hurd.h>

#define MACH_INCLUDE

#include <ddekit/printf.h>

#include "vm_param.h"
#include "device_reply_U.h"
#include "dev_hdr.h"
#include "util.h"
#include "mach_glue.h"

/* One of these is associated with each open instance of a device.  */
struct block_data
{
  struct port_info port;	/* device port */
  struct emul_device device;	/* generic device structure */
  dev_mode_t mode;
  struct block_device *dev;
};

/* Return a send right associated with network device ND.  */
static mach_port_t
dev_to_port (void *nd)
{
  return (nd
	  ? ports_get_send_right (nd)
	  : MACH_PORT_NULL);
}

extern struct device_emulation_ops linux_block_emulation_ops;

#define DISK_NAME_LEN 32

/* Parse the device NAME.
   Set *SLICE to be the DOS partition and
   *PART the BSD/Mach partition, if any.  */
static char *
translate_name (char *name, int *slice, int *part)
{
  char *p, *q, *end;
  char *ret;
  int disk_num;

  /* Parse name into name, unit, DOS partition (slice) and partition.  */
  for (*slice = 0, *part = -1, p = name; isalpha (*p); p++)
    ;
  if (p == name || ! isdigit (*p))
    return NULL;
  end = p;
  disk_num = strtol (p, &p, 0);
  if (disk_num < 0 || disk_num > 26)
    return NULL;
//  do
//    p++;
//  while (isdigit (*p));
  if (*p)
    {
      q = p;
      if (*q == 's' && isdigit (*(q + 1)))
	{
	  q++;
	  do
	    *slice = *slice * 10 + *q++ - '0';
	  while (isdigit (*q));
	  if (! *q)
	    goto find_major;
	}
      if (! isalpha (*q) || *(q + 1))
	return NULL;
      *part = *q - 'a';
    }

find_major:
  ret = malloc (DISK_NAME_LEN);
  sprintf (ret, "hd%c", 'a' + disk_num);
  return ret;
}

static io_return_t
device_open (mach_port_t reply_port, mach_msg_type_name_t reply_port_type,
	     dev_mode_t mode, char *name, device_t *devp,
	     mach_msg_type_name_t *devicePoly)
{
  io_return_t err = D_SUCCESS;
  struct block_data *bd = NULL;
  int slice, part;
  char *dev_name = NULL;
  int dev_err;

  dev_name = translate_name (name, &slice, &part);

  // TODO when the port isn't used by clients, it should be destroyed.
  err = create_device_port (sizeof (*bd), &bd);
  if (err)
    {
      ddekit_printf ("after create_device_port: cannot create a port\n");
      goto out;
    }
  bd->dev = open_block_dev (dev_name, slice, mode);
  dev_err = (int) bd->dev;
  if (dev_err < 0)
    {
      ddekit_printf ("open_block_dev %s fails with %d\n", dev_name, bd->dev);
      err = linux_to_mach_error (dev_err);
      goto out;
    }
  bd->device.emul_data = bd;
  bd->device.emul_ops = &linux_block_emulation_ops;
  bd->mode = mode;

out:
  free (dev_name);
  if (err)
    {
      if (bd)
	{
	  ports_destroy_right (bd);
	  bd = NULL;
	}
    }
  else
    {
      *devp = ports_get_send_right (bd);
      ports_port_deref (bd);
      *devicePoly = MACH_MSG_TYPE_MOVE_SEND;
    }
  return err;
}

static io_return_t
device_write (void *d, mach_port_t reply_port,
	      mach_msg_type_name_t reply_port_type, dev_mode_t mode,
	      recnum_t bn, io_buf_ptr_t data, unsigned int count,
	      int *bytes_written)
{
  struct block_data *bd = d;
  /* the number of pages that contain DATA. */
  int npages = (((int) data + count) - ((int) data & ~PAGE_MASK)
		+ PAGE_MASK) / PAGE_SIZE;
  io_return_t err = D_SUCCESS;
  int i;
  int writes = 0;

  void write_done (int err)
    {
      int len = err ? 0 : count;
      writes--;
      if (writes == 0)
	{
	  err = linux_to_mach_error (err);
	  ds_device_write_reply (reply_port, reply_port_type, err, len);
	}
    }

  /* the data is at the beginning of a page. */
  if ((int) data & ~PAGE_MASK)
    return D_INVALID_OPERATION;
  
  if ((bd->mode & D_WRITE) == 0)
    return D_INVALID_OPERATION;

  for (i = 0; i < npages; i++)
    {
      int size = PAGE_SIZE - ((int) data &~PAGE_MASK) > count ?
	PAGE_SIZE - ((int) data &~PAGE_MASK) : count;

      err = block_dev_write (bd->dev, bn, data, count, write_done);
      if (err)
	break;
      bn += count >> 9;
      data += size;
      count -= size;
      writes++;
    }
  if (writes)
    return MIG_NO_REPLY;
  return linux_to_mach_error (err);
}

static io_return_t
device_read (void *d, mach_port_t reply_port,
	     mach_msg_type_name_t reply_port_type, dev_mode_t mode,
	     recnum_t bn, int count, io_buf_ptr_t *data,
	     unsigned *bytes_read)
{
  struct block_data *bd = d;

  if ((bd->mode & D_READ) == 0)
    return D_INVALID_OPERATION;
  *bytes_read = 0;
  return D_SUCCESS;
}

static io_return_t
device_get_status (void *d, dev_flavor_t flavor, dev_status_t status,
		   mach_msg_type_number_t *count)
{
  struct block_data *bd = (struct block_data *) d;
  return D_SUCCESS;
}

struct device_emulation_ops linux_block_emulation_ops =
{
  NULL,
  NULL,
  NULL,
  dev_to_port,
  device_open,
  NULL,
  device_write,
  NULL,
  device_read,
  NULL,
  NULL,
  device_get_status,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

void register_block()
{
	extern void reg_dev_emul (struct device_emulation_ops *ops);
	reg_dev_emul (&linux_block_emulation_ops);
}
