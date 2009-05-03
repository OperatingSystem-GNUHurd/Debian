/* Module socket */

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
#include "pfinet.h"

/* Routine socket_create */
mig_internal void _Xsocket_create
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t sock_typeType;
		int sock_type;
		mach_msg_type_t protocolType;
		int protocol;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t sockType;
		mach_port_t sock;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_socket_create
		(trivfs_protid_t server, int sock_type, int protocol, mach_port_t *sock, mach_msg_type_name_t *sockPoly);

	boolean_t msgh_simple;
	auto const mach_msg_type_t sock_typeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t protocolCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t sockType = {
		/* msgt_name = */		-1,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	trivfs_protid_t server;
	mach_msg_type_name_t sockPoly;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 40) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->sock_typeType, &sock_typeCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->protocolType, &protocolCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	server = trivfs_begin_using_protid(In0P->Head.msgh_request_port);

	OutP->RetCode = S_socket_create(server, In0P->sock_type, In0P->protocol, &OutP->sock, &sockPoly);
	trivfs_end_using_protid(server);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	OutP->Head.msgh_size = 40;

	OutP->sockType = sockType;

	if (MACH_MSG_TYPE_PORT_ANY(sockPoly))
		msgh_simple = FALSE;

	OutP->sockType.msgt_name = sockPoly;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
}

/* Routine socket_listen */
mig_internal void _Xsocket_listen
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t queue_limitType;
		int queue_limit;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_socket_listen
		(sock_user_t sock, int queue_limit);

	auto const mach_msg_type_t queue_limitCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	sock_user_t sock;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 32) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->queue_limitType, &queue_limitCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	sock = begin_using_socket_port(In0P->Head.msgh_request_port);

	OutP->RetCode = S_socket_listen(sock, In0P->queue_limit);
	end_using_socket_port(sock);
}

/* Routine socket_accept */
mig_internal void _Xsocket_accept
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t conn_sockType;
		mach_port_t conn_sock;
		mach_msg_type_t peer_addrType;
		mach_port_t peer_addr;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_socket_accept
		(sock_user_t sock, mach_port_t *conn_sock, mach_msg_type_name_t *conn_sockPoly, mach_port_t *peer_addr, mach_msg_type_name_t *peer_addrPoly);

	boolean_t msgh_simple;
	auto const mach_msg_type_t conn_sockType = {
		/* msgt_name = */		-1,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t peer_addrType = {
		/* msgt_name = */		-1,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	sock_user_t sock;
	mach_msg_type_name_t conn_sockPoly;
	mach_msg_type_name_t peer_addrPoly;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 24) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	sock = begin_using_socket_port(In0P->Head.msgh_request_port);

	OutP->RetCode = S_socket_accept(sock, &OutP->conn_sock, &conn_sockPoly, &OutP->peer_addr, &peer_addrPoly);
	end_using_socket_port(sock);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	OutP->Head.msgh_size = 48;

	OutP->conn_sockType = conn_sockType;

	if (MACH_MSG_TYPE_PORT_ANY(conn_sockPoly))
		msgh_simple = FALSE;

	OutP->conn_sockType.msgt_name = conn_sockPoly;

	OutP->peer_addrType = peer_addrType;

	if (MACH_MSG_TYPE_PORT_ANY(peer_addrPoly))
		msgh_simple = FALSE;

	OutP->peer_addrType.msgt_name = peer_addrPoly;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
}

/* Routine socket_connect */
mig_internal void _Xsocket_connect
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t addrType;
		addr_port_t addr;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_socket_connect
		(sock_user_t sock, sock_addr_t addr);

	auto const mach_msg_type_t addrCheck = {
		/* msgt_name = */		17,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	sock_user_t sock;
	sock_addr_t addr;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 32) ||
	    !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->addrType, &addrCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	sock = begin_using_socket_port(In0P->Head.msgh_request_port);

	addr = begin_using_sockaddr_port(In0P->addr);

	OutP->RetCode = S_socket_connect(sock, addr);
	end_using_sockaddr_port(addr);
	end_using_socket_port(sock);
}

/* Routine socket_bind */
mig_internal void _Xsocket_bind
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t addrType;
		addr_port_t addr;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_socket_bind
		(sock_user_t sock, sock_addr_t addr);

	auto const mach_msg_type_t addrCheck = {
		/* msgt_name = */		17,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	sock_user_t sock;
	sock_addr_t addr;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 32) ||
	    !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->addrType, &addrCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	sock = begin_using_socket_port(In0P->Head.msgh_request_port);

	addr = begin_using_sockaddr_port(In0P->addr);

	OutP->RetCode = S_socket_bind(sock, addr);
	end_using_sockaddr_port(addr);
	end_using_socket_port(sock);
}

