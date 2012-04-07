PKGDIR        ?= ../libdde_linux26
L4DIR         ?= $(PKGDIR)

include Makeconf.local

TARGET         = dde_pcnet32

SRC_C          = main.c pcnet32.c

LIBS          += $(libmachdev_path) -ldde_linux26.o -ldde_linux26_net $(libddekit_path) -lfshelp -ltrivfs -lpciaccess -lthreads -lshouldbeinlibc -lports $(libslab_path) $(libbpf_path)
CFLAGS        += -g -I$(PKGDIR)/include -I$(BUILDDIR)/include

# DDE configuration
include $(L4DIR)/Makeconf

include $(L4DIR)/mk/prog.mk
