/* Module iioctl */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#define EXPORT_BOOLEAN
#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/message.h>
#include <mach/mig_errors.h>
#include <mach/mig_support.h>

#ifndef	mig_internal
#define	mig_internal	static
#endif

#ifndef	mig_external
#define mig_external
#endif

#ifndef	TypeCheck
#define	TypeCheck 1
#endif

#ifndef	UseExternRCSId
#define	UseExternRCSId		1
#endif

#define BAD_TYPECHECK(type, check) ({\
  union { mach_msg_type_t t; unsigned32_t w; } _t, _c;\
  _t.t = *(type); _c.t = *(check); _t.w != _c.w; })
#define msgh_request_port	msgh_local_port
#define MACH_MSGH_BITS_REQUEST(bits)	MACH_MSGH_BITS_LOCAL(bits)
#define msgh_reply_port		msgh_remote_port
#define MACH_MSGH_BITS_REPLY(bits)	MACH_MSGH_BITS_REMOTE(bits)

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
mig_internal void _Xiioctl_siocsifaddr
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t ifnamType;
		ifname_t ifnam;
		mach_msg_type_t addrType;
		sockaddr_t addr;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_iioctl_siocsifaddr
		(io_t reqport, ifname_t ifnam, sockaddr_t addr);

	auto const mach_msg_type_t ifnamCheck = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t addrCheck = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

#if	TypeCheck
	if ((In0P->Head.msgh_size != 64) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->ifnamType, &ifnamCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->addrType, &addrCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = S_iioctl_siocsifaddr(In0P->Head.msgh_request_port, In0P->ifnam, In0P->addr);
}

/* Routine iioctl_siocsifdstaddr */
mig_internal void _Xiioctl_siocsifdstaddr
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t ifnamType;
		ifname_t ifnam;
		mach_msg_type_t dstaddrType;
		sockaddr_t dstaddr;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_iioctl_siocsifdstaddr
		(io_t reqport, ifname_t ifnam, sockaddr_t dstaddr);

	auto const mach_msg_type_t ifnamCheck = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t dstaddrCheck = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

#if	TypeCheck
	if ((In0P->Head.msgh_size != 64) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->ifnamType, &ifnamCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->dstaddrType, &dstaddrCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = S_iioctl_siocsifdstaddr(In0P->Head.msgh_request_port, In0P->ifnam, In0P->dstaddr);
}