/* Routine socket_name */
mig_internal void _Xsocket_name
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t addrType;
		mach_port_t addr;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_socket_name
		(sock_user_t sock, mach_port_t *addr, mach_msg_type_name_t *addrPoly);

	boolean_t msgh_simple;
	auto const mach_msg_type_t addrType = {
		/* msgt_name = */		-1,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	sock_user_t sock;
	mach_msg_type_name_t addrPoly;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 24) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	sock = begin_using_socket_port(In0P->Head.msgh_request_port);

	OutP->RetCode = S_socket_name(sock, &OutP->addr, &addrPoly);
	end_using_socket_port(sock);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	OutP->Head.msgh_size = 40;

	OutP->addrType = addrType;

	if (MACH_MSG_TYPE_PORT_ANY(addrPoly))
		msgh_simple = FALSE;

	OutP->addrType.msgt_name = addrPoly;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
}

/* Routine socket_peername */
mig_internal void _Xsocket_peername
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t addrType;
		mach_port_t addr;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_socket_peername
		(sock_user_t sock, mach_port_t *addr, mach_msg_type_name_t *addrPoly);

	boolean_t msgh_simple;
	auto const mach_msg_type_t addrType = {
		/* msgt_name = */		-1,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	sock_user_t sock;
	mach_msg_type_name_t addrPoly;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 24) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	sock = begin_using_socket_port(In0P->Head.msgh_request_port);

	OutP->RetCode = S_socket_peername(sock, &OutP->addr, &addrPoly);
	end_using_socket_port(sock);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	OutP->Head.msgh_size = 40;

	OutP->addrType = addrType;

	if (MACH_MSG_TYPE_PORT_ANY(addrPoly))
		msgh_simple = FALSE;

	OutP->addrType.msgt_name = addrPoly;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
}

/* Routine socket_connect2 */
mig_internal void _Xsocket_connect2
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t sock2Type;
		socket_t sock2;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_socket_connect2
		(sock_user_t sock1, sock_user_t sock2);

	auto const mach_msg_type_t sock2Check = {
		/* msgt_name = */		17,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	sock_user_t sock1;
	sock_user_t sock2;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 32) ||
	    !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->sock2Type, &sock2Check))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	sock1 = begin_using_socket_port(In0P->Head.msgh_request_port);

	sock2 = begin_using_socket_port(In0P->sock2);

	OutP->RetCode = S_socket_connect2(sock1, sock2);
	end_using_socket_port(sock2);
	end_using_socket_port(sock1);
}

/* Routine socket_create_address */
mig_internal void _Xsocket_create_address
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t sockaddr_typeType;
		int sockaddr_type;
		mach_msg_type_long_t sockaddrType;
		char sockaddr[2048];
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t addrType;
		mach_port_t addr;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_socket_create_address
		(mach_port_t server, int sockaddr_type, data_t sockaddr, mach_msg_type_number_t sockaddrCnt, mach_port_t *addr, mach_msg_type_name_t *addrPoly);

	boolean_t msgh_simple;
	unsigned int msgh_size;

	auto const mach_msg_type_t sockaddr_typeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t addrType = {
		/* msgt_name = */		-1,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	mach_msg_type_name_t addrPoly;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size < 44))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->sockaddr_typeType, &sockaddr_typeCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if ((In0P->sockaddrType.msgtl_header.msgt_longform != TRUE) ||
	    (In0P->sockaddrType.msgtl_name != 8) ||
	    (In0P->sockaddrType.msgtl_size != 8))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (msgh_size != 44 + ((In0P->sockaddrType.msgtl_header.msgt_inline) ? (In0P->sockaddrType.msgtl_number + 3) & ~3 : sizeof(char *)))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = S_socket_create_address(In0P->Head.msgh_request_port, In0P->sockaddr_type, (In0P->sockaddrType.msgtl_header.msgt_inline) ? In0P->sockaddr : *((char **)In0P->sockaddr), In0P->sockaddrType.msgtl_number, &OutP->addr, &addrPoly);
	if (OutP->RetCode == KERN_SUCCESS)
		if (!In0P->sockaddrType.msgtl_header.msgt_inline)
			__mig_deallocate(* (vm_offset_t *) In0P->sockaddr,  In0P->sockaddrType.msgtl_number);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	OutP->Head.msgh_size = 40;

	OutP->addrType = addrType;

	if (MACH_MSG_TYPE_PORT_ANY(addrPoly))
		msgh_simple = FALSE;

	OutP->addrType.msgt_name = addrPoly;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
}

