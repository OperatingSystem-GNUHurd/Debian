# -*- Makefile -*-
#
# DROPS (Dresden Realtime OPerating System) Component
#
# Makefile-Template for library directories
#
# $Id: lib.mk 240 2007-11-27 03:55:44Z l4check $
#
# $Author: l4check $
#
# install.inc is used, see there for further documentation
# binary.inc is used, see there for further documentation


ifeq ($(origin _L4DIR_MK_LIB_MK),undefined)
_L4DIR_MK_LIB_MK=y

ROLE = lib.mk

# define INSTALLDIRs prior to including install.inc, where the install-
# rules are defined. Same for INSTALLDIR.
INSTALLDIR_LIB		?= $(DROPS_STDDIR)/lib/$(subst -,/,$(SYSTEM))
INSTALLDIR_LIB_LOCAL	?= $(OBJ_BASE)/lib/$(subst -,/,$(SYSTEM))
INSTALLFILE_LIB		?= $(INSTALL) -m 644 $(1) $(2)
INSTALLFILE_LIB_LOCAL	?= $(LN) -sf $(call absfilename,$(1)) $(2)

INSTALLFILE		= $(INSTALLFILE_LIB)
INSTALLDIR		= $(INSTALLDIR_LIB)
INSTALLFILE_LOCAL	= $(INSTALLFILE_LIB_LOCAL)
INSTALLDIR_LOCAL	= $(INSTALLDIR_LIB_LOCAL)

# our mode
MODE			?= l4env

# include all Makeconf.locals, define common rules/variables
include $(L4DIR)/mk/Makeconf
include $(L4DIR)/mk/binary.inc
$(GENERAL_D_LOC): $(L4DIR)/mk/lib.mk

ifneq ($(SYSTEM),) # if we a system, really build

TARGET_LIB      := $(TARGET) $(TARGET_$(OSYSTEM))
TARGET_SHARED   := $(filter %.s.so,$(TARGET_LIB))
TARGET_STANDARD := $(filter-out %.s.so,$(TARGET_LIB))
TARGET_PROFILE  := $(patsubst %.a,%.pr.a,\
			$(filter $(BUILD_PROFILE),$(TARGET_STANDARD)))
TARGET_PROFILE_SHARED := $(filter %.s.so,$(TARGET_PROFILE))
TARGET_PIC      := $(patsubst %.a,%.p.a,\
			$(filter $(BUILD_PIC),$(TARGET_STANDARD)))
TARGET_PROFILE_PIC := $(patsubst %.a,%.p.a,\
			$(filter $(BUILD_PIC),$(TARGET_PROFILE)))
TARGET	+= $(TARGET_$(OSYSTEM)) $(TARGET_PIC)
TARGET	+= $(TARGET_PROFILE) $(TARGET_PROFILE_SHARED) $(TARGET_PROFILE_PIC)

# define some variables different for lib.mk and prog.mk
LDFLAGS += $(addprefix -L, $(PRIVATE_LIBDIR) $(PRIVATE_LIBDIR_$(OSYSTEM)) $(PRIVATE_LIBDIR_$@) $(PRIVATE_LIBDIR_$@_$(OSYSTEM)))
LDFLAGS += $(addprefix -L, $(L4LIBDIR)) $(LIBCLIBDIR)
LDFLAGS	+= $(LIBS) $(LDFLAGS_$@) $(LDNOSTDLIB)

LDSCRIPT = $(call findfile,main_rel.ld,$(L4LIBDIR))

# install.inc eventually defines rules for every target
#include $(L4DIR)/mk/install.inc

DEPS	+= $(foreach file,$(TARGET), $(dir $(file)).$(notdir $(file)).d)

$(filter-out %.s.so %.o.a %.o.pr.a, $(TARGET)):%.a: $(OBJS)
	@$(AR_MESSAGE)
	$(VERBOSE)$(RM) $@
	$(VERBOSE)$(AR) crs $@ $(OBJS)
	@$(BUILT_MESSAGE)

# shared lib
$(filter %.s.so, $(TARGET)):%.s.so: $(OBJS) $(CRTP) $(LIBDEPS)
	@$(LINK_SHARED_MESSAGE)
	$(VERBOSE)$(call MAKEDEP,$(LD)) $(LD) -m $(LD_EMULATION) \
	   -o $@ -shared $(addprefix -T,$(LDSCRIPT)) $(CRTP) $(OBJS) $(LDFLAGS)
	@$(BUILT_MESSAGE)

# build an object file (which looks like a lib to a later link-call), which
# is either later included as a whole or not at all (important for static
# constructors)
$(filter %.o.a %.o.pr.a, $(TARGET)):%.a: $(OBJS) $(LIBDEPS)
	@$(LINK_PARTIAL_MESSAGE)
	$(VERBOSE)$(call MAKEDEP,$(LD)) $(LD) -m $(LD_EMULATION) \
	   -o $@ -r $(OBJS) $(LDFLAGS)
	@$(BUILT_MESSAGE)

endif	# architecture is defined, really build

.PHONY: all clean cleanall config help install oldconfig txtconfig
-include $(DEPSVAR)
help::
	@echo "  all            - compile and install the libraries locally"
ifneq ($(SYSTEM),)
	@echo "                   to $(INSTALLDIR_LOCAL)"
endif
	@echo "  install        - compile and install the libraries globally"
ifneq ($(SYSTEM),)
	@echo "                   to $(INSTALLDIR)"
endif
	@echo "  scrub          - delete backup and temporary files"
	@echo "  clean          - delete generated object files"
	@echo "  cleanall       - delete all generated, backup and temporary files"
	@echo "  help           - this help"
	@echo
ifneq ($(SYSTEM),)
	@echo "  libraries are: $(TARGET)"
else
	@echo "  build for architectures: $(TARGET_SYSTEMS)"
endif

endif	# _L4DIR_MK_LIB_MK undefined
