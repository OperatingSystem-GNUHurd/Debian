PKGDIR        ?= ../libdde_linux26
L4DIR         ?= $(PKGDIR)

SYSTEMS        = x86-l4v2

DEFAULT_RELOC  = 0x00a00000

include Makeconf.local
#-include $(PKGDIR_OBJ)/../dde/Makeconf

TARGET         = windhoek

IDE_SRC = ide.c ide-disk.c ide-generic.c ide-io.c ide-park.c \
		  ide-iops.c ide-lib.c ide-pm.c ide-pnp.c ide-probe.c ide-taskfile.c  \
		  setup-pci.c ide-pci-generic.c ide-pio-blacklist.c ide-sysfs.c \
		  piix.c ide-dma.c ide-gd.c
IDEFILES = $(addprefix ide/, $(IDE_SRC))

CDROM_SRC = #cdrom.c
CDROMFILES = $(addprefix cdrom/, $(CDROM_SRC))

PARTITION_SRC = msdos.c check.c
PARTITIONFILES = $(addprefix partitions/, $(PARTITION_SRC))

SRC_C          = main.c genhd.c components.c
SRC_C          += $(IDEFILES) \
				 $(CDROMFILES) \
				 $(PARTITIONFILES)

LIBS          += --whole-archive -ldde_linux26_block --no-whole-archive -ldde_linux26.o \
				 -lddekit

# DDE configuration
include $(L4DIR)/Makeconf

include $(L4DIR)/mk/prog.mk
