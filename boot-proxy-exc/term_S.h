#ifndef	_term_server_
#define	_term_server_

/* Module term */

#include <mach/kern_return.h>
#include <mach/port.h>
#include <mach/message.h>

#include <mach/std_types.h>
#include <mach/mach_types.h>
#include <device/device_types.h>
#include <device/net_status.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <hurd/hurd_types.h>

/* Routine term_getctty */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_term_getctty
#if	defined(LINTLIBRARY)
    (terminal, ctty, cttyPoly)
	io_t terminal;
	mach_port_t *ctty;
	mach_msg_type_name_t *cttyPoly;
{ return S_term_getctty(terminal, ctty, cttyPoly); }
#else
(
	io_t terminal,
	mach_port_t *ctty,
	mach_msg_type_name_t *cttyPoly
);
#endif

/* Routine term_open_ctty */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_term_open_ctty
#if	defined(LINTLIBRARY)
    (terminal, pid, pgrp, newtty, newttyPoly)
	io_t terminal;
	pid_t pid;
	pid_t pgrp;
	mach_port_t *newtty;
	mach_msg_type_name_t *newttyPoly;
{ return S_term_open_ctty(terminal, pid, pgrp, newtty, newttyPoly); }
#else
(
	io_t terminal,
	pid_t pid,
	pid_t pgrp,
	mach_port_t *newtty,
	mach_msg_type_name_t *newttyPoly
);
#endif

/* Routine term_set_nodename */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_term_set_nodename
#if	defined(LINTLIBRARY)
    (terminal, name)
	io_t terminal;
	string_t name;
{ return S_term_set_nodename(terminal, name); }
#else
(
	io_t terminal,
	string_t name
);
#endif

/* Routine term_get_nodename */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_term_get_nodename
#if	defined(LINTLIBRARY)
    (terminal, name)
	io_t terminal;
	string_t name;
{ return S_term_get_nodename(terminal, name); }
#else
(
	io_t terminal,
	string_t name
);
#endif

/* Routine term_set_filenode */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_term_set_filenode
#if	defined(LINTLIBRARY)
    (terminal, filenode)
	io_t terminal;
	file_t filenode;
{ return S_term_set_filenode(terminal, filenode); }
#else
(
	io_t terminal,
	file_t filenode
);
#endif

/* Routine term_get_bottom_type */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_term_get_bottom_type
#if	defined(LINTLIBRARY)
    (terminal, ttype)
	io_t terminal;
	int *ttype;
{ return S_term_get_bottom_type(terminal, ttype); }
#else
(
	io_t terminal,
	int *ttype
);
#endif

/* Routine term_on_machdev */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_term_on_machdev
#if	defined(LINTLIBRARY)
    (terminal, machdev)
	io_t terminal;
	mach_port_t machdev;
{ return S_term_on_machdev(terminal, machdev); }
#else
(
	io_t terminal,
	mach_port_t machdev
);
#endif

/* Routine term_on_hurddev */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_term_on_hurddev
#if	defined(LINTLIBRARY)
    (terminal, hurddev)
	io_t terminal;
	io_t hurddev;
{ return S_term_on_hurddev(terminal, hurddev); }
#else
(
	io_t terminal,
	io_t hurddev
);
#endif

/* Routine term_on_pty */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_term_on_pty
#if	defined(LINTLIBRARY)
    (terminal, ptymaster)
	io_t terminal;
	io_t *ptymaster;
{ return S_term_on_pty(terminal, ptymaster); }
#else
(
	io_t terminal,
	io_t *ptymaster
);
#endif

/* Routine termctty_open_terminal */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_termctty_open_terminal
#if	defined(LINTLIBRARY)
    (ctty, flags, terminal, terminalPoly)
	mach_port_t ctty;
	int flags;
	mach_port_t *terminal;
	mach_msg_type_name_t *terminalPoly;
{ return S_termctty_open_terminal(ctty, flags, terminal, terminalPoly); }
#else
(
	mach_port_t ctty,
	int flags,
	mach_port_t *terminal,
	mach_msg_type_name_t *terminalPoly
);
#endif

/* Routine term_get_peername */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_term_get_peername
#if	defined(LINTLIBRARY)
    (terminal, name)
	io_t terminal;
	string_t name;
{ return S_term_get_peername(terminal, name); }
#else
(
	io_t terminal,
	string_t name
);
#endif

#endif	/* not defined(_term_server_) */
