PKGDIR        ?= ../libdde_linux26
L4DIR         ?= $(PKGDIR)

SYSTEMS        = x86-l4v2

include Makeconf.local

TARGET         = dde_rtl8139

SRC_C          = main.c 8139cp.c

LIBS          += ../libmachdev/libmachdev.a -ldde_linux26.o -ldde_linux26_net ../libddekit/libddekit.a -lfshelp -ltrivfs -lpciaccess -lthreads -lshouldbeinlibc -lports -lhurd-slab
CFLAGS        += -g -I/include

# DDE configuration
include $(L4DIR)/Makeconf

include $(L4DIR)/mk/prog.mk
