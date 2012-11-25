PKGDIR        ?= ../libdde_linux26
L4DIR         ?= $(PKGDIR)

SYSTEMS        = x86-l4v2

include Makeconf.local

TARGET         = dde_rtl8139

SRC_C          = main.c 8139cp.c

LIBS          += $(libmachdev_path) -ldde_linux26.o -ldde_linux26_net $(libddekit_path) -lfshelp -ltrivfs -lpciaccess -lz -lpthread -lshouldbeinlibc -lports $(libslab_path) $(libbpf_path)
CFLAGS        += -g -I$(PKGDIR)/include -I$(BUILDDIR)/include

# DDE configuration
include $(L4DIR)/Makeconf

include $(L4DIR)/mk/prog.mk
