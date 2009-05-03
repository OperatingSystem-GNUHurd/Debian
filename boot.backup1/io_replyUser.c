#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#include "io_reply_U.h"
#define EXPORT_BOOLEAN
#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/message.h>
#include <mach/notify.h>
#include <mach/mach_types.h>
#include <mach/mig_errors.h>
#include <mach/mig_support.h>
#include <mach/msg_type.h>
/* LINTLIBRARY */

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
#define msgh_request_port	msgh_remote_port
#define msgh_reply_port		msgh_local_port

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
mig_external kern_return_t io_write_reply
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code,
	vm_size_t amount
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
		mach_msg_type_t amountType;
		vm_size_t amount;
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;


	auto const mach_msg_type_t return_codeType = {
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

	InP->return_codeType = return_codeType;

	InP->return_code = return_code;

	InP->amountType = amountType;

	InP->amount = amount;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(replyPoly, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reply;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 21100;

	return __mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, 40, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
}

/* SimpleRoutine io_read_reply */
mig_external kern_return_t io_read_reply
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code,
	data_t data,
	mach_msg_type_number_t dataCnt
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
		mach_msg_type_long_t dataType;
		char data[2048];
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;

	boolean_t msgh_simple = TRUE;
	unsigned int msgh_size;

	auto const mach_msg_type_t return_codeType = {
		/* msgt_name = */		2,
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

	if (MACH_MSG_TYPE_PORT_ANY(replyPoly))
		msgh_simple = FALSE;

	InP->return_codeType = return_codeType;

	InP->return_code = return_code;

	InP->dataType = dataType;

	if (dataCnt > 2048) {
		InP->dataType.msgtl_header.msgt_inline = FALSE;
		*((char **)InP->data) = data;
		msgh_simple = FALSE;
	}
	else {
		memcpy(InP->data, data, dataCnt);
	}

	InP->dataType.msgtl_number = dataCnt;

	msgh_size = 44 + ((InP->dataType.msgtl_header.msgt_inline) ? (dataCnt + 3) & ~3 : sizeof(char *));
	InP->Head.msgh_bits = msgh_simple ?
		MACH_MSGH_BITS(replyPoly, 0) :
		(MACH_MSGH_BITS_COMPLEX|
		 MACH_MSGH_BITS(replyPoly, 0));
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reply;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 21101;

	return __mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, msgh_size, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
}

/* SimpleRoutine io_seek_reply */
mig_external kern_return_t io_seek_reply
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code,
	loff_t newp
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
		mach_msg_type_t newpType;
		loff_t newp;
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;


	auto const mach_msg_type_t return_codeType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t newpType = {
		/* msgt_name = */		11,
		/* msgt_size = */		64,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->return_codeType = return_codeType;

	InP->return_code = return_code;

	InP->newpType = newpType;

	InP->newp = newp;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(replyPoly, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reply;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 21102;

	return __mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, 44, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
}

/* SimpleRoutine io_readable_reply */
mig_external kern_return_t io_readable_reply
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code,
	vm_size_t amount
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
		mach_msg_type_t amountType;
		vm_size_t amount;
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;


	auto const mach_msg_type_t return_codeType = {
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

	InP->return_codeType = return_codeType;

	InP->return_code = return_code;

	InP->amountType = amountType;

	InP->amount = amount;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(replyPoly, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reply;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 21103;

	return __mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, 40, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
}

/* SimpleRoutine io_set_all_openmodes_reply */
mig_external kern_return_t io_set_all_openmodes_reply
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;


	auto const mach_msg_type_t return_codeType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->return_codeType = return_codeType;

	InP->return_code = return_code;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(replyPoly, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reply;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 21104;

	return __mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, 32, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
}

/* SimpleRoutine io_get_openmodes_reply */
mig_external kern_return_t io_get_openmodes_reply
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code,
	int bits
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
		mach_msg_type_t bitsType;
		int bits;
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;


	auto const mach_msg_type_t return_codeType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t bitsType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->return_codeType = return_codeType;

	InP->return_code = return_code;

	InP->bitsType = bitsType;

	InP->bits = bits;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(replyPoly, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reply;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 21105;

	return __mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, 40, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
}

/* SimpleRoutine io_set_some_openmodes_reply */
mig_external kern_return_t io_set_some_openmodes_reply
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;


	auto const mach_msg_type_t return_codeType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->return_codeType = return_codeType;

	InP->return_code = return_code;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(replyPoly, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reply;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 21106;

	return __mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, 32, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
}

/* SimpleRoutine io_clear_some_openmodes_reply */
mig_external kern_return_t io_clear_some_openmodes_reply
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;


	auto const mach_msg_type_t return_codeType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->return_codeType = return_codeType;

	InP->return_code = return_code;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(replyPoly, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reply;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 21107;

	return __mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, 32, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
}

/* SimpleRoutine io_async_reply */
mig_external kern_return_t io_async_reply
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code,
	mach_port_t async_id_port,
	mach_msg_type_name_t async_id_portPoly
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
		mach_msg_type_t async_id_portType;
		mach_port_t async_id_port;
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;

	boolean_t msgh_simple = TRUE;

	auto const mach_msg_type_t return_codeType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t async_id_portType = {
		/* msgt_name = */		-1,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	if (MACH_MSG_TYPE_PORT_ANY(replyPoly))
		msgh_simple = FALSE;

	InP->return_codeType = return_codeType;

	InP->return_code = return_code;

	InP->async_id_portType = async_id_portType;

	InP->async_id_port = async_id_port;

	if (MACH_MSG_TYPE_PORT_ANY(async_id_portPoly))
		msgh_simple = FALSE;

	InP->async_id_portType.msgt_name = async_id_portPoly;

	InP->Head.msgh_bits = msgh_simple ?
		MACH_MSGH_BITS(replyPoly, 0) :
		(MACH_MSGH_BITS_COMPLEX|
		 MACH_MSGH_BITS(replyPoly, 0));
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reply;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 21108;

	return __mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, 40, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
}

/* SimpleRoutine io_mod_owner_reply */
mig_external kern_return_t io_mod_owner_reply
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;


	auto const mach_msg_type_t return_codeType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->return_codeType = return_codeType;

	InP->return_code = return_code;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(replyPoly, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reply;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 21109;

	return __mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, 32, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
}

/* SimpleRoutine io_get_owner_reply */
mig_external kern_return_t io_get_owner_reply
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code,
	pid_t owner
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
		mach_msg_type_t ownerType;
		pid_t owner;
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;


	auto const mach_msg_type_t return_codeType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t ownerType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->return_codeType = return_codeType;

	InP->return_code = return_code;

	InP->ownerType = ownerType;

	InP->owner = owner;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(replyPoly, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reply;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 21110;

	return __mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, 40, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
}

/* SimpleRoutine io_get_icky_async_id_reply */
mig_external kern_return_t io_get_icky_async_id_reply
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code,
	mach_port_t id_port,
	mach_msg_type_name_t id_portPoly
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
		mach_msg_type_t id_portType;
		mach_port_t id_port;
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;

	boolean_t msgh_simple = TRUE;

	auto const mach_msg_type_t return_codeType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t id_portType = {
		/* msgt_name = */		-1,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	if (MACH_MSG_TYPE_PORT_ANY(replyPoly))
		msgh_simple = FALSE;

	InP->return_codeType = return_codeType;

	InP->return_code = return_code;

	InP->id_portType = id_portType;

	InP->id_port = id_port;

	if (MACH_MSG_TYPE_PORT_ANY(id_portPoly))
		msgh_simple = FALSE;

	InP->id_portType.msgt_name = id_portPoly;

	InP->Head.msgh_bits = msgh_simple ?
		MACH_MSGH_BITS(replyPoly, 0) :
		(MACH_MSGH_BITS_COMPLEX|
		 MACH_MSGH_BITS(replyPoly, 0));
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reply;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 21111;

	return __mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, 40, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
}

/* SimpleRoutine io_select_reply */
mig_external kern_return_t io_select_reply
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code,
	int select_result
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
		mach_msg_type_t select_resultType;
		int select_result;
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;


	auto const mach_msg_type_t return_codeType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t select_resultType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->return_codeType = return_codeType;

	InP->return_code = return_code;

	InP->select_resultType = select_resultType;

	InP->select_result = select_result;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(replyPoly, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reply;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 21112;

	return __mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, 40, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
}

/* SimpleRoutine io_stat_reply */
mig_external kern_return_t io_stat_reply
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code,
	io_statbuf_t stat_info
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
		mach_msg_type_t stat_infoType;
		io_statbuf_t stat_info;
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;


	auto const mach_msg_type_t return_codeType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t stat_infoType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		32,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->return_codeType = return_codeType;

	InP->return_code = return_code;

	InP->stat_infoType = stat_infoType;

	InP->stat_info = stat_info;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(replyPoly, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reply;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 21113;

	return __mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, 164, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
}

/* SimpleRoutine io_restrict_auth_reply */
mig_external kern_return_t io_restrict_auth_reply
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code,
	mach_port_t new_object,
	mach_msg_type_name_t new_objectPoly
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
		mach_msg_type_t new_objectType;
		mach_port_t new_object;
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;

	boolean_t msgh_simple = TRUE;

	auto const mach_msg_type_t return_codeType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t new_objectType = {
		/* msgt_name = */		-1,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	if (MACH_MSG_TYPE_PORT_ANY(replyPoly))
		msgh_simple = FALSE;

	InP->return_codeType = return_codeType;

	InP->return_code = return_code;

	InP->new_objectType = new_objectType;

	InP->new_object = new_object;

	if (MACH_MSG_TYPE_PORT_ANY(new_objectPoly))
		msgh_simple = FALSE;

	InP->new_objectType.msgt_name = new_objectPoly;

	InP->Head.msgh_bits = msgh_simple ?
		MACH_MSGH_BITS(replyPoly, 0) :
		(MACH_MSGH_BITS_COMPLEX|
		 MACH_MSGH_BITS(replyPoly, 0));
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reply;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 21115;

	return __mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, 40, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
}

/* SimpleRoutine io_duplicate_reply */
mig_external kern_return_t io_duplicate_reply
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code,
	mach_port_t newport,
	mach_msg_type_name_t newportPoly
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
		mach_msg_type_t newportType;
		mach_port_t newport;
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;

	boolean_t msgh_simple = TRUE;

	auto const mach_msg_type_t return_codeType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t newportType = {
		/* msgt_name = */		-1,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	if (MACH_MSG_TYPE_PORT_ANY(replyPoly))
		msgh_simple = FALSE;

	InP->return_codeType = return_codeType;

	InP->return_code = return_code;

	InP->newportType = newportType;

	InP->newport = newport;

	if (MACH_MSG_TYPE_PORT_ANY(newportPoly))
		msgh_simple = FALSE;

	InP->newportType.msgt_name = newportPoly;

	InP->Head.msgh_bits = msgh_simple ?
		MACH_MSGH_BITS(replyPoly, 0) :
		(MACH_MSGH_BITS_COMPLEX|
		 MACH_MSGH_BITS(replyPoly, 0));
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reply;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 21116;

	return __mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, 40, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
}

/* SimpleRoutine io_map_reply */
mig_external kern_return_t io_map_reply
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code,
	mach_port_t rdobject,
	mach_msg_type_name_t rdobjectPoly,
	mach_port_t wrobject,
	mach_msg_type_name_t wrobjectPoly
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
		mach_msg_type_t rdobjectType;
		mach_port_t rdobject;
		mach_msg_type_t wrobjectType;
		mach_port_t wrobject;
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;

	boolean_t msgh_simple = TRUE;

	auto const mach_msg_type_t return_codeType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t rdobjectType = {
		/* msgt_name = */		-1,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t wrobjectType = {
		/* msgt_name = */		-1,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	if (MACH_MSG_TYPE_PORT_ANY(replyPoly))
		msgh_simple = FALSE;

	InP->return_codeType = return_codeType;

	InP->return_code = return_code;

	InP->rdobjectType = rdobjectType;

	InP->rdobject = rdobject;

	if (MACH_MSG_TYPE_PORT_ANY(rdobjectPoly))
		msgh_simple = FALSE;

	InP->rdobjectType.msgt_name = rdobjectPoly;

	InP->wrobjectType = wrobjectType;

	InP->wrobject = wrobject;

	if (MACH_MSG_TYPE_PORT_ANY(wrobjectPoly))
		msgh_simple = FALSE;

	InP->wrobjectType.msgt_name = wrobjectPoly;

	InP->Head.msgh_bits = msgh_simple ?
		MACH_MSGH_BITS(replyPoly, 0) :
		(MACH_MSGH_BITS_COMPLEX|
		 MACH_MSGH_BITS(replyPoly, 0));
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reply;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 21117;

	return __mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, 48, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
}

/* SimpleRoutine io_map_cntl_reply */
mig_external kern_return_t io_map_cntl_reply
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code,
	mach_port_t memobj,
	mach_msg_type_name_t memobjPoly
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
		mach_msg_type_t memobjType;
		mach_port_t memobj;
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;

	boolean_t msgh_simple = TRUE;

	auto const mach_msg_type_t return_codeType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t memobjType = {
		/* msgt_name = */		-1,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	if (MACH_MSG_TYPE_PORT_ANY(replyPoly))
		msgh_simple = FALSE;

	InP->return_codeType = return_codeType;

	InP->return_code = return_code;

	InP->memobjType = memobjType;

	InP->memobj = memobj;

	if (MACH_MSG_TYPE_PORT_ANY(memobjPoly))
		msgh_simple = FALSE;

	InP->memobjType.msgt_name = memobjPoly;

	InP->Head.msgh_bits = msgh_simple ?
		MACH_MSGH_BITS(replyPoly, 0) :
		(MACH_MSGH_BITS_COMPLEX|
		 MACH_MSGH_BITS(replyPoly, 0));
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reply;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 21118;

	return __mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, 40, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
}

/* SimpleRoutine io_get_conch_reply */
mig_external kern_return_t io_get_conch_reply
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;


	auto const mach_msg_type_t return_codeType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->return_codeType = return_codeType;

	InP->return_code = return_code;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(replyPoly, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reply;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 21119;

	return __mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, 32, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
}

/* SimpleRoutine io_release_conch_reply */
mig_external kern_return_t io_release_conch_reply
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;


	auto const mach_msg_type_t return_codeType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->return_codeType = return_codeType;

	InP->return_code = return_code;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(replyPoly, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reply;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 21120;

	return __mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, 32, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
}

/* SimpleRoutine io_eofnotify_reply */
mig_external kern_return_t io_eofnotify_reply
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;


	auto const mach_msg_type_t return_codeType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->return_codeType = return_codeType;

	InP->return_code = return_code;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(replyPoly, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reply;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 21121;

	return __mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, 32, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
}

/* SimpleRoutine io_prenotify_reply */
mig_external kern_return_t io_prenotify_reply
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;


	auto const mach_msg_type_t return_codeType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->return_codeType = return_codeType;

	InP->return_code = return_code;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(replyPoly, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reply;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 21122;

	return __mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, 32, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
}

/* SimpleRoutine io_postnotify_reply */
mig_external kern_return_t io_postnotify_reply
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;


	auto const mach_msg_type_t return_codeType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->return_codeType = return_codeType;

	InP->return_code = return_code;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(replyPoly, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reply;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 21123;

	return __mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, 32, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
}

/* SimpleRoutine io_readnotify_reply */
mig_external kern_return_t io_readnotify_reply
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;


	auto const mach_msg_type_t return_codeType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->return_codeType = return_codeType;

	InP->return_code = return_code;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(replyPoly, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reply;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 21124;

	return __mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, 32, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
}

/* SimpleRoutine io_readsleep_reply */
mig_external kern_return_t io_readsleep_reply
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;


	auto const mach_msg_type_t return_codeType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->return_codeType = return_codeType;

	InP->return_code = return_code;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(replyPoly, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reply;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 21125;

	return __mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, 32, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
}

/* SimpleRoutine io_sigio_reply */
mig_external kern_return_t io_sigio_reply
(
	mach_port_t reply,
	mach_msg_type_name_t replyPoly,
	kern_return_t return_code
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;


	auto const mach_msg_type_t return_codeType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->return_codeType = return_codeType;

	InP->return_code = return_code;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(replyPoly, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reply;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 21126;

	return __mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, 32, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
}
