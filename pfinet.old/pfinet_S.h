#ifndef	_pfinet_server_
#define	_pfinet_server_

/* Module pfinet */

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

/* Routine pfinet_siocgifconf */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_pfinet_siocgifconf
#if	defined(LINTLIBRARY)
    (port, amount, buf, bufCnt)
	io_t port;
	vm_size_t amount;
	data_t *buf;
	mach_msg_type_number_t *bufCnt;
{ return S_pfinet_siocgifconf(port, amount, buf, bufCnt); }
#else
(
	io_t port,
	vm_size_t amount,
	data_t *buf,
	mach_msg_type_number_t *bufCnt
);
#endif

#endif	/* not defined(_pfinet_server_) */
