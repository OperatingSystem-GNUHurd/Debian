#ifndef	_io_reply_user_
#define	_io_reply_user_

/* Module io_reply */

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

/* SimpleRoutine io_write_reply */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t io_write_reply
#if	defined(LINTLIBRARY)
    (reply, replyPoly, return_code, amount)
	mach_port_t reply;
	mach_msg_type_name_t replyPoly;
	kern_return_t return_code;
	vm_size_t amount;
{ return io_write_reply(reply, replyPoly, return_code, amount); }
#else
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code,
	vm_size_t amount
);
#endif

/* SimpleRoutine io_read_reply */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t io_read_reply
#if	defined(LINTLIBRARY)
    (reply, replyPoly, return_code, data, dataCnt)
	mach_port_t reply;
	mach_msg_type_name_t replyPoly;
	kern_return_t return_code;
	data_t data;
	mach_msg_type_number_t dataCnt;
{ return io_read_reply(reply, replyPoly, return_code, data, dataCnt); }
#else
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code,
	data_t data,
	mach_msg_type_number_t dataCnt
);
#endif

/* SimpleRoutine io_seek_reply */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t io_seek_reply
#if	defined(LINTLIBRARY)
    (reply, replyPoly, return_code, newp)
	mach_port_t reply;
	mach_msg_type_name_t replyPoly;
	kern_return_t return_code;
	loff_t newp;
{ return io_seek_reply(reply, replyPoly, return_code, newp); }
#else
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code,
	loff_t newp
);
#endif

/* SimpleRoutine io_readable_reply */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t io_readable_reply
#if	defined(LINTLIBRARY)
    (reply, replyPoly, return_code, amount)
	mach_port_t reply;
	mach_msg_type_name_t replyPoly;
	kern_return_t return_code;
	vm_size_t amount;
{ return io_readable_reply(reply, replyPoly, return_code, amount); }
#else
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code,
	vm_size_t amount
);
#endif

/* SimpleRoutine io_set_all_openmodes_reply */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t io_set_all_openmodes_reply
#if	defined(LINTLIBRARY)
    (reply, replyPoly, return_code)
	mach_port_t reply;
	mach_msg_type_name_t replyPoly;
	kern_return_t return_code;
{ return io_set_all_openmodes_reply(reply, replyPoly, return_code); }
#else
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code
);
#endif

/* SimpleRoutine io_get_openmodes_reply */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t io_get_openmodes_reply
#if	defined(LINTLIBRARY)
    (reply, replyPoly, return_code, bits)
	mach_port_t reply;
	mach_msg_type_name_t replyPoly;
	kern_return_t return_code;
	int bits;
{ return io_get_openmodes_reply(reply, replyPoly, return_code, bits); }
#else
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code,
	int bits
);
#endif

/* SimpleRoutine io_set_some_openmodes_reply */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t io_set_some_openmodes_reply
#if	defined(LINTLIBRARY)
    (reply, replyPoly, return_code)
	mach_port_t reply;
	mach_msg_type_name_t replyPoly;
	kern_return_t return_code;
{ return io_set_some_openmodes_reply(reply, replyPoly, return_code); }
#else
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code
);
#endif

/* SimpleRoutine io_clear_some_openmodes_reply */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t io_clear_some_openmodes_reply
#if	defined(LINTLIBRARY)
    (reply, replyPoly, return_code)
	mach_port_t reply;
	mach_msg_type_name_t replyPoly;
	kern_return_t return_code;
{ return io_clear_some_openmodes_reply(reply, replyPoly, return_code); }
#else
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code
);
#endif

/* SimpleRoutine io_async_reply */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t io_async_reply
#if	defined(LINTLIBRARY)
    (reply, replyPoly, return_code, async_id_port, async_id_portPoly)
	mach_port_t reply;
	mach_msg_type_name_t replyPoly;
	kern_return_t return_code;
	mach_port_t async_id_port;
	mach_msg_type_name_t async_id_portPoly;
{ return io_async_reply(reply, replyPoly, return_code, async_id_port, async_id_portPoly); }
#else
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code,
	mach_port_t async_id_port,
	mach_msg_type_name_t async_id_portPoly
);
#endif