/* Routine socket_fabricate_address */
mig_internal void _Xsocket_fabricate_address
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t sockaddr_typeType;
		int sockaddr_type;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t addrType;
		mach_port_t addr;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_socket_fabricate_address
		(mach_port_t server, int sockaddr_type, mach_port_t *addr, mach_msg_type_name_t *addrPoly);

	boolean_t msgh_simple;
	auto const mach_msg_type_t sockaddr_typeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t addrType = {
		/* msgt_name = */		-1,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	mach_msg_type_name_t addrPoly;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 32) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->sockaddr_typeType, &sockaddr_typeCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = S_socket_fabricate_address(In0P->Head.msgh_request_port, In0P->sockaddr_type, &OutP->addr, &addrPoly);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	OutP->Head.msgh_size = 40;

	OutP->addrType = addrType;

	if (MACH_MSG_TYPE_PORT_ANY(addrPoly))
		msgh_simple = FALSE;

	OutP->addrType.msgt_name = addrPoly;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
}

/* Routine socket_whatis_address */
mig_internal void _Xsocket_whatis_address
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t sockaddr_typeType;
		int sockaddr_type;
		mach_msg_type_long_t sockaddrType;
		char sockaddr[2048];
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_socket_whatis_address
		(sock_addr_t addr, int *sockaddr_type, data_t *sockaddr, mach_msg_type_number_t *sockaddrCnt);

	boolean_t msgh_simple;
	auto const mach_msg_type_t sockaddr_typeType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_long_t sockaddrType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	8,
		/* msgtl_size = */	8,
		/* msgtl_number = */	2048,
	};

	sock_addr_t addr;
	mach_msg_type_number_t sockaddrCnt;

	char *sockaddrP;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 24) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	addr = begin_using_sockaddr_port(In0P->Head.msgh_request_port);

	sockaddrP = OutP->sockaddr;
	sockaddrCnt = 2048;

	OutP->RetCode = S_socket_whatis_address(addr, &OutP->sockaddr_type, &sockaddrP, &sockaddrCnt);
	end_using_sockaddr_port(addr);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;

	OutP->sockaddr_typeType = sockaddr_typeType;

	OutP->sockaddrType = sockaddrType;
	if (sockaddrP != OutP->sockaddr) {
		OutP->sockaddrType.msgtl_header.msgt_inline = FALSE;
		OutP->sockaddrType.msgtl_header.msgt_deallocate = TRUE;
		*((char **)OutP->sockaddr) = sockaddrP;
		msgh_simple = FALSE;
	}

	OutP->sockaddrType.msgtl_number = sockaddrCnt;
	OutP->Head.msgh_size = 52 + ((OutP->sockaddrType.msgtl_header.msgt_inline) ? (sockaddrCnt + 3) & ~3 : sizeof(char *));

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
}

/* Routine socket_shutdown */
mig_internal void _Xsocket_shutdown
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t directionType;
		int direction;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_socket_shutdown
		(sock_user_t sock, int direction);

	auto const mach_msg_type_t directionCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	sock_user_t sock;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 32) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->directionType, &directionCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	sock = begin_using_socket_port(In0P->Head.msgh_request_port);

	OutP->RetCode = S_socket_shutdown(sock, In0P->direction);
	end_using_socket_port(sock);
}

