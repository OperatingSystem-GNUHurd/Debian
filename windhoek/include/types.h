/**
 * \file   windhoek/include/types.h
 * \brief  Windhoek data types
 *
 * \date   2008-02-15
 * \author Bjoern Doebel <doebel@tudos.org>
 *
 * (c) 2008 Technische Universitaet Dresden
 * This file is part of DROPS, which is distributed under the terms of the
 * GNU General Public License 2. Please see the COPYING file for details.
 */

#ifndef WINDHOEK_DATA_TYPES_H
#define WINDHOEK_DATA_TYPES_H

#include <l4/dm_generic/types.h>
#include <l4/log/l4log.h>
#include <l4/util/macros.h>

#define MAX_NAME_LEN    16

/** Windhoek BIO types */
enum
{
	BIO_READ,
	BIO_WRITE
};

/*
 * Windhoek manages connections using dataspaces. Every connection is
 * represented by a server-side handle that the client needs to use whenever
 * it calls Windhoek. Using this handle, the server manages client state
 * containing exactly one control dataspace (CTRL) and an arbitrary number of
 * data dataspaces (DATA).
 *
 * CTRL layout: 
 *
 *     +----------------------------------------------------------+
 *     | HEAD                                                     |
 *     |    * device description (struct windhoek_device)         |
 *     +----------------------------------------------------------+
 *     | REQUEST DESCRIPTORS                                      |
 *     |    * array of request descriptors                        |
 *     |      (struct windhoek_bio)                               |
 *     |                                                          |
 *     |    ...                                                   |
 *     +----------------------------------------------------------+
 *
 *
 *
 * DATA layout: 
 *
 * DATA spaces contain arbitrary block data and are managed by
 * the client. For requests, the client allocates an appropriate region inside
 * a DATA space and hands over this dataspace's ID and an offset along with
 * the request struct to the server.
 */


/*
 * Handle to a  block device. Retrieved during open() call.
 */
typedef l4_int32_t      blockdev_handle_t;


/** Information about a Windhoek block device
 */
typedef struct
{
	unsigned int        start_sector;          /* First sector on disk belonging
	                                              to this dev */
	unsigned int        block_size;            /* sector size */
	unsigned int        cylinders;
	unsigned int        heads;
	unsigned int        sectors;               /* Number of sectors per cylinder */
	unsigned int        capacity;              /* total no of sectors */
	unsigned long       flags;                 /* flags */
	char                name[MAX_NAME_LEN];    /* device name */
	blockdev_handle_t   handle;                /* device handle */
} windhoek_device;


/** Dump block device info
 */
L4_INLINE void dump_windhoek_device(windhoek_device *dev);
L4_INLINE void dump_windhoek_device(windhoek_device *dev)
{
	LOG("Windhoek device '%s', @ %p", dev->name, dev);
	LOG("\tsectors %d, startsect %x", dev->sectors, dev->start_sector);
	LOG("\tblock size %d, heads %d, cyl %d", dev->block_size,
	    dev->heads, dev->cylinders);
	LOG("\tflags %lx, handle %x", dev->flags, dev->handle);
}


/** Just like BDDF, a BIO consists of one or more chunks inside a dataspace
 */
typedef struct
{
	l4dm_dataspace_t    ds;           /**< dataspace ID */
	unsigned int        offset;       /**< offset in DS */
	unsigned int        size;         /**< chunk size   */
} ds_list_element;


L4_INLINE void dump_ds_element(ds_list_element *d);
L4_INLINE void dump_ds_element(ds_list_element *d)
{
	LOG("ds id %04x, offset %x, size %x", d->ds.id, d->offset, d->size);
}


/** BIO struct used for transmitting requests
 */
typedef struct
{
	unsigned int        uid;          /**< unique identifier - managed by clients */
	unsigned int        type;         /**< bio type */
	blockdev_handle_t   handle;       /**< handle to block device */
	unsigned int        start_sector; /**< start sector of request */
	unsigned short		sg_count;     /**< number of SG elements */
	/* XXX: Must be last element! */
	ds_list_element     sg_data[0];   /**< SG data */
} windhoek_bio;


L4_INLINE void dump_windhoek_bio(windhoek_bio *bio);
L4_INLINE void dump_windhoek_bio(windhoek_bio *bio)
{
	LOG("bio type %s", bio->type == BIO_READ ? "read" : "write");
	LOG("handle %x, start sect %x, sg_count %x", bio->handle,
	    bio->start_sector, bio->sg_count);
}

#endif
