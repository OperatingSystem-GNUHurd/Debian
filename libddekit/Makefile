#
#   Copyright (C) 2009 Free Software Foundation, Inc.
#
#   This program is free software; you can redistribute it and/or
#   modify it under the terms of the GNU General Public License as
#   published by the Free Software Foundation; either version 2, or (at
#   your option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

dir := libddekit
makemode := library

libname = libddekit
SRCS= condvar.c init.c interrupt.c lock.c malloc.c memory.c 	\
      pci.c pgtab.c printf.c resources.c list.c panic.c \
      thread.c timer.c initcall.c
installhdrs = ddekit/condvar.h ddekit/lock.h	\
	  ddekit/semaphore.h ddekit/debug.h	\
	  ddekit/inline.h ddekit/panic.h	\
	  ddekit/thread.h ddekit/types.h	\
	  ddekit/pgtab.h ddekit/printf.h	\
	  ddekit/pci.h ddekit/assert.h		\
	  ddekit/interrupt.h ddekit/resources.h	\
	  ddekit/memory.h ddekit/timer.h	\
	  ddekit/initcall.h

installhdrsubdir = .
LCLHDRS = $(installhdrs)	\
	  dde.h		\
	  config.h list.h util.h

MIGSTUBS = deviceUser.o machUser.o
OBJS = $(sort $(SRCS:.c=.o) $(MIGSTUBS))

HURDLIBS = threads ports shouldbeinlibc hurd-slab

MIGCOMSFLAGS = -prefix dde_

include ../Makeconf

LIBS += -lpciaccess

$(libname).so.$(hurd-version):
	echo "INPUT ( $(libname).a )" > $@
