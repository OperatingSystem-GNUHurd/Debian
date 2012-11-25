PKGDIR        ?= ../libdde_linux26
L4DIR         ?= $(PKGDIR)

include Makeconf.local

TARGET         = dde_e100

SRC_C          = main.c e100.c

LIBS          += $(libmachdev_path) -ldde_linux26.o -ldde_linux26_net $(libddekit_path) -lfshelp -ltrivfs -lpciaccess -lpthread -lshouldbeinlibc -lports $(libslab_path) $(libbpf_path)
CFLAGS        += -g -I$(PKGDIR)/include -I$(BUILDDIR)/include

# DDE configuration
include $(L4DIR)/Makeconf

include $(L4DIR)/mk/prog.mk
