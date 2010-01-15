#include "local.h"

#include <dde26.h>
#include <dde.h>

#define DEBUG_PCI(msg, ...)	ddekit_printf( "\033[33m"msg"\033[0m\n", ##__VA_ARGS__)

/* Didn't know where to put this. */
unsigned long __per_cpu_offset[NR_CPUS];

extern void driver_init(void);
extern int classes_init(void);

void __init __attribute__((used)) l4dde26_init(void)
{
	/* first, initialize DDEKit */
	ddekit_init();

	l4dde26_kmalloc_init();

	/* Init Linux driver framework before trying to add PCI devs to the bus */
	driver_init();

	printk("Initialized DDELinux 2.6\n");
}

void l4dde26_do_initcalls(void)
{
	extern void dde_page_cache_init (void);
	extern void pci_iommu_init (void);
	extern void init_workqueues (void);
	extern void pci_init (void);
	extern void pci_driver_init (void);
	extern void pcibus_class_init (void);
	extern void net_dev_init (void);
	extern void neigh_init (void);
	extern void net_ns_init (void);
	extern void blk_ioc_init (void);
	extern void blk_settings_init (void);
	extern void blk_softirq_init (void);
	extern void genhd_device_init (void);
	extern void noop_init (void);
	extern void bdi_class_init (void);
	extern void init_bio (void);
	extern void chrdev_init (void);
	/* finally, let DDEKit perform all the initcalls */
//	ddekit_do_initcalls();
	/* 1000: pure_, core_ */
	l4dde26_init ();
	l4dde26_process_init ();
	l4dde26_init_timers ();
	net_ns_init ();
	dde_page_cache_init ();
	init_workqueues ();
	chrdev_init ();
	/* 1001: postcore_ */
	pci_driver_init ();
	pcibus_class_init ();
	bdi_class_init ();
	/* 1002: arch_ */
	l4dde26_init_pci ();
	/* 1003: subsys_ */
	net_dev_init ();
//	neigh_init ();
	blk_ioc_init ();
	blk_settings_init ();
	blk_softirq_init ();
//	genhd_device_init ();
	noop_init ();
	init_bio ();
	/* 1004: fs_ */
	pci_iommu_init ();
	/* 1005: device_ */
	pci_init ();
	/* 1006: late_ */
}

//dde_initcall(l4dde26_init);
