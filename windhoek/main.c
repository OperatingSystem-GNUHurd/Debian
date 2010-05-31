/**
 * \file   windhoek/server/src/main.c
 * \brief  Windhoek main function
 *
 * \date   2008-01-29
 * \author Bjoern Doebel <doebel@tudos.org>
 *
 * (c) 2008 Technische Universitaet Dresden
 * This file is part of DROPS, which is distributed under the terms of the
 * GNU General Public License 2. Please see the COPYING file for details.
 */
#include <asm/current.h>

#include <linux/kernel.h>
#include <linux/completion.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/genhd.h>
#include <linux/bio.h>
#include <linux/hdreg.h>
#include <linux/list.h>
#include <linux/buffer_head.h>

#include <dde.h>
#include <ddekit/initcall.h>
#include <ddekit/assert.h>
#include <dde26.h>

extern int bdev_cache_init(void);
extern int ide_generic_init(void);
extern int ide_cdrom_init(void);
extern int genhd_device_init(void);

int using_std = 1;

int main(int argc, const char **argv)
{
	extern void ds_server(void);
	int err;

	l4dde26_init();
	l4dde26_process_init();
	l4dde26_init_timers();
	l4dde26_softirq_init();
	printk("DDE base system initialized.\n");
	err = bdev_cache_init();
	printk("Initialized blockdev caches. (%x)\n", err);

	gendisk_init();
	l4dde26_do_initcalls();

	/* no generic driver, we use a dedicated one
	 * XXX: make this a fallback if no other driver is found */
#if 0
	err = ide_generic_init();
	printk("Initialized generic IDE driver. (%x)\n", err);
#endif

	printk("+----------------------------------------+\n");
	printk("| Windhoek block server                  |\n");
	printk("| ready to rumble....                    |\n");
	printk("+----------------------------------------+\n");

	mach_device_init();
	trivfs_init();

	cthread_detach (cthread_fork (ds_server, NULL));
	trivfs_server();

	return 0;
}
