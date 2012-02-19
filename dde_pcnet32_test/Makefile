PKGDIR        ?= ../libdde_linux26
L4DIR         ?= $(PKGDIR)

SYSTEMS        = x86-l4v2

include Makeconf.local

TARGET         = dde_pcnet32

SRC_C          = main.c arping.c pcnet32.c

LIBS          += -ldde_linux26.o -ldde_linux26_block -ldde_linux26_char -ldde_linux26_net /root/hurd/libddekit/libddekit.a -lpciaccess -lthreads -lshouldbeinlibc
CFLAGS        += -g

# DDE configuration
include $(L4DIR)/Makeconf

include $(L4DIR)/mk/prog.mk
