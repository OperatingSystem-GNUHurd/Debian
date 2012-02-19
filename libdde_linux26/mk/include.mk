# -*- Makefile -*-
#
# DROPS (Dresden Realtime OPerating System) Component
#
# Makefile-Template for include directories
#
# $Id: include.mk 240 2007-11-27 03:55:44Z l4check $
#
# $Author: l4check $

#
# supported targets:
#
#   all				- the default, link the includes into the
#				  local include dir
#   install			- install the includes into the global
#				  include dir
#   config			- do nothing, may be overwritten
#   relink			- reinstall locally


INSTALLDIR_INC		?= $(DROPS_STDDIR)/include
INSTALLDIR_INC_LOCAL	?= $(OBJ_BASE)/include

INSTALLDIR		= $(INSTALLDIR_INC)
INSTALLDIR_LOCAL	= $(INSTALLDIR_INC_LOCAL)

ifeq ($(origin TARGET),undefined)
# use POSIX -print here
TARGET_CMD		= (cd $(INCSRC_DIR); find . -name '*.[ih]' -print)
else
TARGET_CMD		= echo $(TARGET)
endif
INSTALL_INC_PREFIX	?= l4/$(PKGNAME)
INCSRC_DIR		?= $(SRC_DIR)

include $(L4DIR)/mk/Makeconf
$(GENERAL_D_LOC): $(L4DIR)/mk/include.mk
-include $(DEPSVAR)

do_link = if (readlink($$dst) ne $$src) {                                     \
            if ($$notify eq 1) {                                              \
              $$notify=0; $(if $(VERBOSE),print "  ... Updating symlinks\n";,)\
            }                                                                 \
            system("ln","-sf$(if $(VERBOSE),,v)",$$src,$$dst) && exit 1;      \
          }
do_inst = system("install","-$(if $(VERBOSE),,v)m","644",$$src,$$dst) && exit 1;
installscript = perl -e '                                                     \
  chomp($$srcdir="$(INCSRC_DIR)");                                            \
  $$notify=1;                                                                 \
  while(<>) {                                                                 \
    @_ = split; while(@_) {                                                        \
      $$_=shift @_; s|^\./||; $$src=$$_;                                      \
      if(s|^ARCH-([^/]*)/L4API-([^/]*)/([^ ]*)$$|\1/\2/$(INSTALL_INC_PREFIX)/\3| ||\
	 s|^ARCH-([^/]*)/([^ ]*)$$|\1/$(INSTALL_INC_PREFIX)/\2| ||            \
	 s|^L4API-([^/]*)/([^ ]*)$$|\1/$(INSTALL_INC_PREFIX)/\2| ||           \
	 s|^([^ ]*)$$|$(INSTALL_INC_PREFIX)/\1|) {                            \
	    $$src="$$srcdir/$$src" if $$src !~ /^\//;                         \
	    $$dstdir=$$dst="$(if $(1),$(INSTALLDIR_LOCAL),$(INSTALLDIR))/$$_";\
	    $$dstdir=~s|/[^/]*$$||;                                           \
	    -d $$dstdir || system("install","-$(if $(VERBOSE),,v)d",$$dstdir) && exit 1;        \
	    $(if $(1),$(do_link),$(do_inst))                                  \
	  }                                                                   \
    }                                                                         \
  }'

headers::

all:: headers
	@$(TARGET_CMD) | $(call installscript,1)

install::
	@$(INSTALL_LINK_MESSAGE)
	@$(TARGET_CMD) | $(call installscript);

cleanall::
	$(VERBOSE)$(RM) .general.d

help::
	@echo "  all            - install files to $(INSTALLDIR_LOCAL)"
	@echo "  install        - install files to $(INSTALLDIR)"
	@echo "  scrub          - delete backup and temporary files"
	@echo "  clean          - same as scrub"
	@echo "  cleanall       - same as scrub"
	@echo "  help           - this help"

scrub clean cleanall::
	$(VERBOSE)$(SCRUB)
