/**
 * \file   windhoek/server/src/genhd.c
 * \brief  Windhoek genhd replacement
 *
 * \date   2008-01-29
 * \author Bjoern Doebel <doebel@tudos.org>
 *
 * (c) 2008 Technische Universitaet Dresden
 * This file is part of DROPS, which is distributed under the terms of the
 * GNU General Public License 2. Please see the COPYING file for details.
 */

#include <linux/genhd.h>
#include <linux/hdreg.h>
#include <linux/ide.h>

#include "windhoek_local.h"

/*
 * Array for storing all known block devices
 */
struct gendisk *known_disks[MAX_GENDISKS];

static void print_gendisk(struct gendisk *gendisk)
{
	struct block_device *bdev = bdget_disk(gendisk, 0);
	struct hd_geometry geom;

	DEBUG_MSG("disk: %p (maj %x, min %x, max_min %x)", gendisk,
	          gendisk->major, gendisk->first_minor, gendisk->minors);
	DEBUG_MSG("\tdisk name '%s'", gendisk->disk_name);
//	DEBUG_MSG("\tpartitions @ %p, fops @ %p", gendisk->part, gendisk->fops);
	DEBUG_MSG("\trq @ %p, privdata @ %p", gendisk->queue, gendisk->private_data);
	DEBUG_MSG("\tcapacity %d, flags %x, dev %p", get_capacity(gendisk), gendisk->flags,
	          gendisk->driverfs_dev);
}


void add_disk(struct gendisk *disk)
{
	int i, err;
	dev_t devt;
	struct block_device *bdev;
	
	/* minors == 0 indicates to use ext devt from part0 and should
	 * be accompanied with EXT_DEVT flag.  Make sure all
	 * parameters make sense.
	 */
	WARN_ON(disk->minors && !(disk->major || disk->first_minor));
	WARN_ON(!disk->minors && !(disk->flags & GENHD_FL_EXT_DEVT));

	for (i = 0; i < MAX_GENDISKS; ++i) {
		if (known_disks[i] == NULL)
			break;
	}	

	print_gendisk(disk);
	known_disks[i] = disk;

	disk->flags |= GENHD_FL_UP;

	err = blk_alloc_devt(&disk->part0, &devt);
	if (err) {
		WARN_ON(1);
		return;
	}
	disk_to_dev(disk)->devt = devt;

	/* ->major and ->first_minor aren't supposed to be
	 * dereferenced from here on, but set them just in case.
	 */
	disk->major = MAJOR(devt);
	disk->first_minor = MINOR(devt);

//	blk_register_region(disk_devt(disk), disk->minors, NULL,
//			    exact_match, exact_lock, disk);
	blk_register_queue(disk);

	bdev = bdget_disk(disk, 0);
	BUG_ON(!bdev);

	bdev->bd_invalidated = 0;
	err = blkdev_get(bdev, FMODE_READ);
	BUG_ON(err);
	//blkdev_put(bdev);
}


struct gendisk *get_gendisk(dev_t dev, int *part)
{
	int i;

	// XXX: no partitions
	*part = 0;

	for (i = 0; i < MAX_GENDISKS; ++i) {
		if (!known_disks[i])
			continue;

		if (MKDEV(known_disks[i]->major, known_disks[i]->first_minor) == dev)
			return known_disks[i];
	}

	return NULL;
}


struct gendisk* find_disk_by_name(const char* name)
{
	struct gendisk *ret = NULL;
	int i;

	for (i = 0; i < MAX_GENDISKS; ++i) {
		if (!known_disks[i])
			continue;

		if (!strcmp(known_disks[i]->disk_name, name)) {
			ret = known_disks[i];
			break;
		}
	}

	return ret;
}


void gendisk_init()
{
	int i;
	for (i = 0; i < MAX_GENDISKS; ++i)
		known_disks[i] = NULL;
}
