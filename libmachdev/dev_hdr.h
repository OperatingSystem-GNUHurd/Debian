/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
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
 * Mach device emulation definitions (i386at version).
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

#ifndef	_DEVICE_DEV_HDR_H_
#define	_DEVICE_DEV_HDR_H_

#include <mach.h>
#include <hurd.h>
#include <hurd/ports.h>
#include <pthread.h>

#include "device_emul.h"

/*
 * Operations list for major device types.
 */
struct dev_ops {
	char *    	d_name;		/* name for major device */
	int		(*d_open)();	/* open device */
	int		(*d_close)();	/* close device */
	int		(*d_read)();	/* read */
	int		(*d_write)();	/* write */
	int		(*d_getstat)();	/* get status/control */
	int		(*d_setstat)();	/* set status/control */
	vm_offset_t	(*d_mmap)();	/* map memory */
	int		(*d_async_in)();/* asynchronous input setup */
	int		(*d_reset)();	/* reset device */
	int		(*d_port_death)();
					/* clean up reply ports */
	int		d_subdev;	/* number of sub-devices per
					   unit */
	int		(*d_dev_info)(); /* driver info for kernel */
};
typedef struct dev_ops *dev_ops_t;

/* This structure is associated with each open device port.
 * The port representing the device points to this structure.  */
struct emul_device
{
    struct device_emulation_ops *emul_ops;
    void *emul_data;
};

typedef struct emul_device *emul_device_t;

#define DEVICE_NULL     ((device_t) 0)

/*
 * Generic device header.  May be allocated with the device,
 * or built when the device is opened.
 */
struct mach_device {
	struct port_info port;
	struct emul_device	dev;		/* the real device structure */
};
typedef	struct mach_device *mach_device_t;
#define	MACH_DEVICE_NULL ((mach_device_t)0)

/*
 * To find and remove device entries
 */
mach_device_t	device_lookup(char *);	/* by name */

/*
 * To find and remove port-to-device mappings
 */
void		dev_port_enter(mach_device_t);
void		dev_port_remove(mach_device_t);

/*
 * To call a routine on each device
 */
boolean_t	dev_map(boolean_t (*)(), mach_port_t);

/*
 * To lock and unlock state and open-count
 */
#define	device_lock(device)	pthread_mutex_lock(&(device)->lock)
#define	device_unlock(device)	pthread_mutex_unlock(&(device)->lock)

#endif	/* _DEVICE_DEV_HDR_H_ */
