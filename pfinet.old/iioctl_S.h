#ifndef	_iioctl_server_
#define	_iioctl_server_

/* Module iioctl */

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
#include <hurd/ioctl_types.h>

/* Routine iioctl_siocsifaddr */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_iioctl_siocsifaddr
#if	defined(LINTLIBRARY)
    (reqport, ifnam, addr)
	io_t reqport;
	ifname_t ifnam;
	sockaddr_t addr;
{ return S_iioctl_siocsifaddr(reqport, ifnam, addr); }
#else
(
	io_t reqport,
	ifname_t ifnam,
	sockaddr_t addr
);
#endif

/* Routine iioctl_siocsifdstaddr */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_iioctl_siocsifdstaddr
#if	defined(LINTLIBRARY)
    (reqport, ifnam, dstaddr)
	io_t reqport;
	ifname_t ifnam;
	sockaddr_t dstaddr;
{ return S_iioctl_siocsifdstaddr(reqport, ifnam, dstaddr); }
#else
(
	io_t reqport,
	ifname_t ifnam,
	sockaddr_t dstaddr
);
#endif

/* Routine iioctl_siocsifflags */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_iioctl_siocsifflags
#if	defined(LINTLIBRARY)
    (reqport, ifnam, flags)
	io_t reqport;
	ifname_t ifnam;
	short flags;
{ return S_iioctl_siocsifflags(reqport, ifnam, flags); }
#else
(
	io_t reqport,
	ifname_t ifnam,
	short flags
);
#endif

/* Routine iioctl_siocgifflags */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_iioctl_siocgifflags
#if	defined(LINTLIBRARY)
    (reqport, ifnam, flags)
	io_t reqport;
	ifname_t ifnam;
	short *flags;
{ return S_iioctl_siocgifflags(reqport, ifnam, flags); }
#else
(
	io_t reqport,
	ifname_t ifnam,
	short *flags
);
#endif

/* Routine iioctl_siocsifbrdaddr */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_iioctl_siocsifbrdaddr
#if	defined(LINTLIBRARY)
    (reqport, ifnam, brdaddr)
	io_t reqport;
	ifname_t ifnam;
	sockaddr_t brdaddr;
{ return S_iioctl_siocsifbrdaddr(reqport, ifnam, brdaddr); }
#else
(
	io_t reqport,
	ifname_t ifnam,
	sockaddr_t brdaddr
);
#endif

/* Routine iioctl_siocsifnetmask */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_iioctl_siocsifnetmask
#if	defined(LINTLIBRARY)
    (reqport, ifnam, netmask)
	io_t reqport;
	ifname_t ifnam;
	sockaddr_t netmask;
{ return S_iioctl_siocsifnetmask(reqport, ifnam, netmask); }
#else
(
	io_t reqport,
	ifname_t ifnam,
	sockaddr_t netmask
);
#endif

/* Routine iioctl_siocgifmetric */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_iioctl_siocgifmetric
#if	defined(LINTLIBRARY)
    (reqport, ifnam, metric)
	io_t reqport;
	ifname_t ifnam;
	int *metric;
{ return S_iioctl_siocgifmetric(reqport, ifnam, metric); }
#else
(
	io_t reqport,
	ifname_t ifnam,
	int *metric
);
#endif

/* Routine iioctl_siocsifmetric */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_iioctl_siocsifmetric
#if	defined(LINTLIBRARY)
    (reqport, ifnam, metric)
	io_t reqport;
	ifname_t ifnam;
	int metric;
{ return S_iioctl_siocsifmetric(reqport, ifnam, metric); }
#else
(
	io_t reqport,
	ifname_t ifnam,
	int metric
);
#endif

/* Routine iioctl_siocdifaddr */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_iioctl_siocdifaddr
#if	defined(LINTLIBRARY)
    (reqport, ifnam, addr)
	io_t reqport;
	ifname_t ifnam;
	sockaddr_t addr;
{ return S_iioctl_siocdifaddr(reqport, ifnam, addr); }
#else
(
	io_t reqport,
	ifname_t ifnam,
	sockaddr_t addr
);
#endif

