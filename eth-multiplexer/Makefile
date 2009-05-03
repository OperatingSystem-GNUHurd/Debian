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

dir := eth-multiplexer
makemode := server
target = eth-multiplexer

#CFLAGS += -DDEBUG
SRCS = ethernet.c vdev.c multiplexer.c bpf_impl.c queue.c dev_stat.c netfs_impl.c notify_impl.c device_impl.c demuxer.c
MIGSTUBS = ourdeviceServer.o notifyServer.o
OBJS = $(SRCS:.c=.o) $(MIGSTUBS)
LCLHDRS = bpf_impl.h ethernet.h queue.h util.h vdev.h netfs_impl.h
DIST_FILES = ourdevice.defs notify.defs
HURDLIBS=ports fshelp shouldbeinlibc netfs

include ../Makeconf

ourdevice.defs: device.defs
	$(CPP) $(CPPFLAGS) -x c $< | sed -e '/out[ 	]*device[ 	]*:[ 	]*device_t/s/device_t/mach_port_send_t/' > $@

