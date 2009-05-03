# config.make.  Generated from config.make.in by configure.

# Machine architecture.
machine = i686
asm_syntax = i386

# Build options.
build-profiled = 
build-static = ext2fs ufs
boot-store-types = device remap gunzip bunzip2

# Prefix prepended to names of machine-independent installed files.
prefix = 
# Prefix prepended to names of machine-dependent installed files.
exec_prefix = ${prefix}

# Directories where things get installed.
hurddir = ${exec_prefix}/hurd
libdir = ${exec_prefix}/lib
bindir = ${exec_prefix}/bin
sbindir = ${exec_prefix}/sbin
includedir = ${prefix}/include
libexecdir = ${exec_prefix}/libexec
bootdir = ${exec_prefix}/boot
infodir = ${prefix}/share/info
sysconfdir = ${prefix}/etc
localstatedir = ${prefix}/var
sharedstatedir = ${prefix}/com
datadir = ${prefix}/share

# All of those directories together:
installationdirlist = $(hurddir) $(libdir) $(bindir) $(sbindir) \
	$(includedir) $(libexecdir) $(bootdir) $(infodir) $(sysconfdir) \
	$(localstatedir) $(sharedstatedir)


# How to run compilation tools.
CC = gcc
CPP = $(CC) -E -x c # We need this option when input file names are not *.c.
LD = ld
OBJCOPY = objcopy
AR = ar
RANLIB = ranlib
MIG = mig
MIGCOM = $(MIG) -cc cat - /dev/null
AWK = gawk

# Compilation flags.  Append these to the definitions already made by
# the specific Makefile.
CPPFLAGS +=  -DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DHAVE_MIG_RETCODE=1 -DHAVE_GETGROUPLIST=1 -DHAVE_USELOCALE=1
CFLAGS += -g -O2
LDFLAGS += 

gnu89-inline-CFLAGS = -fgnu89-inline

# `yes' or `no' to indicate if ld --version-script is available.
VERSIONING = yes

# If a separate libcrypt is available, use it.
LIBCRYPT = -lcrypt

LIBPCAP = -lpcap

# How to link against Parted libraries, if at all.
PARTED_LIBS = @PARTED_LIBS@

# How to compile and link against ncursesw.
LIBNCURSESW = -lncursesw
NCURSESW_INCLUDE = -I/usr/include/ncursesw

# Installation tools.
INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
