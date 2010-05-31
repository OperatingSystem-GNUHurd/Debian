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

#include <l4/dde/dde.h>
#include <l4/dde/ddekit/initcall.h>
#include <l4/dde/ddekit/assert.h>
#include <l4/dde/linux26/dde26.h>

#include <l4/util/parse_cmd.h>
#include <l4/util/util.h>
#include <l4/log/l4log.h>
#include <l4/names/libnames.h>

#include "windhoek_local.h"
#include <l4/windhoek/server-internal-server.h>

extern int bdev_cache_init(void);
extern int ide_generic_init(void);
extern int ide_cdrom_init(void);
extern int genhd_device_init(void);

l4_threadid_t main_thread = L4_INVALID_ID;

int main(int argc, const char **argv)
{
	int err;

	l4dde26_init();
	l4dde26_process_init();
	l4dde26_init_timers();
	l4dde26_softirq_init();
	printk("DDE base system initialized.\n");
	err = bdev_cache_init();
	printk("Initialized blockdev caches. (%x)\n", err);

	gendisk_init();
	client_state_init();
	l4dde26_do_initcalls();

	/* no generic driver, we use a dedicated one
	 * XXX: make this a fallback if no other driver is found */
#if 0
	err = ide_generic_init();
	printk("Initialized generic IDE driver. (%x)\n", err);
#endif


	err = names_register("windhoek");
	printk("registered at names. \n", err);

	main_thread = l4_myself();

	printk("+----------------------------------------+\n");
	printk("| Windhoek block server                  |\n");
	printk("| ready to rumble....                    |\n");
	printk("+----------------------------------------+\n");

	windhoek_server_server_loop(NULL);

	l4_sleep_forever();

	return 0;
}