/* Routine iioctl_siocgifaddr */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_iioctl_siocgifaddr
#if	defined(LINTLIBRARY)
    (reqport, ifnam, addr)
	io_t reqport;
	ifname_t ifnam;
	sockaddr_t *addr;
{ return S_iioctl_siocgifaddr(reqport, ifnam, addr); }
#else
(
	io_t reqport,
	ifname_t ifnam,
	sockaddr_t *addr
);
#endif

/* Routine iioctl_siocgifdstaddr */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_iioctl_siocgifdstaddr
#if	defined(LINTLIBRARY)
    (reqport, ifnam, dstaddr)
	io_t reqport;
	ifname_t ifnam;
	sockaddr_t *dstaddr;
{ return S_iioctl_siocgifdstaddr(reqport, ifnam, dstaddr); }
#else
(
	io_t reqport,
	ifname_t ifnam,
	sockaddr_t *dstaddr
);
#endif

/* Routine iioctl_siocgifbrdaddr */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_iioctl_siocgifbrdaddr
#if	defined(LINTLIBRARY)
    (reqport, ifnam, brdaddr)
	io_t reqport;
	ifname_t ifnam;
	sockaddr_t *brdaddr;
{ return S_iioctl_siocgifbrdaddr(reqport, ifnam, brdaddr); }
#else
(
	io_t reqport,
	ifname_t ifnam,
	sockaddr_t *brdaddr
);
#endif

/* Routine iioctl_siocgifnetmask */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_iioctl_siocgifnetmask
#if	defined(LINTLIBRARY)
    (reqport, ifnam, netmask)
	io_t reqport;
	ifname_t ifnam;
	sockaddr_t *netmask;
{ return S_iioctl_siocgifnetmask(reqport, ifnam, netmask); }
#else
(
	io_t reqport,
	ifname_t ifnam,
	sockaddr_t *netmask
);
#endif

/* Routine iioctl_siocgifhwaddr */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_iioctl_siocgifhwaddr
#if	defined(LINTLIBRARY)
    (reqport, ifnam, netmask)
	io_t reqport;
	ifname_t ifnam;
	sockaddr_t *netmask;
{ return S_iioctl_siocgifhwaddr(reqport, ifnam, netmask); }
#else
(
	io_t reqport,
	ifname_t ifnam,
	sockaddr_t *netmask
);
#endif

/* Routine iioctl_siocgifmtu */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_iioctl_siocgifmtu
#if	defined(LINTLIBRARY)
    (reqport, ifnam, mtu)
	io_t reqport;
	ifname_t ifnam;
	int *mtu;
{ return S_iioctl_siocgifmtu(reqport, ifnam, mtu); }
#else
(
	io_t reqport,
	ifname_t ifnam,
	int *mtu
);
#endif

/* Routine iioctl_siocsifmtu */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_iioctl_siocsifmtu
#if	defined(LINTLIBRARY)
    (reqport, ifnam, mtu)
	io_t reqport;
	ifname_t ifnam;
	int mtu;
{ return S_iioctl_siocsifmtu(reqport, ifnam, mtu); }
#else
(
	io_t reqport,
	ifname_t ifnam,
	int mtu
);
#endif

/* Routine iioctl_siocgifindex */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_iioctl_siocgifindex
#if	defined(LINTLIBRARY)
    (reqport, ifnam, index)
	io_t reqport;
	ifname_t ifnam;
	int *index;
{ return S_iioctl_siocgifindex(reqport, ifnam, index); }
#else
(
	io_t reqport,
	ifname_t ifnam,
	int *index
);
#endif

/* Routine iioctl_siocgifname */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_iioctl_siocgifname
#if	defined(LINTLIBRARY)
    (reqport, ifnam, index)
	io_t reqport;
	ifname_t ifnam;
	int *index;
{ return S_iioctl_siocgifname(reqport, ifnam, index); }
#else
(
	io_t reqport,
	ifname_t ifnam,
	int *index
);
#endif

#endif	/* not defined(_iioctl_server_) */
