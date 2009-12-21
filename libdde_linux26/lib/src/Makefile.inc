# automatically generated, do not edit!

IDL_SRC_C_libdde_linux26.o.a= 
IDL_SRC_C_libdde_linux26_net.a= 
IDL_SRC_C_libdde_linux26_block.a= 
IDL_SRC_C_libdde_linux26_sound.a= 
IDL_SRC_C_libdde_linux26_char.a=
SRC_C_libdde_linux26.o.a += $(IDL_SRC_C_libdde_linux26.o.a) 
SRC_C_libdde_linux26_net.a += $(IDL_SRC_C_libdde_linux26_net.a) 
SRC_C_libdde_linux26_block.a += $(IDL_SRC_C_libdde_linux26_block.a) 
SRC_C_libdde_linux26_sound.a += $(IDL_SRC_C_libdde_linux26_sound.a) 
SRC_C_libdde_linux26_char.a += $(IDL_SRC_C_libdde_linux26_char.a)
Makefile.inc: IDL_SRC_C_libdde_linux26.o.a= 
Makefile.inc: IDL_SRC_C_libdde_linux26_net.a= 
Makefile.inc: IDL_SRC_C_libdde_linux26_block.a= 
Makefile.inc: IDL_SRC_C_libdde_linux26_sound.a= 
Makefile.inc: IDL_SRC_C_libdde_linux26_char.a=
IDL_SRC_CC_libdde_linux26.o.a= 
IDL_SRC_CC_libdde_linux26_net.a= 
IDL_SRC_CC_libdde_linux26_block.a= 
IDL_SRC_CC_libdde_linux26_sound.a= 
IDL_SRC_CC_libdde_linux26_char.a=
SRC_CC_libdde_linux26.o.a += $(IDL_SRC_CC_libdde_linux26.o.a) 
SRC_CC_libdde_linux26_net.a += $(IDL_SRC_CC_libdde_linux26_net.a) 
SRC_CC_libdde_linux26_block.a += $(IDL_SRC_CC_libdde_linux26_block.a) 
SRC_CC_libdde_linux26_sound.a += $(IDL_SRC_CC_libdde_linux26_sound.a) 
SRC_CC_libdde_linux26_char.a += $(IDL_SRC_CC_libdde_linux26_char.a)
Makefile.inc: IDL_SRC_CC_libdde_linux26.o.a= 
Makefile.inc: IDL_SRC_CC_libdde_linux26_net.a= 
Makefile.inc: IDL_SRC_CC_libdde_linux26_block.a= 
Makefile.inc: IDL_SRC_CC_libdde_linux26_sound.a= 
Makefile.inc: IDL_SRC_CC_libdde_linux26_char.a=
OBJS_libdde_linux26.o.a += arch/l4/cli_sti.o arch/l4/fs.o arch/l4/hw-helpers.o arch/l4/init_task.o arch/l4/init.o arch/l4/pci.o arch/l4/power.o arch/l4/process.o arch/l4/res.o arch/l4/sched.o arch/l4/signal.o arch/l4/smp.o arch/l4/softirq.o arch/l4/timer.o arch/l4/page_alloc.o arch/l4/kmem_cache.o arch/l4/kmalloc.o arch/l4/irq.o arch/l4/param.o arch/l4/vmalloc.o arch/l4/vmstat.o arch/l4/mm-helper.o lib/rwsem.o arch/x86/kernel/pci-dma.o arch/x86/kernel/pci-nommu.o kernel/exit.o kernel/kthread.o kernel/mutex.o kernel/notifier.o kernel/resource.o kernel/rwsem.o kernel/sched.o kernel/semaphore.o kernel/sys.o kernel/time.o kernel/timer.o kernel/wait.o kernel/workqueue.o lib/bitmap.o lib/bitrev.o lib/crc32.o lib/ctype.o lib/cpumask.o lib/find_next_bit.o lib/hexdump.o lib/idr.o lib/iomap.o lib/hweight.o lib/kasprintf.o lib/kernel_lock.o lib/klist.o lib/kobject.o lib/kref.o lib/parser.o lib/proportions.o lib/radix-tree.o lib/scatterlist.o lib/sha1.o lib/string.o lib/vsprintf.o mm/dmapool.o mm/mempool.o mm/swap.o mm/util.o drivers/base/attribute_container.o drivers/base/bus.o drivers/base/class.o drivers/base/core.o drivers/base/cpu.o drivers/base/dd.o drivers/base/devres.o drivers/base/driver.o drivers/base/init.o drivers/base/map.o drivers/base/platform.o drivers/base/sys.o drivers/pci/access.o drivers/pci/bus.o drivers/pci/hotplug-pci.o drivers/pci/pci.o drivers/pci/pci-driver.o drivers/pci/probe.o drivers/pci/search.o drivers/pci/setup-bus.o drivers/pci/setup-res.o arch/x86/lib/semaphore_32.o 
OBJS_libdde_linux26_net.a += arch/l4/net.o drivers/net/mii.o net/core/dev.o net/core/dev_mcast.o net/core/ethtool.o net/core/link_watch.o net/core/neighbour.o net/core/netevent.o net/core/net-sysfs.o net/core/net_namespace.o net/core/rtnetlink.o net/core/skbuff.o net/core/skb_dma_map.o net/core/utils.o net/ethernet/eth.o net/sched/sch_generic.o arch/x86/lib/checksum_32.o 
OBJS_libdde_linux26_block.a += arch/l4/inodes.o block/blk-barrier.o block/blk-core.o block/blk-exec.o block/blk-ioc.o block/blk-merge.o block/blk-settings.o block/blk-softirq.o block/blk-sysfs.o block/blk-tag.o block/blk-timeout.o block/elevator.o block/genhd.o block/noop-iosched.o block/ioctl.o block/scsi_ioctl.o mm/backing-dev.o mm/bounce.o mm/page-writeback.o fs/bio.o fs/block_dev.o fs/buffer.o fs/filesystems.o 
OBJS_libdde_linux26_sound.a += sound/sound_core.o arch/l4/sound.o 
OBJS_libdde_linux26_char.a += arch/l4/inodes.o fs/char_dev.o
                                                                                                                                

libdde_linux26.o.a: $(OBJS_libdde_linux26.o.a) 
libdde_linux26_net.a: $(OBJS_libdde_linux26_net.a) 
libdde_linux26_block.a: $(OBJS_libdde_linux26_block.a) 
libdde_linux26_sound.a: $(OBJS_libdde_linux26_sound.a) 
libdde_linux26_char.a: $(OBJS_libdde_linux26_char.a)

$(OBJS_libdde_linux26.o.a): .general.d 
$(OBJS_libdde_linux26_net.a): .general.d 
$(OBJS_libdde_linux26_block.a): .general.d 
$(OBJS_libdde_linux26_sound.a): .general.d 
$(OBJS_libdde_linux26_char.a): .general.d
