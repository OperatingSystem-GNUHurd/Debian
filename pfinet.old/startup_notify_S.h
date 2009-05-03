#ifndef	_startup_notify_server_
#define	_startup_notify_server_

/* Module startup_notify */

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

/* Routine startup_dosync */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_startup_dosync
#if	defined(LINTLIBRARY)
    (notify_port)
	mach_port_t notify_port;
{ return S_startup_dosync(notify_port); }
#else
(
	mach_port_t notify_port
);
#endif

#endif	/* not defined(_startup_notify_server_) */
