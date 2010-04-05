PKGDIR        ?= ../libdde_linux26
L4DIR         ?= $(PKGDIR)

SYSTEMS        = x86-l4v2

include Makeconf.local

TARGET         = dde_e1000

SRC_C          = main.c e1000_ethtool.c e1000_hw.c e1000_main.c e1000_param.c

LIBS          += ../libmachdev/libmachdev.a -ldde_linux26.o -ldde_linux26_net ../libddekit/libddekit.a -lfshelp -ltrivfs -lpciaccess -lthreads -lshouldbeinlibc -lports
CFLAGS        += -g

# DDE configuration
include $(L4DIR)/Makeconf

include $(L4DIR)/mk/prog.mk