/* SimpleRoutine io_mod_owner_reply */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t io_mod_owner_reply
#if	defined(LINTLIBRARY)
    (reply, replyPoly, return_code)
	mach_port_t reply;
	mach_msg_type_name_t replyPoly;
	kern_return_t return_code;
{ return io_mod_owner_reply(reply, replyPoly, return_code); }
#else
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code
);
#endif

/* SimpleRoutine io_get_owner_reply */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t io_get_owner_reply
#if	defined(LINTLIBRARY)
    (reply, replyPoly, return_code, owner)
	mach_port_t reply;
	mach_msg_type_name_t replyPoly;
	kern_return_t return_code;
	pid_t owner;
{ return io_get_owner_reply(reply, replyPoly, return_code, owner); }
#else
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code,
	pid_t owner
);
#endif

/* SimpleRoutine io_get_icky_async_id_reply */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t io_get_icky_async_id_reply
#if	defined(LINTLIBRARY)
    (reply, replyPoly, return_code, id_port, id_portPoly)
	mach_port_t reply;
	mach_msg_type_name_t replyPoly;
	kern_return_t return_code;
	mach_port_t id_port;
	mach_msg_type_name_t id_portPoly;
{ return io_get_icky_async_id_reply(reply, replyPoly, return_code, id_port, id_portPoly); }
#else
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code,
	mach_port_t id_port,
	mach_msg_type_name_t id_portPoly
);
#endif

/* SimpleRoutine io_select_reply */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t io_select_reply
#if	defined(LINTLIBRARY)
    (reply, replyPoly, return_code, select_result)
	mach_port_t reply;
	mach_msg_type_name_t replyPoly;
	kern_return_t return_code;
	int select_result;
{ return io_select_reply(reply, replyPoly, return_code, select_result); }
#else
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code,
	int select_result
);
#endif

/* SimpleRoutine io_stat_reply */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t io_stat_reply
#if	defined(LINTLIBRARY)
    (reply, replyPoly, return_code, stat_info)
	mach_port_t reply;
	mach_msg_type_name_t replyPoly;
	kern_return_t return_code;
	io_statbuf_t stat_info;
{ return io_stat_reply(reply, replyPoly, return_code, stat_info); }
#else
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code,
	io_statbuf_t stat_info
);
#endif

/* SimpleRoutine io_restrict_auth_reply */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t io_restrict_auth_reply
#if	defined(LINTLIBRARY)
    (reply, replyPoly, return_code, new_object, new_objectPoly)
	mach_port_t reply;
	mach_msg_type_name_t replyPoly;
	kern_return_t return_code;
	mach_port_t new_object;
	mach_msg_type_name_t new_objectPoly;
{ return io_restrict_auth_reply(reply, replyPoly, return_code, new_object, new_objectPoly); }
#else
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code,
	mach_port_t new_object,
	mach_msg_type_name_t new_objectPoly
);
#endif

/* SimpleRoutine io_duplicate_reply */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t io_duplicate_reply
#if	defined(LINTLIBRARY)
    (reply, replyPoly, return_code, newport, newportPoly)
	mach_port_t reply;
	mach_msg_type_name_t replyPoly;
	kern_return_t return_code;
	mach_port_t newport;
	mach_msg_type_name_t newportPoly;
{ return io_duplicate_reply(reply, replyPoly, return_code, newport, newportPoly); }
#else
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code,
	mach_port_t newport,
	mach_msg_type_name_t newportPoly
);
#endif