/* Routine iioctl_siocsifflags */
mig_internal void _Xiioctl_siocsifflags
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t ifnamType;
		ifname_t ifnam;
		mach_msg_type_t flagsType;
		short flags;
		char flagsPad[2];
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_iioctl_siocsifflags
		(io_t reqport, ifname_t ifnam, short flags);

	auto const mach_msg_type_t ifnamCheck = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t flagsCheck = {
		/* msgt_name = */		1,
		/* msgt_size = */		16,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

#if	TypeCheck
	if ((In0P->Head.msgh_size != 52) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->ifnamType, &ifnamCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->flagsType, &flagsCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = S_iioctl_siocsifflags(In0P->Head.msgh_request_port, In0P->ifnam, In0P->flags);
}

/* Routine iioctl_siocgifflags */
mig_internal void _Xiioctl_siocgifflags
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t ifnamType;
		ifname_t ifnam;
		mach_msg_type_t flagsType;
		short flags;
		char flagsPad[2];
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t ifnamType;
		ifname_t ifnam;
		mach_msg_type_t flagsType;
		short flags;
		char flagsPad[2];
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_iioctl_siocgifflags
		(io_t reqport, ifname_t ifnam, short *flags);

	auto const mach_msg_type_t ifnamCheck = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t flagsCheck = {
		/* msgt_name = */		1,
		/* msgt_size = */		16,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t ifnamType = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t flagsType = {
		/* msgt_name = */		1,
		/* msgt_size = */		16,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

#if	TypeCheck
	if ((In0P->Head.msgh_size != 52) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->ifnamType, &ifnamCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->flagsType, &flagsCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = S_iioctl_siocgifflags(In0P->Head.msgh_request_port, In0P->ifnam, &In0P->flags);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 60;

	OutP->ifnamType = ifnamType;

	{ typedef struct { char data[16]; } *sp; * (sp) OutP->ifnam = * (sp) In0P->ifnam; }

	OutP->flagsType = flagsType;

	OutP->flags = In0P->flags;
}

/* Routine iioctl_siocsifbrdaddr */
mig_internal void _Xiioctl_siocsifbrdaddr
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t ifnamType;
		ifname_t ifnam;
		mach_msg_type_t brdaddrType;
		sockaddr_t brdaddr;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_iioctl_siocsifbrdaddr
		(io_t reqport, ifname_t ifnam, sockaddr_t brdaddr);

	auto const mach_msg_type_t ifnamCheck = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t brdaddrCheck = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

#if	TypeCheck
	if ((In0P->Head.msgh_size != 64) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->ifnamType, &ifnamCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->brdaddrType, &brdaddrCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = S_iioctl_siocsifbrdaddr(In0P->Head.msgh_request_port, In0P->ifnam, In0P->brdaddr);
}

/* Routine iioctl_siocsifnetmask */
mig_internal void _Xiioctl_siocsifnetmask
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t ifnamType;
		ifname_t ifnam;
		mach_msg_type_t netmaskType;
		sockaddr_t netmask;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_iioctl_siocsifnetmask
		(io_t reqport, ifname_t ifnam, sockaddr_t netmask);

	auto const mach_msg_type_t ifnamCheck = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t netmaskCheck = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

#if	TypeCheck
	if ((In0P->Head.msgh_size != 64) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->ifnamType, &ifnamCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->netmaskType, &netmaskCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = S_iioctl_siocsifnetmask(In0P->Head.msgh_request_port, In0P->ifnam, In0P->netmask);
}

/* Routine iioctl_siocgifmetric */
mig_internal void _Xiioctl_siocgifmetric
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t ifnamType;
		ifname_t ifnam;
		mach_msg_type_t metricType;
		int metric;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t ifnamType;
		ifname_t ifnam;
		mach_msg_type_t metricType;
		int metric;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_iioctl_siocgifmetric
		(io_t reqport, ifname_t ifnam, int *metric);

	auto const mach_msg_type_t ifnamCheck = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t metricCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t ifnamType = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t metricType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

#if	TypeCheck
	if ((In0P->Head.msgh_size != 52) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->ifnamType, &ifnamCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->metricType, &metricCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = S_iioctl_siocgifmetric(In0P->Head.msgh_request_port, In0P->ifnam, &In0P->metric);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 60;

	OutP->ifnamType = ifnamType;

	{ typedef struct { char data[16]; } *sp; * (sp) OutP->ifnam = * (sp) In0P->ifnam; }

	OutP->metricType = metricType;

	OutP->metric = In0P->metric;
}

/* Routine iioctl_siocsifmetric */
mig_internal void _Xiioctl_siocsifmetric
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t ifnamType;
		ifname_t ifnam;
		mach_msg_type_t metricType;
		int metric;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_iioctl_siocsifmetric
		(io_t reqport, ifname_t ifnam, int metric);

	auto const mach_msg_type_t ifnamCheck = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t metricCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

#if	TypeCheck
	if ((In0P->Head.msgh_size != 52) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->ifnamType, &ifnamCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->metricType, &metricCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = S_iioctl_siocsifmetric(In0P->Head.msgh_request_port, In0P->ifnam, In0P->metric);
}

/* Routine iioctl_siocdifaddr */
mig_internal void _Xiioctl_siocdifaddr
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t ifnamType;
		ifname_t ifnam;
		mach_msg_type_t addrType;
		sockaddr_t addr;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_iioctl_siocdifaddr
		(io_t reqport, ifname_t ifnam, sockaddr_t addr);

	auto const mach_msg_type_t ifnamCheck = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t addrCheck = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

#if	TypeCheck
	if ((In0P->Head.msgh_size != 64) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->ifnamType, &ifnamCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->addrType, &addrCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = S_iioctl_siocdifaddr(In0P->Head.msgh_request_port, In0P->ifnam, In0P->addr);
}

/* Routine iioctl_siocgifaddr */
mig_internal void _Xiioctl_siocgifaddr
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t ifnamType;
		ifname_t ifnam;
		mach_msg_type_t addrType;
		sockaddr_t addr;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t ifnamType;
		ifname_t ifnam;
		mach_msg_type_t addrType;
		sockaddr_t addr;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_iioctl_siocgifaddr
		(io_t reqport, ifname_t ifnam, sockaddr_t *addr);

	auto const mach_msg_type_t ifnamCheck = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t addrCheck = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t ifnamType = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t addrType = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

#if	TypeCheck
	if ((In0P->Head.msgh_size != 64) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->ifnamType, &ifnamCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->addrType, &addrCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = S_iioctl_siocgifaddr(In0P->Head.msgh_request_port, In0P->ifnam, &In0P->addr);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 72;

	OutP->ifnamType = ifnamType;

	{ typedef struct { char data[16]; } *sp; * (sp) OutP->ifnam = * (sp) In0P->ifnam; }

	OutP->addrType = addrType;

	OutP->addr = In0P->addr;
}

/* Routine iioctl_siocgifdstaddr */
mig_internal void _Xiioctl_siocgifdstaddr
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t ifnamType;
		ifname_t ifnam;
		mach_msg_type_t dstaddrType;
		sockaddr_t dstaddr;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t ifnamType;
		ifname_t ifnam;
		mach_msg_type_t dstaddrType;
		sockaddr_t dstaddr;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_iioctl_siocgifdstaddr
		(io_t reqport, ifname_t ifnam, sockaddr_t *dstaddr);

	auto const mach_msg_type_t ifnamCheck = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t dstaddrCheck = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t ifnamType = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t dstaddrType = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

#if	TypeCheck
	if ((In0P->Head.msgh_size != 64) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->ifnamType, &ifnamCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->dstaddrType, &dstaddrCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = S_iioctl_siocgifdstaddr(In0P->Head.msgh_request_port, In0P->ifnam, &In0P->dstaddr);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 72;

	OutP->ifnamType = ifnamType;

	{ typedef struct { char data[16]; } *sp; * (sp) OutP->ifnam = * (sp) In0P->ifnam; }

	OutP->dstaddrType = dstaddrType;

	OutP->dstaddr = In0P->dstaddr;
}

/* Routine iioctl_siocgifbrdaddr */
mig_internal void _Xiioctl_siocgifbrdaddr
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t ifnamType;
		ifname_t ifnam;
		mach_msg_type_t brdaddrType;
		sockaddr_t brdaddr;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t ifnamType;
		ifname_t ifnam;
		mach_msg_type_t brdaddrType;
		sockaddr_t brdaddr;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_iioctl_siocgifbrdaddr
		(io_t reqport, ifname_t ifnam, sockaddr_t *brdaddr);

	auto const mach_msg_type_t ifnamCheck = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t brdaddrCheck = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t ifnamType = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t brdaddrType = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

#if	TypeCheck
	if ((In0P->Head.msgh_size != 64) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->ifnamType, &ifnamCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->brdaddrType, &brdaddrCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = S_iioctl_siocgifbrdaddr(In0P->Head.msgh_request_port, In0P->ifnam, &In0P->brdaddr);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 72;

	OutP->ifnamType = ifnamType;

	{ typedef struct { char data[16]; } *sp; * (sp) OutP->ifnam = * (sp) In0P->ifnam; }

	OutP->brdaddrType = brdaddrType;

	OutP->brdaddr = In0P->brdaddr;
}

/* Routine iioctl_siocgifnetmask */
mig_internal void _Xiioctl_siocgifnetmask
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t ifnamType;
		ifname_t ifnam;
		mach_msg_type_t netmaskType;
		sockaddr_t netmask;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t ifnamType;
		ifname_t ifnam;
		mach_msg_type_t netmaskType;
		sockaddr_t netmask;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_iioctl_siocgifnetmask
		(io_t reqport, ifname_t ifnam, sockaddr_t *netmask);

	auto const mach_msg_type_t ifnamCheck = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t netmaskCheck = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t ifnamType = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t netmaskType = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

#if	TypeCheck
	if ((In0P->Head.msgh_size != 64) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->ifnamType, &ifnamCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->netmaskType, &netmaskCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = S_iioctl_siocgifnetmask(In0P->Head.msgh_request_port, In0P->ifnam, &In0P->netmask);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 72;

	OutP->ifnamType = ifnamType;

	{ typedef struct { char data[16]; } *sp; * (sp) OutP->ifnam = * (sp) In0P->ifnam; }

	OutP->netmaskType = netmaskType;

	OutP->netmask = In0P->netmask;
}

/* Routine iioctl_siocgifhwaddr */
mig_internal void _Xiioctl_siocgifhwaddr
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t ifnamType;
		ifname_t ifnam;
		mach_msg_type_t netmaskType;
		sockaddr_t netmask;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t ifnamType;
		ifname_t ifnam;
		mach_msg_type_t netmaskType;
		sockaddr_t netmask;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_iioctl_siocgifhwaddr
		(io_t reqport, ifname_t ifnam, sockaddr_t *netmask);

	auto const mach_msg_type_t ifnamCheck = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t netmaskCheck = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t ifnamType = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t netmaskType = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

#if	TypeCheck
	if ((In0P->Head.msgh_size != 64) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->ifnamType, &ifnamCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->netmaskType, &netmaskCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = S_iioctl_siocgifhwaddr(In0P->Head.msgh_request_port, In0P->ifnam, &In0P->netmask);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 72;

	OutP->ifnamType = ifnamType;

	{ typedef struct { char data[16]; } *sp; * (sp) OutP->ifnam = * (sp) In0P->ifnam; }

	OutP->netmaskType = netmaskType;

	OutP->netmask = In0P->netmask;
}

/* Routine iioctl_siocgifmtu */
mig_internal void _Xiioctl_siocgifmtu
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t ifnamType;
		ifname_t ifnam;
		mach_msg_type_t mtuType;
		int mtu;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t ifnamType;
		ifname_t ifnam;
		mach_msg_type_t mtuType;
		int mtu;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_iioctl_siocgifmtu
		(io_t reqport, ifname_t ifnam, int *mtu);

	auto const mach_msg_type_t ifnamCheck = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t mtuCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t ifnamType = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t mtuType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

#if	TypeCheck
	if ((In0P->Head.msgh_size != 52) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->ifnamType, &ifnamCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->mtuType, &mtuCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = S_iioctl_siocgifmtu(In0P->Head.msgh_request_port, In0P->ifnam, &In0P->mtu);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 60;

	OutP->ifnamType = ifnamType;

	{ typedef struct { char data[16]; } *sp; * (sp) OutP->ifnam = * (sp) In0P->ifnam; }

	OutP->mtuType = mtuType;

	OutP->mtu = In0P->mtu;
}

/* Routine iioctl_siocsifmtu */
mig_internal void _Xiioctl_siocsifmtu
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t ifnamType;
		ifname_t ifnam;
		mach_msg_type_t mtuType;
		int mtu;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_iioctl_siocsifmtu
		(io_t reqport, ifname_t ifnam, int mtu);

	auto const mach_msg_type_t ifnamCheck = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t mtuCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

#if	TypeCheck
	if ((In0P->Head.msgh_size != 52) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->ifnamType, &ifnamCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->mtuType, &mtuCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = S_iioctl_siocsifmtu(In0P->Head.msgh_request_port, In0P->ifnam, In0P->mtu);
}

/* Routine iioctl_siocgifindex */
mig_internal void _Xiioctl_siocgifindex
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t ifnamType;
		ifname_t ifnam;
		mach_msg_type_t indexType;
		int index;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t ifnamType;
		ifname_t ifnam;
		mach_msg_type_t indexType;
		int index;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_iioctl_siocgifindex
		(io_t reqport, ifname_t ifnam, int *index);

	auto const mach_msg_type_t ifnamCheck = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t indexCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t ifnamType = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t indexType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

#if	TypeCheck
	if ((In0P->Head.msgh_size != 52) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->ifnamType, &ifnamCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->indexType, &indexCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = S_iioctl_siocgifindex(In0P->Head.msgh_request_port, In0P->ifnam, &In0P->index);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 60;

	OutP->ifnamType = ifnamType;

	{ typedef struct { char data[16]; } *sp; * (sp) OutP->ifnam = * (sp) In0P->ifnam; }

	OutP->indexType = indexType;

	OutP->index = In0P->index;
}

/* Routine iioctl_siocgifname */
mig_internal void _Xiioctl_siocgifname
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t ifnamType;
		ifname_t ifnam;
		mach_msg_type_t indexType;
		int index;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t ifnamType;
		ifname_t ifnam;
		mach_msg_type_t indexType;
		int index;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_iioctl_siocgifname
		(io_t reqport, ifname_t ifnam, int *index);

	auto const mach_msg_type_t ifnamCheck = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t indexCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t ifnamType = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t indexType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

#if	TypeCheck
	if ((In0P->Head.msgh_size != 52) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->ifnamType, &ifnamCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->indexType, &indexCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = S_iioctl_siocgifname(In0P->Head.msgh_request_port, In0P->ifnam, &In0P->index);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 60;

	OutP->ifnamType = ifnamType;

	{ typedef struct { char data[16]; } *sp; * (sp) OutP->ifnam = * (sp) In0P->ifnam; }

	OutP->indexType = indexType;

	OutP->index = In0P->index;
}

static mig_routine_t iioctl_server_routines[] = {
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		_Xiioctl_siocsifaddr,
		0,
		_Xiioctl_siocsifdstaddr,
		0,
		_Xiioctl_siocsifflags,
		_Xiioctl_siocgifflags,
		0,
		_Xiioctl_siocsifbrdaddr,
		0,
		0,
		_Xiioctl_siocsifnetmask,
		_Xiioctl_siocgifmetric,
		_Xiioctl_siocsifmetric,
		_Xiioctl_siocdifaddr,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		_Xiioctl_siocgifaddr,
		_Xiioctl_siocgifdstaddr,
		_Xiioctl_siocgifbrdaddr,
		0,
		_Xiioctl_siocgifnetmask,
		0,
		_Xiioctl_siocgifhwaddr,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		_Xiioctl_siocgifmtu,
		_Xiioctl_siocsifmtu,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		_Xiioctl_siocgifindex,
		_Xiioctl_siocgifname,
};

mig_external boolean_t iioctl_server
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	register mach_msg_header_t *InP =  InHeadP;
	register mig_reply_header_t *OutP = (mig_reply_header_t *) OutHeadP;

	auto const mach_msg_type_t RetCodeType = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	register mig_routine_t routine;

	OutP->Head.msgh_bits = MACH_MSGH_BITS(MACH_MSGH_BITS_REPLY(InP->msgh_bits), 0);
	OutP->Head.msgh_size = sizeof *OutP;
	OutP->Head.msgh_remote_port = InP->msgh_reply_port;
	OutP->Head.msgh_local_port = MACH_PORT_NULL;
	OutP->Head.msgh_seqno = 0;
	OutP->Head.msgh_id = InP->msgh_id + 100;

	OutP->RetCodeType = RetCodeType;

	if ((InP->msgh_id > 112091) || (InP->msgh_id < 112000) ||
	    ((routine = iioctl_server_routines[InP->msgh_id - 112000]) == 0)) {
		OutP->RetCode = MIG_BAD_ID;
		return FALSE;
	}
	(*routine) (InP, &OutP->Head);
	return TRUE;
}

mig_external mig_routine_t iioctl_server_routine
	(const mach_msg_header_t *InHeadP)
{
	register int msgh_id;

	msgh_id = InHeadP->msgh_id - 112000;

	if ((msgh_id > 91) || (msgh_id < 0))
		return 0;

	return iioctl_server_routines[msgh_id];
}