/* Routine socket_getopt */
mig_internal void _Xsocket_getopt
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t levelType;
		int level;
		mach_msg_type_t optionType;
		int option;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_long_t optvalType;
		char optval[2048];
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_socket_getopt
		(sock_user_t sock, int level, int option, data_t *optval, mach_msg_type_number_t *optvalCnt);

	boolean_t msgh_simple;
	auto const mach_msg_type_t levelCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t optionCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_long_t optvalType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	8,
		/* msgtl_size = */	8,
		/* msgtl_number = */	2048,
	};

	sock_user_t sock;
	mach_msg_type_number_t optvalCnt;

	char *optvalP;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 40) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->levelType, &levelCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->optionType, &optionCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	sock = begin_using_socket_port(In0P->Head.msgh_request_port);

	optvalP = OutP->optval;
	optvalCnt = 2048;

	OutP->RetCode = S_socket_getopt(sock, In0P->level, In0P->option, &optvalP, &optvalCnt);
	end_using_socket_port(sock);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;

	OutP->optvalType = optvalType;
	if (optvalP != OutP->optval) {
		OutP->optvalType.msgtl_header.msgt_inline = FALSE;
		OutP->optvalType.msgtl_header.msgt_deallocate = TRUE;
		*((char **)OutP->optval) = optvalP;
		msgh_simple = FALSE;
	}

	OutP->optvalType.msgtl_number = optvalCnt;
	OutP->Head.msgh_size = 44 + ((OutP->optvalType.msgtl_header.msgt_inline) ? (optvalCnt + 3) & ~3 : sizeof(char *));

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
}

/* Routine socket_setopt */
mig_internal void _Xsocket_setopt
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t levelType;
		int level;
		mach_msg_type_t optionType;
		int option;
		mach_msg_type_long_t optvalType;
		char optval[2048];
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_socket_setopt
		(sock_user_t sock, int level, int option, data_t optval, mach_msg_type_number_t optvalCnt);

#if	TypeCheck
	boolean_t msgh_simple;
#endif	/* TypeCheck */

	unsigned int msgh_size;

	auto const mach_msg_type_t levelCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t optionCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	sock_user_t sock;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size < 52))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->levelType, &levelCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->optionType, &optionCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if ((In0P->optvalType.msgtl_header.msgt_longform != TRUE) ||
	    (In0P->optvalType.msgtl_name != 8) ||
	    (In0P->optvalType.msgtl_size != 8))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (msgh_size != 52 + ((In0P->optvalType.msgtl_header.msgt_inline) ? (In0P->optvalType.msgtl_number + 3) & ~3 : sizeof(char *)))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	sock = begin_using_socket_port(In0P->Head.msgh_request_port);

	OutP->RetCode = S_socket_setopt(sock, In0P->level, In0P->option, (In0P->optvalType.msgtl_header.msgt_inline) ? In0P->optval : *((char **)In0P->optval), In0P->optvalType.msgtl_number);
	if (OutP->RetCode == KERN_SUCCESS)
		if (!In0P->optvalType.msgtl_header.msgt_inline)
			__mig_deallocate(* (vm_offset_t *) In0P->optval,  In0P->optvalType.msgtl_number);
	end_using_socket_port(sock);
}

