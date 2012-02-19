# -*- Makefile -*-
#
# DROPS (Dresden Realtime OPerating System) Component
#
# Makefile-Template for binary directories
#
# $Id: prog.mk 397 2008-08-22 02:50:28Z l4check $
#
# $Author: l4check $
#
# Makeconf is used, see there for further documentation
# install.inc is used, see there for further documentation
# binary.inc is used, see there for further documentation

ifeq ($(origin _L4DIR_MK_PROG_MK),undefined)
_L4DIR_MK_PROG_MK=y

ROLE = prog.mk

include $(L4DIR)/mk/Makeconf
$(GENERAL_D_LOC): $(L4DIR)/mk/prog.mk

# define INSTALLDIRs prior to including install.inc, where the install-
# rules are defined.
ifeq ($(MODE),host)
INSTALLDIR_BIN		?= $(DROPS_STDDIR)/bin/host
INSTALLDIR_BIN_LOCAL	?= $(OBJ_BASE)/bin/host
else
INSTALLDIR_BIN		?= $(DROPS_STDDIR)/bin/$(subst -,/,$(SYSTEM))
INSTALLDIR_BIN_LOCAL	?= $(OBJ_BASE)/bin/$(subst -,/,$(SYSTEM))
endif
ifeq ($(BID_STRIP_PROGS),y)
INSTALLFILE_BIN 	?= $(STRIP) --strip-unneeded $(1) -o $(2) && \
			   chmod 755 $(2)
INSTALLFILE_BIN_LOCAL 	?= $(STRIP) --strip-unneeded $(1) -o $(2) && \
			   chmod 755 $(2)
else
INSTALLFILE_BIN 	?= $(INSTALL) -m 755 $(1) $(2)
INSTALLFILE_BIN_LOCAL 	?= $(INSTALL) -m 755 $(1) $(2)
endif

INSTALLFILE		= $(INSTALLFILE_BIN)
INSTALLDIR		= $(INSTALLDIR_BIN)
INSTALLFILE_LOCAL	= $(INSTALLFILE_BIN_LOCAL)
INSTALLDIR_LOCAL	= $(INSTALLDIR_BIN_LOCAL)

# our mode
MODE 			?= l4env

# include all Makeconf.locals, define common rules/variables
include $(L4DIR)/mk/binary.inc

ifneq ($(SYSTEM),) # if we have a system, really build

TARGET_STANDARD := $(TARGET) $(TARGET_$(OSYSTEM))
TARGET_PROFILE := $(addsuffix .pr,$(filter $(BUILD_PROFILE),$(TARGET)))
TARGET	+= $(TARGET_$(OSYSTEM)) $(TARGET_PROFILE)

CFLAGS += $(DDEKITINCDIR)
# define some variables different for lib.mk and prog.mk
ifeq ($(MODE),loader)
LDFLAGS += --dynamic-linker libld-l4.s.so
endif
ifeq ($(BID_GENERATE_MAPFILE),y)
LDFLAGS += -Map $(strip $@).map
endif
LDFLAGS += $(addprefix -L, $(PRIVATE_LIBDIR) $(PRIVATE_LIBDIR_$(OSYSTEM)) $(PRIVATE_LIBDIR_$@) $(PRIVATE_LIBDIR_$@_$(OSYSTEM)))
LDFLAGS += $(addprefix -L, $(L4LIBDIR)) $(LIBCLIBDIR)
LDFLAGS += -L$(DDE26LIBDIR)
LDFLAGS += -L$(DDEKITLIBDIR)
LDFLAGS	+= $(LIBS) #$(addprefix -T, $(LDSCRIPT)) $(L4LIBS) $(LIBCLIBS) $(LDFLAGS_$@)
# Not all host linkers understand this option
ifneq ($(HOST_LINK),1)
LDFLAGS += -Wl,--warn-common
endif

