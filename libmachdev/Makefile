# Copyright (C) 2009 Free Software Foundation, Inc.
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

dir := libmachdev
makemode := library
libname = libmachdev

SRCS = deviceUser.c machUser.c net.c ds_routines.c queue.c trivfs_server.c \
       device_replyUser.c deviceServer.c notifyServer.c misc.c
LCLHDRS = dev_hdr.h device_emul.h ds_routines.h vm_param.h \
	  util.h queue.h io_req.h if_ether.h machdev.h linux-errno.h \
	  errno-base.h
installhdrs = machdev.h device_emul.h
HURDLIBS = ports threads trivfs
OBJS = $(SRCS:.c=.o) $(MIGSTUBS)

include ../Makeconf

CFLAGS += -I../libddekit/include