/* SimpleRoutine io_map_reply */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t io_map_reply
#if	defined(LINTLIBRARY)
    (reply, replyPoly, return_code, rdobject, rdobjectPoly, wrobject, wrobjectPoly)
	mach_port_t reply;
	mach_msg_type_name_t replyPoly;
	kern_return_t return_code;
	mach_port_t rdobject;
	mach_msg_type_name_t rdobjectPoly;
	mach_port_t wrobject;
	mach_msg_type_name_t wrobjectPoly;
{ return io_map_reply(reply, replyPoly, return_code, rdobject, rdobjectPoly, wrobject, wrobjectPoly); }
#else
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code,
	mach_port_t rdobject,
	mach_msg_type_name_t rdobjectPoly,
	mach_port_t wrobject,
	mach_msg_type_name_t wrobjectPoly
);
#endif

/* SimpleRoutine io_map_cntl_reply */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t io_map_cntl_reply
#if	defined(LINTLIBRARY)
    (reply, replyPoly, return_code, memobj, memobjPoly)
	mach_port_t reply;
	mach_msg_type_name_t replyPoly;
	kern_return_t return_code;
	mach_port_t memobj;
	mach_msg_type_name_t memobjPoly;
{ return io_map_cntl_reply(reply, replyPoly, return_code, memobj, memobjPoly); }
#else
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code,
	mach_port_t memobj,
	mach_msg_type_name_t memobjPoly
);
#endif

/* SimpleRoutine io_get_conch_reply */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t io_get_conch_reply
#if	defined(LINTLIBRARY)
    (reply, replyPoly, return_code)
	mach_port_t reply;
	mach_msg_type_name_t replyPoly;
	kern_return_t return_code;
{ return io_get_conch_reply(reply, replyPoly, return_code); }
#else
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code
);
#endif

/* SimpleRoutine io_release_conch_reply */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t io_release_conch_reply
#if	defined(LINTLIBRARY)
    (reply, replyPoly, return_code)
	mach_port_t reply;
	mach_msg_type_name_t replyPoly;
	kern_return_t return_code;
{ return io_release_conch_reply(reply, replyPoly, return_code); }
#else
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code
);
#endif

/* SimpleRoutine io_eofnotify_reply */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t io_eofnotify_reply
#if	defined(LINTLIBRARY)
    (reply, replyPoly, return_code)
	mach_port_t reply;
	mach_msg_type_name_t replyPoly;
	kern_return_t return_code;
{ return io_eofnotify_reply(reply, replyPoly, return_code); }
#else
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code
);
#endif

/* SimpleRoutine io_prenotify_reply */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t io_prenotify_reply
#if	defined(LINTLIBRARY)
    (reply, replyPoly, return_code)
	mach_port_t reply;
	mach_msg_type_name_t replyPoly;
	kern_return_t return_code;
{ return io_prenotify_reply(reply, replyPoly, return_code); }
#else
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code
);
#endif

/* SimpleRoutine io_postnotify_reply */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t io_postnotify_reply
#if	defined(LINTLIBRARY)
    (reply, replyPoly, return_code)
	mach_port_t reply;
	mach_msg_type_name_t replyPoly;
	kern_return_t return_code;
{ return io_postnotify_reply(reply, replyPoly, return_code); }
#else
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code
);
#endif

/* SimpleRoutine io_readnotify_reply */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t io_readnotify_reply
#if	defined(LINTLIBRARY)
    (reply, replyPoly, return_code)
	mach_port_t reply;
	mach_msg_type_name_t replyPoly;
	kern_return_t return_code;
{ return io_readnotify_reply(reply, replyPoly, return_code); }
#else
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code
);
#endif

/* SimpleRoutine io_readsleep_reply */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t io_readsleep_reply
#if	defined(LINTLIBRARY)
    (reply, replyPoly, return_code)
	mach_port_t reply;
	mach_msg_type_name_t replyPoly;
	kern_return_t return_code;
{ return io_readsleep_reply(reply, replyPoly, return_code); }
#else
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code
);
#endif

/* SimpleRoutine io_sigio_reply */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t io_sigio_reply
#if	defined(LINTLIBRARY)
    (reply, replyPoly, return_code)
	mach_port_t reply;
	mach_msg_type_name_t replyPoly;
	kern_return_t return_code;
{ return io_sigio_reply(reply, replyPoly, return_code); }
#else
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code
);
#endif

#endif	/* not defined(_io_reply_user_) */
