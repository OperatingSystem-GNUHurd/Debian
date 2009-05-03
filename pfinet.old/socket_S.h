#ifndef	_socket_server_
#define	_socket_server_

/* Module socket */

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
#include "pfinet.h"

/* Routine socket_create */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_socket_create
#if	defined(LINTLIBRARY)
    (server, sock_type, protocol, sock, sockPoly)
	trivfs_protid_t server;
	int sock_type;
	int protocol;
	mach_port_t *sock;
	mach_msg_type_name_t *sockPoly;
{ return S_socket_create(server, sock_type, protocol, sock, sockPoly); }
#else
(
	trivfs_protid_t server,
	int sock_type,
	int protocol,
	mach_port_t *sock,
	mach_msg_type_name_t *sockPoly
);
#endif

/* Routine socket_listen */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_socket_listen
#if	defined(LINTLIBRARY)
    (sock, queue_limit)
	sock_user_t sock;
	int queue_limit;
{ return S_socket_listen(sock, queue_limit); }
#else
(
	sock_user_t sock,
	int queue_limit
);
#endif

/* Routine socket_accept */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_socket_accept
#if	defined(LINTLIBRARY)
    (sock, conn_sock, conn_sockPoly, peer_addr, peer_addrPoly)
	sock_user_t sock;
	mach_port_t *conn_sock;
	mach_msg_type_name_t *conn_sockPoly;
	mach_port_t *peer_addr;
	mach_msg_type_name_t *peer_addrPoly;
{ return S_socket_accept(sock, conn_sock, conn_sockPoly, peer_addr, peer_addrPoly); }
#else
(
	sock_user_t sock,
	mach_port_t *conn_sock,
	mach_msg_type_name_t *conn_sockPoly,
	mach_port_t *peer_addr,
	mach_msg_type_name_t *peer_addrPoly
);
#endif

/* Routine socket_connect */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_socket_connect
#if	defined(LINTLIBRARY)
    (sock, addr)
	sock_user_t sock;
	sock_addr_t addr;
{ return S_socket_connect(sock, addr); }
#else
(
	sock_user_t sock,
	sock_addr_t addr
);
#endif

/* Routine socket_bind */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_socket_bind
#if	defined(LINTLIBRARY)
    (sock, addr)
	sock_user_t sock;
	sock_addr_t addr;
{ return S_socket_bind(sock, addr); }
#else
(
	sock_user_t sock,
	sock_addr_t addr
);
#endif

/* Routine socket_name */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_socket_name
#if	defined(LINTLIBRARY)
    (sock, addr, addrPoly)
	sock_user_t sock;
	mach_port_t *addr;
	mach_msg_type_name_t *addrPoly;
{ return S_socket_name(sock, addr, addrPoly); }
#else
(
	sock_user_t sock,
	mach_port_t *addr,
	mach_msg_type_name_t *addrPoly
);
#endif

/* Routine socket_peername */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_socket_peername
#if	defined(LINTLIBRARY)
    (sock, addr, addrPoly)
	sock_user_t sock;
	mach_port_t *addr;
	mach_msg_type_name_t *addrPoly;
{ return S_socket_peername(sock, addr, addrPoly); }
#else
(
	sock_user_t sock,
	mach_port_t *addr,
	mach_msg_type_name_t *addrPoly
);
#endif

/* Routine socket_connect2 */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_socket_connect2
#if	defined(LINTLIBRARY)
    (sock1, sock2)
	sock_user_t sock1;
	sock_user_t sock2;
{ return S_socket_connect2(sock1, sock2); }
#else
(
	sock_user_t sock1,
	sock_user_t sock2
);
#endif

/* Routine socket_create_address */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_socket_create_address
#if	defined(LINTLIBRARY)
    (server, sockaddr_type, sockaddr, sockaddrCnt, addr, addrPoly)
	mach_port_t server;
	int sockaddr_type;
	data_t sockaddr;
	mach_msg_type_number_t sockaddrCnt;
	mach_port_t *addr;
	mach_msg_type_name_t *addrPoly;
{ return S_socket_create_address(server, sockaddr_type, sockaddr, sockaddrCnt, addr, addrPoly); }
#else
(
	mach_port_t server,
	int sockaddr_type,
	data_t sockaddr,
	mach_msg_type_number_t sockaddrCnt,
	mach_port_t *addr,
	mach_msg_type_name_t *addrPoly
);
#endif

