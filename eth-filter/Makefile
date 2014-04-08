# Copyright (C) 1992, 1993, 1994, 1995, 1996, 2008 Free Software Foundation, Inc.
# This file is part of the GNU Hurd.
#
# The GNU Hurd is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# The GNU Hurd is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with the GNU Hurd; see the file COPYING.  If not, write to
# the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

dir := eth-filter
makemode := server

SRCS = bpf_impl.c filter.c queue.c pcap_filter.c
LCLHDRS = bpf_impl.h queue.h util.h
HURDLIBS = ports trivfs fshelp ihash shouldbeinlibc
target = eth-filter
MIGSTUBS = deviceServer.o notifyServer.o
MIGSFLAGS = -imacros $(srcdir)/mig-mutate.h
OBJS = $(SRCS:.c=.o) $(MIGSTUBS)

include ../Makeconf

#CFLAGS += -I../pfinet/linux-src/include -I../pfinet/glue-include
LDFLAGS += -lpcap
