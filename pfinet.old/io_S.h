#ifndef	_io_server_
#define	_io_server_

/* Module io */

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

/* Routine io_write */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_io_write
#if	defined(LINTLIBRARY)
    (io_object, data, dataCnt, offset, amount)
	sock_user_t io_object;
	data_t data;
	mach_msg_type_number_t dataCnt;
	loff_t offset;
	vm_size_t *amount;
{ return S_io_write(io_object, data, dataCnt, offset, amount); }
#else
(
	sock_user_t io_object,
	data_t data,
	mach_msg_type_number_t dataCnt,
	loff_t offset,
	vm_size_t *amount
);
#endif

/* Routine io_read */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_io_read
#if	defined(LINTLIBRARY)
    (io_object, data, dataCnt, offset, amount)
	sock_user_t io_object;
	data_t *data;
	mach_msg_type_number_t *dataCnt;
	loff_t offset;
	vm_size_t amount;
{ return S_io_read(io_object, data, dataCnt, offset, amount); }
#else
(
	sock_user_t io_object,
	data_t *data,
	mach_msg_type_number_t *dataCnt,
	loff_t offset,
	vm_size_t amount
);
#endif

/* Routine io_seek */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_io_seek
#if	defined(LINTLIBRARY)
    (io_object, offset, whence, newp)
	sock_user_t io_object;
	loff_t offset;
	int whence;
	loff_t *newp;
{ return S_io_seek(io_object, offset, whence, newp); }
#else
(
	sock_user_t io_object,
	loff_t offset,
	int whence,
	loff_t *newp
);
#endif

/* Routine io_readable */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_io_readable
#if	defined(LINTLIBRARY)
    (io_object, amount)
	sock_user_t io_object;
	vm_size_t *amount;
{ return S_io_readable(io_object, amount); }
#else
(
	sock_user_t io_object,
	vm_size_t *amount
);
#endif

/* Routine io_set_all_openmodes */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_io_set_all_openmodes
#if	defined(LINTLIBRARY)
    (io_object, newbits)
	sock_user_t io_object;
	int newbits;
{ return S_io_set_all_openmodes(io_object, newbits); }
#else
(
	sock_user_t io_object,
	int newbits
);
#endif

/* Routine io_get_openmodes */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_io_get_openmodes
#if	defined(LINTLIBRARY)
    (io_object, bits)
	sock_user_t io_object;
	int *bits;
{ return S_io_get_openmodes(io_object, bits); }
#else
(
	sock_user_t io_object,
	int *bits
);
#endif

/* Routine io_set_some_openmodes */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_io_set_some_openmodes
#if	defined(LINTLIBRARY)
    (io_object, bits_to_set)
	sock_user_t io_object;
	int bits_to_set;
{ return S_io_set_some_openmodes(io_object, bits_to_set); }
#else
(
	sock_user_t io_object,
	int bits_to_set
);
#endif

/* Routine io_clear_some_openmodes */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_io_clear_some_openmodes
#if	defined(LINTLIBRARY)
    (io_object, bits_to_clear)
	sock_user_t io_object;
	int bits_to_clear;
{ return S_io_clear_some_openmodes(io_object, bits_to_clear); }
#else
(
	sock_user_t io_object,
	int bits_to_clear
);
#endif

/* Routine io_async */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_io_async
#if	defined(LINTLIBRARY)
    (io_object, notify_port, async_id_port, async_id_portPoly)
	sock_user_t io_object;
	mach_port_t notify_port;
	mach_port_t *async_id_port;
	mach_msg_type_name_t *async_id_portPoly;
{ return S_io_async(io_object, notify_port, async_id_port, async_id_portPoly); }
#else
(
	sock_user_t io_object,
	mach_port_t notify_port,
	mach_port_t *async_id_port,
	mach_msg_type_name_t *async_id_portPoly
);
#endif

/* Routine io_mod_owner */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_io_mod_owner
#if	defined(LINTLIBRARY)
    (io_object, owner)
	sock_user_t io_object;
	pid_t owner;
{ return S_io_mod_owner(io_object, owner); }
#else
(
	sock_user_t io_object,
	pid_t owner
);
#endif

/* Routine io_get_owner */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_io_get_owner
#if	defined(LINTLIBRARY)
    (io_object, owner)
	sock_user_t io_object;
	pid_t *owner;
{ return S_io_get_owner(io_object, owner); }
#else
(
	sock_user_t io_object,
	pid_t *owner
);
#endif