/* Routine socket_send */
mig_internal void _Xsocket_send
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t addrType;
		addr_port_t addr;
		mach_msg_type_t flagsType;
		int flags;
		mach_msg_type_long_t dataType;
		char data[2048];
		mach_msg_type_long_t portsType;
		mach_port_t ports[512];
		mach_msg_type_long_t controlType;
		char control[2048];
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t amountType;
		vm_size_t amount;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Request *In1P;
	register Request *In2P;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_socket_send
		(sock_user_t sock, sock_addr_t addr, int flags, data_t data, mach_msg_type_number_t dataCnt, portarray_t ports, mach_msg_type_number_t portsCnt, data_t control, mach_msg_type_number_t controlCnt, vm_size_t *amount);

	unsigned int msgh_size;
	unsigned int msgh_size_delta;

	auto const mach_msg_type_t addrCheck = {
		/* msgt_name = */		17,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t flagsCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t amountType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	sock_user_t sock;
	sock_addr_t addr;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	if ((msgh_size < 76) ||
	    !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->addrType, &addrCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->flagsType, &flagsCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if ((In0P->dataType.msgtl_header.msgt_longform != TRUE) ||
	    (In0P->dataType.msgtl_name != 8) ||
	    (In0P->dataType.msgtl_size != 8))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	msgh_size_delta = (In0P->dataType.msgtl_header.msgt_inline) ? (In0P->dataType.msgtl_number + 3) & ~3 : sizeof(char *);
#if	TypeCheck
	if (msgh_size < 76 + msgh_size_delta)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
	msgh_size -= msgh_size_delta;
#endif	/* TypeCheck */

	In1P = (Request *) ((char *) In0P + msgh_size_delta - 2048);

#if	TypeCheck
	if ((In1P->portsType.msgtl_header.msgt_longform != TRUE) ||
	    (In1P->portsType.msgtl_name != 17) ||
	    (In1P->portsType.msgtl_size != 32))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	msgh_size_delta = (In1P->portsType.msgtl_header.msgt_inline) ? 4 * In1P->portsType.msgtl_number : sizeof(mach_port_t *);
#if	TypeCheck
	if (msgh_size < 76 + msgh_size_delta)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
	msgh_size -= msgh_size_delta;
#endif	/* TypeCheck */

	In2P = (Request *) ((char *) In1P + msgh_size_delta - 2048);

#if	TypeCheck
	if ((In2P->controlType.msgtl_header.msgt_longform != TRUE) ||
	    (In2P->controlType.msgtl_name != 8) ||
	    (In2P->controlType.msgtl_size != 8))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (msgh_size != 76 + ((In2P->controlType.msgtl_header.msgt_inline) ? (In2P->controlType.msgtl_number + 3) & ~3 : sizeof(char *)))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	sock = begin_using_socket_port(In0P->Head.msgh_request_port);

	addr = begin_using_sockaddr_port(In0P->addr);

	OutP->RetCode = S_socket_send(sock, addr, In0P->flags, (In0P->dataType.msgtl_header.msgt_inline) ? In0P->data : *((char **)In0P->data), In0P->dataType.msgtl_number, (In1P->portsType.msgtl_header.msgt_inline) ? In1P->ports : *((mach_port_t **)In1P->ports), In1P->portsType.msgtl_number, (In2P->controlType.msgtl_header.msgt_inline) ? In2P->control : *((char **)In2P->control), In2P->controlType.msgtl_number, &OutP->amount);
	if (OutP->RetCode == KERN_SUCCESS)
		if (!In2P->controlType.msgtl_header.msgt_inline)
			__mig_deallocate(* (vm_offset_t *) In2P->control,  In2P->controlType.msgtl_number);
	if (OutP->RetCode == KERN_SUCCESS)
		if (!In1P->portsType.msgtl_header.msgt_inline)
			__mig_deallocate(* (vm_offset_t *) In1P->ports, 4 *  In1P->portsType.msgtl_number);
	if (OutP->RetCode == KERN_SUCCESS)
		if (!In0P->dataType.msgtl_header.msgt_inline)
			__mig_deallocate(* (vm_offset_t *) In0P->data,  In0P->dataType.msgtl_number);
	end_using_sockaddr_port(addr);
	end_using_socket_port(sock);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 40;

	OutP->amountType = amountType;
}

/* Routine socket_recv */
mig_internal void _Xsocket_recv
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t flagsType;
		int flags;
		mach_msg_type_t amountType;
		vm_size_t amount;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t addrType;
		mach_port_t addr;
		mach_msg_type_long_t dataType;
		char data[2048];
		mach_msg_type_long_t portsType;
		mach_port_t ports[512];
		mach_msg_type_long_t controlType;
		char control[2048];
		mach_msg_type_t outflagsType;
		int outflags;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_socket_recv
		(sock_user_t sock, mach_port_t *addr, mach_msg_type_name_t *addrPoly, int flags, data_t *data, mach_msg_type_number_t *dataCnt, portarray_t *ports, mach_msg_type_name_t *portsPoly, mach_msg_type_number_t *portsCnt, data_t *control, mach_msg_type_number_t *controlCnt, int *outflags, vm_size_t amount);

	boolean_t msgh_simple;
	unsigned int msgh_size;
	unsigned int msgh_size_delta;

	auto const mach_msg_type_t flagsCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t amountCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t addrType = {
		/* msgt_name = */		-1,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_long_t dataType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	8,
		/* msgtl_size = */	8,
		/* msgtl_number = */	2048,
	};

	auto const mach_msg_type_long_t portsType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	-1,
		/* msgtl_size = */	32,
		/* msgtl_number = */	512,
	};

	auto const mach_msg_type_long_t controlType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	8,
		/* msgtl_size = */	8,
		/* msgtl_number = */	2048,
	};

	auto const mach_msg_type_t outflagsType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	sock_user_t sock;
	mach_msg_type_name_t addrPoly;
	mach_msg_type_number_t dataCnt;
	mach_port_t ports[512];
	mach_msg_type_name_t portsPoly;
	mach_msg_type_number_t portsCnt;
	char control[2048];
	mach_msg_type_number_t controlCnt;
	int outflags;

	char *dataP;
	mach_port_t *portsP;
	char *controlP;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 40) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->flagsType, &flagsCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->amountType, &amountCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	sock = begin_using_socket_port(In0P->Head.msgh_request_port);

	dataP = OutP->data;
	dataCnt = 2048;

	portsP = ports;
	portsCnt = 512;

	controlP = control;
	controlCnt = 2048;

	OutP->RetCode = S_socket_recv(sock, &OutP->addr, &addrPoly, In0P->flags, &dataP, &dataCnt, &portsP, &portsPoly, &portsCnt, &controlP, &controlCnt, &outflags, In0P->amount);
	end_using_socket_port(sock);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;

	OutP->addrType = addrType;

	if (MACH_MSG_TYPE_PORT_ANY(addrPoly))
		msgh_simple = FALSE;

	OutP->addrType.msgt_name = addrPoly;

	OutP->dataType = dataType;
	if (dataP != OutP->data) {
		OutP->dataType.msgtl_header.msgt_inline = FALSE;
		OutP->dataType.msgtl_header.msgt_deallocate = TRUE;
		*((char **)OutP->data) = dataP;
		msgh_simple = FALSE;
	}

	OutP->dataType.msgtl_number = dataCnt;
	msgh_size_delta = (OutP->dataType.msgtl_header.msgt_inline) ? (dataCnt + 3) & ~3 : sizeof(char *);
	msgh_size = 84 + msgh_size_delta;
	OutP = (Reply *) ((char *) OutP + msgh_size_delta - 2048);

	OutP->portsType = portsType;

	if (portsP != ports) {
		OutP->portsType.msgtl_header.msgt_inline = FALSE;
		OutP->portsType.msgtl_header.msgt_deallocate = TRUE;
		*((mach_port_t **)OutP->ports) = portsP;
		msgh_simple = FALSE;
	}
	else {
		memcpy(OutP->ports, ports, 4 * portsCnt);
	}

	if (MACH_MSG_TYPE_PORT_ANY(portsPoly))
		msgh_simple = FALSE;

	OutP->portsType.msgtl_name = portsPoly;

	OutP->portsType.msgtl_number = portsCnt;
	msgh_size_delta = (OutP->portsType.msgtl_header.msgt_inline) ? 4 * portsCnt : sizeof(mach_port_t *);
	msgh_size += msgh_size_delta;
	OutP = (Reply *) ((char *) OutP + msgh_size_delta - 2048);

	OutP->controlType = controlType;

	if (controlP != control) {
		OutP->controlType.msgtl_header.msgt_inline = FALSE;
		OutP->controlType.msgtl_header.msgt_deallocate = TRUE;
		*((char **)OutP->control) = controlP;
		msgh_simple = FALSE;
	}
	else {
		memcpy(OutP->control, control, controlCnt);
	}

	OutP->controlType.msgtl_number = controlCnt;
	msgh_size_delta = (OutP->controlType.msgtl_header.msgt_inline) ? (controlCnt + 3) & ~3 : sizeof(char *);
	msgh_size += msgh_size_delta;
	OutP = (Reply *) ((char *) OutP + msgh_size_delta - 2048);

	OutP->outflagsType = outflagsType;

	OutP->outflags = outflags;

	OutP = (Reply *) OutHeadP;
	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

static mig_routine_t socket_server_routines[] = {
		_Xsocket_create,
		_Xsocket_listen,
		_Xsocket_accept,
		_Xsocket_connect,
		_Xsocket_bind,
		_Xsocket_name,
		_Xsocket_peername,
		_Xsocket_connect2,
		_Xsocket_create_address,
		_Xsocket_fabricate_address,
		_Xsocket_whatis_address,
		_Xsocket_shutdown,
		_Xsocket_getopt,
		_Xsocket_setopt,
		_Xsocket_send,
		_Xsocket_recv,
};

mig_external boolean_t socket_server
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

	if ((InP->msgh_id > 26015) || (InP->msgh_id < 26000) ||
	    ((routine = socket_server_routines[InP->msgh_id - 26000]) == 0)) {
		OutP->RetCode = MIG_BAD_ID;
		return FALSE;
	}
	(*routine) (InP, &OutP->Head);
	return TRUE;
}

mig_external mig_routine_t socket_server_routine
	(const mach_msg_header_t *InHeadP)
{
	register int msgh_id;

	msgh_id = InHeadP->msgh_id - 26000;

	if ((msgh_id > 15) || (msgh_id < 0))
		return 0;

	return socket_server_routines[msgh_id];
}

