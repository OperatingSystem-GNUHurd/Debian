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
SRCS= condvar.c init.c initcall.c interrupt.c lock.c malloc.c memory.c 	\
      pci.c pgtab-old.c pgtab.c printf.c resources.c list.c \
      thread.c timer.c kmem.c
LCLHDRS = include/ddekit/condvar.h include/ddekit/lock.h	\
	  include/ddekit/initcall.h include/ddekit/debug.h	\
	  include/ddekit/inline.h include/ddekit/panic.h	\
	  include/ddekit/thread.h include/ddekit/types.h	\
	  include/ddekit/pgtab.h include/ddekit/printf.h	\
	  include/ddekit/pci.h include/ddekit/assert.h		\
	  include/ddekit/interrupt.h include/ddekit/resources.h	\
	  include/ddekit/memory.h include/ddekit/timer.h	\
	  include/ddekit/semaphore.h include/dde.h		\
	  config.h list.h
installhdrs = 

MIGSTUBS = 
OBJS = $(sort $(SRCS:.c=.o) $(MIGSTUBS))

HURDLIBS = threads ports

MIGCOMSFLAGS = -prefix dde_

include ../Makeconf

LDFLAGS += -lpciaccess