/* Routine io_get_icky_async_id */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_io_get_icky_async_id
#if	defined(LINTLIBRARY)
    (io_object, icky_async_id_port, icky_async_id_portPoly)
	sock_user_t io_object;
	mach_port_t *icky_async_id_port;
	mach_msg_type_name_t *icky_async_id_portPoly;
{ return S_io_get_icky_async_id(io_object, icky_async_id_port, icky_async_id_portPoly); }
#else
(
	sock_user_t io_object,
	mach_port_t *icky_async_id_port,
	mach_msg_type_name_t *icky_async_id_portPoly
);
#endif

/* Routine io_select */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_io_select
#if	defined(LINTLIBRARY)
    (io_object, reply, replyPoly, select_type)
	sock_user_t io_object;
	mach_port_t reply;
	mach_msg_type_name_t replyPoly;
	int *select_type;
{ return S_io_select(io_object, reply, replyPoly, select_type); }
#else
(
	sock_user_t io_object,
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	int *select_type
);
#endif

/* Routine io_stat */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_io_stat
#if	defined(LINTLIBRARY)
    (stat_object, stat_info)
	sock_user_t stat_object;
	io_statbuf_t *stat_info;
{ return S_io_stat(stat_object, stat_info); }
#else
(
	sock_user_t stat_object,
	io_statbuf_t *stat_info
);
#endif

/* SimpleRoutine io_reauthenticate */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_io_reauthenticate
#if	defined(LINTLIBRARY)
    (auth_object, rendezvous2)
	sock_user_t auth_object;
	mach_port_t rendezvous2;
{ return S_io_reauthenticate(auth_object, rendezvous2); }
#else
(
	sock_user_t auth_object,
	mach_port_t rendezvous2
);
#endif

/* Routine io_restrict_auth */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_io_restrict_auth
#if	defined(LINTLIBRARY)
    (io_object, new_object, new_objectPoly, uids, uidsCnt, gids, gidsCnt)
	sock_user_t io_object;
	mach_port_t *new_object;
	mach_msg_type_name_t *new_objectPoly;
	idarray_t uids;
	mach_msg_type_number_t uidsCnt;
	idarray_t gids;
	mach_msg_type_number_t gidsCnt;
{ return S_io_restrict_auth(io_object, new_object, new_objectPoly, uids, uidsCnt, gids, gidsCnt); }
#else
(
	sock_user_t io_object,
	mach_port_t *new_object,
	mach_msg_type_name_t *new_objectPoly,
	idarray_t uids,
	mach_msg_type_number_t uidsCnt,
	idarray_t gids,
	mach_msg_type_number_t gidsCnt
);
#endif

/* Routine io_duplicate */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_io_duplicate
#if	defined(LINTLIBRARY)
    (io_object, newport, newportPoly)
	sock_user_t io_object;
	mach_port_t *newport;
	mach_msg_type_name_t *newportPoly;
{ return S_io_duplicate(io_object, newport, newportPoly); }
#else
(
	sock_user_t io_object,
	mach_port_t *newport,
	mach_msg_type_name_t *newportPoly
);
#endif

/* Routine io_server_version */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_io_server_version
#if	defined(LINTLIBRARY)
    (vers_object, server_name, server_major_version, server_minor_version, server_edit_level)
	sock_user_t vers_object;
	string_t server_name;
	int *server_major_version;
	int *server_minor_version;
	int *server_edit_level;
{ return S_io_server_version(vers_object, server_name, server_major_version, server_minor_version, server_edit_level); }
#else
(
	sock_user_t vers_object,
	string_t server_name,
	int *server_major_version,
	int *server_minor_version,
	int *server_edit_level
);
#endif

/* Routine io_map */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_io_map
#if	defined(LINTLIBRARY)
    (io_object, memobjrd, memobjrdPoly, memobjwt, memobjwtPoly)
	sock_user_t io_object;
	mach_port_t *memobjrd;
	mach_msg_type_name_t *memobjrdPoly;
	mach_port_t *memobjwt;
	mach_msg_type_name_t *memobjwtPoly;
{ return S_io_map(io_object, memobjrd, memobjrdPoly, memobjwt, memobjwtPoly); }
#else
(
	sock_user_t io_object,
	mach_port_t *memobjrd,
	mach_msg_type_name_t *memobjrdPoly,
	mach_port_t *memobjwt,
	mach_msg_type_name_t *memobjwtPoly
);
#endif