#ifeq ($(notdir $(LDSCRIPT)),main_stat.ld)
# ld denies -gc-section when linking against shared libraries
#ifeq ($(findstring FOO,$(patsubst -l%.s,FOO,$(LIBS) $(L4LIBS) $(LIBCLIBS))),)
LDFLAGS += -Wl,-gc-sections
#endif
#endif
LDFLAGS += -Wl,-Tdefault.ld

#include $(L4DIR)/mk/install.inc

#VPATHEX = $(foreach obj, $(OBJS), $(firstword $(foreach dir, \
#          . $(VPATH),$(wildcard $(dir)/$(obj)))))

# target-rule:

# Make looks for obj-files using VPATH only when looking for dependencies
# and applying implicit rules. Though make adapts its automatic variables,
# we cannot use them: The dependencies contain files which have not to be
# linked to the binary. Therefore the foreach searches the obj-files like
# make does, using the VPATH variable.
# Use a surrounding strip call to avoid ugly linebreaks in the commands
# output.

# Dependencies: When we have ld.so, we use MAKEDEP to build our
# library dependencies. If not, we fall back to LIBDEPS, an
# approximation of the correct dependencies for the binary. Note, that
# MAKEDEP will be empty if we dont have ld.so, LIBDEPS will be empty
# if we have ld.so.

ifeq ($(HAVE_LDSO),)
LIBDEPS = $(foreach file, \
                    $(patsubst -l%,lib%.a,$(filter-out -L%,$(LDFLAGS))) \
                    $(patsubst -l%,lib%.so,$(filter-out -L%,$(LDFLAGS))),\
                    $(word 1, $(foreach dir, \
                           $(patsubst -L%,%,\
                           $(filter -L%,$(LDFLAGS) $(L4ALL_LIBDIR))),\
                      $(wildcard $(dir)/$(file)))))
endif

DEPS	+= $(foreach file,$(TARGET), $(dir $(file)).$(notdir $(file)).d)

LINK_PROGRAM-C-host-1   := $(CC)
LINK_PROGRAM-CXX-host-1 := $(CXX)

LINK_PROGRAM  := $(LINK_PROGRAM-C-host-$(HOST_LINK))
ifneq ($(SRC_CC),)
LINK_PROGRAM  := $(LINK_PROGRAM-CXX-host-$(HOST_LINK))
endif

ifeq ($(LINK_PROGRAM),)
#LINK_PROGRAM  := $(LD) -m $(LD_EMULATION)
LINK_PROGRAM  := gcc
endif

$(TARGET): $(OBJS) $(LIBDEPS) 
	@$(LINK_MESSAGE)
	$(VERBOSE)$(call MAKEDEP,$(INT_LD_NAME),,,ld) $(LINK_PROGRAM) -o $@ $(OBJS) $(LDFLAGS) $(CRTN)
	@$(BUILT_MESSAGE)
#	$(VERBOSE)$(call MAKEDEP,$(INT_LD_NAME),,,ld) $(LINK_PROGRAM) -o $@ $(CRT0) $(OBJS) $(LDFLAGS) $(CRTN)

endif	# architecture is defined, really build

-include $(DEPSVAR)
.PHONY: all clean cleanall config help install oldconfig txtconfig
help::
	@echo "  all            - compile and install the binaries"
ifneq ($(SYSTEM),)
	@echo "                   to $(INSTALLDIR_LOCAL)"
endif
	@echo "  install        - compile and install the binaries"
ifneq ($(SYSTEM),)
	@echo "                   to $(INSTALLDIR)"
endif
	@echo "  relink         - relink and install the binaries"
ifneq ($(SYSTEM),)
	@echo "                   to $(INSTALLDIR_LOCAL)"
endif
	@echo "  disasm         - disassemble first target"
	@echo "  scrub          - delete backup and temporary files"
	@echo "  clean          - delete generated object files"
	@echo "  cleanall       - delete all generated, backup and temporary files"
	@echo "  help           - this help"
	@echo
ifneq ($(SYSTEM),)
	@echo "  binaries are: $(TARGET)"
else
	@echo "  build for architectures: $(TARGET_SYSTEMS)"
endif

endif	# _L4DIR_MK_PROG_MK undefined
