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

dir := proc_proxy
makemode := server

SRCS = proc_proxy.c process_ops.c
LCLHDRS = proc_proxy.h
DIST_FILES = process.defs
HURDLIBS = threads ports
target = proc_proxy
MIGSTUBS = processServer.o
OBJS = $(SRCS:.c=.o) $(MIGSTUBS)

MIGSFLAGS="-DPROCESS_INTRAN=vpstruct_t reqport_find (process_t)" \
	"-DPROCESS_DESTRUCTOR=process_drop (vpstruct_t)" \
	"-DPROCESS_IMPORTS=import \"proc_proxy.h\";"

include ../Makeconf

my_processUser.c: processUser.c
	cat $< | sed -e '/__mig_get_reply_port *( *)/s/__mig_get_reply_port/my_get_reply_port/' > $@;