/* Routine io_map_cntl */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_io_map_cntl
#if	defined(LINTLIBRARY)
    (io_object, memobj, memobjPoly)
	sock_user_t io_object;
	mach_port_t *memobj;
	mach_msg_type_name_t *memobjPoly;
{ return S_io_map_cntl(io_object, memobj, memobjPoly); }
#else
(
	sock_user_t io_object,
	mach_port_t *memobj,
	mach_msg_type_name_t *memobjPoly
);
#endif

/* Routine io_get_conch */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_io_get_conch
#if	defined(LINTLIBRARY)
    (io_object)
	sock_user_t io_object;
{ return S_io_get_conch(io_object); }
#else
(
	sock_user_t io_object
);
#endif

/* Routine io_release_conch */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_io_release_conch
#if	defined(LINTLIBRARY)
    (io_object)
	sock_user_t io_object;
{ return S_io_release_conch(io_object); }
#else
(
	sock_user_t io_object
);
#endif

/* Routine io_eofnotify */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_io_eofnotify
#if	defined(LINTLIBRARY)
    (io_object)
	sock_user_t io_object;
{ return S_io_eofnotify(io_object); }
#else
(
	sock_user_t io_object
);
#endif

/* Routine io_prenotify */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_io_prenotify
#if	defined(LINTLIBRARY)
    (io_object, write_start, write_end)
	sock_user_t io_object;
	vm_offset_t write_start;
	vm_offset_t write_end;
{ return S_io_prenotify(io_object, write_start, write_end); }
#else
(
	sock_user_t io_object,
	vm_offset_t write_start,
	vm_offset_t write_end
);
#endif

/* Routine io_postnotify */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_io_postnotify
#if	defined(LINTLIBRARY)
    (io_object, write_start, write_end)
	sock_user_t io_object;
	vm_offset_t write_start;
	vm_offset_t write_end;
{ return S_io_postnotify(io_object, write_start, write_end); }
#else
(
	sock_user_t io_object,
	vm_offset_t write_start,
	vm_offset_t write_end
);
#endif

/* Routine io_readnotify */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_io_readnotify
#if	defined(LINTLIBRARY)
    (io_object)
	sock_user_t io_object;
{ return S_io_readnotify(io_object); }
#else
(
	sock_user_t io_object
);
#endif

/* Routine io_readsleep */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_io_readsleep
#if	defined(LINTLIBRARY)
    (io_object)
	sock_user_t io_object;
{ return S_io_readsleep(io_object); }
#else
(
	sock_user_t io_object
);
#endif

/* Routine io_sigio */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_io_sigio
#if	defined(LINTLIBRARY)
    (io_object)
	sock_user_t io_object;
{ return S_io_sigio(io_object); }
#else
(
	sock_user_t io_object
);
#endif

/* Routine io_pathconf */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_io_pathconf
#if	defined(LINTLIBRARY)
    (io_object, name, value)
	sock_user_t io_object;
	int name;
	int *value;
{ return S_io_pathconf(io_object, name, value); }
#else
(
	sock_user_t io_object,
	int name,
	int *value
);
#endif

/* Routine io_identity */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_io_identity
#if	defined(LINTLIBRARY)
    (io_object, idport, idportPoly, fsidport, fsidportPoly, fileno)
	sock_user_t io_object;
	mach_port_t *idport;
	mach_msg_type_name_t *idportPoly;
	mach_port_t *fsidport;
	mach_msg_type_name_t *fsidportPoly;
	ino64_t *fileno;
{ return S_io_identity(io_object, idport, idportPoly, fsidport, fsidportPoly, fileno); }
#else
(
	sock_user_t io_object,
	mach_port_t *idport,
	mach_msg_type_name_t *idportPoly,
	mach_port_t *fsidport,
	mach_msg_type_name_t *fsidportPoly,
	ino64_t *fileno
);
#endif

/* Routine io_revoke */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_io_revoke
#if	defined(LINTLIBRARY)
    (io_object)
	sock_user_t io_object;
{ return S_io_revoke(io_object); }
#else
(
	sock_user_t io_object
);
#endif

#endif	/* not defined(_io_server_) */