/* Routine socket_fabricate_address */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_socket_fabricate_address
#if	defined(LINTLIBRARY)
    (server, sockaddr_type, addr, addrPoly)
	mach_port_t server;
	int sockaddr_type;
	mach_port_t *addr;
	mach_msg_type_name_t *addrPoly;
{ return S_socket_fabricate_address(server, sockaddr_type, addr, addrPoly); }
#else
(
	mach_port_t server,
	int sockaddr_type,
	mach_port_t *addr,
	mach_msg_type_name_t *addrPoly
);
#endif

/* Routine socket_whatis_address */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_socket_whatis_address
#if	defined(LINTLIBRARY)
    (addr, sockaddr_type, sockaddr, sockaddrCnt)
	sock_addr_t addr;
	int *sockaddr_type;
	data_t *sockaddr;
	mach_msg_type_number_t *sockaddrCnt;
{ return S_socket_whatis_address(addr, sockaddr_type, sockaddr, sockaddrCnt); }
#else
(
	sock_addr_t addr,
	int *sockaddr_type,
	data_t *sockaddr,
	mach_msg_type_number_t *sockaddrCnt
);
#endif

/* Routine socket_shutdown */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_socket_shutdown
#if	defined(LINTLIBRARY)
    (sock, direction)
	sock_user_t sock;
	int direction;
{ return S_socket_shutdown(sock, direction); }
#else
(
	sock_user_t sock,
	int direction
);
#endif

/* Routine socket_getopt */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_socket_getopt
#if	defined(LINTLIBRARY)
    (sock, level, option, optval, optvalCnt)
	sock_user_t sock;
	int level;
	int option;
	data_t *optval;
	mach_msg_type_number_t *optvalCnt;
{ return S_socket_getopt(sock, level, option, optval, optvalCnt); }
#else
(
	sock_user_t sock,
	int level,
	int option,
	data_t *optval,
	mach_msg_type_number_t *optvalCnt
);
#endif

/* Routine socket_setopt */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_socket_setopt
#if	defined(LINTLIBRARY)
    (sock, level, option, optval, optvalCnt)
	sock_user_t sock;
	int level;
	int option;
	data_t optval;
	mach_msg_type_number_t optvalCnt;
{ return S_socket_setopt(sock, level, option, optval, optvalCnt); }
#else
(
	sock_user_t sock,
	int level,
	int option,
	data_t optval,
	mach_msg_type_number_t optvalCnt
);
#endif

/* Routine socket_send */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_socket_send
#if	defined(LINTLIBRARY)
    (sock, addr, flags, data, dataCnt, ports, portsCnt, control, controlCnt, amount)
	sock_user_t sock;
	sock_addr_t addr;
	int flags;
	data_t data;
	mach_msg_type_number_t dataCnt;
	portarray_t ports;
	mach_msg_type_number_t portsCnt;
	data_t control;
	mach_msg_type_number_t controlCnt;
	vm_size_t *amount;
{ return S_socket_send(sock, addr, flags, data, dataCnt, ports, portsCnt, control, controlCnt, amount); }
#else
(
	sock_user_t sock,
	sock_addr_t addr,
	int flags,
	data_t data,
	mach_msg_type_number_t dataCnt,
	portarray_t ports,
	mach_msg_type_number_t portsCnt,
	data_t control,
	mach_msg_type_number_t controlCnt,
	vm_size_t *amount
);
#endif

/* Routine socket_recv */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_socket_recv
#if	defined(LINTLIBRARY)
    (sock, addr, addrPoly, flags, data, dataCnt, ports, portsPoly, portsCnt, control, controlCnt, outflags, amount)
	sock_user_t sock;
	mach_port_t *addr;
	mach_msg_type_name_t *addrPoly;
	int flags;
	data_t *data;
	mach_msg_type_number_t *dataCnt;
	portarray_t *ports;
	mach_msg_type_name_t *portsPoly;
	mach_msg_type_number_t *portsCnt;
	data_t *control;
	mach_msg_type_number_t *controlCnt;
	int *outflags;
	vm_size_t amount;
{ return S_socket_recv(sock, addr, addrPoly, flags, data, dataCnt, ports, portsPoly, portsCnt, control, controlCnt, outflags, amount); }
#else
(
	sock_user_t sock,
	mach_port_t *addr,
	mach_msg_type_name_t *addrPoly,
	int flags,
	data_t *data,
	mach_msg_type_number_t *dataCnt,
	portarray_t *ports,
	mach_msg_type_name_t *portsPoly,
	mach_msg_type_number_t *portsCnt,
	data_t *control,
	mach_msg_type_number_t *controlCnt,
	int *outflags,
	vm_size_t amount
);
#endif

#endif	/* not defined(_socket_server_) */
