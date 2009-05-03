#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#include "device_reply_U.h"
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
#include <device/device_types.h>
#include <device/net_status.h>

/* SimpleRoutine device_open_reply */
mig_external kern_return_t ds_device_open_reply
(
	mach_port_t reply_port,
	mach_msg_type_name_t reply_portPoly,
	kern_return_t return_code,
	mach_port_t device_port
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
		mach_msg_type_t device_portType;
		mach_port_t device_port;
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

	auto const mach_msg_type_t device_portType = {
		/* msgt_name = */		20,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->return_codeType = return_codeType;

	InP->return_code = return_code;

	InP->device_portType = device_portType;

	InP->device_port = device_port;

	InP->Head.msgh_bits = MACH_MSGH_BITS_COMPLEX|
		MACH_MSGH_BITS(reply_portPoly, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reply_port;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 2900;

	return __mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, 40, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
}

/* SimpleRoutine device_write_reply */
mig_external kern_return_t ds_device_write_reply
(
	mach_port_t reply_port,
	mach_msg_type_name_t reply_portPoly,
	kern_return_t return_code,
	int bytes_written
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
		mach_msg_type_t bytes_writtenType;
		int bytes_written;
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

	auto const mach_msg_type_t bytes_writtenType = {
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

	InP->bytes_writtenType = bytes_writtenType;

	InP->bytes_written = bytes_written;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(reply_portPoly, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reply_port;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 2902;

	return __mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, 40, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
}

/* SimpleRoutine device_write_reply_inband */
mig_external kern_return_t ds_device_write_reply_inband
(
	mach_port_t reply_port,
	mach_msg_type_name_t reply_portPoly,
	kern_return_t return_code,
	int bytes_written
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
		mach_msg_type_t bytes_writtenType;
		int bytes_written;
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

	auto const mach_msg_type_t bytes_writtenType = {
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

	InP->bytes_writtenType = bytes_writtenType;

	InP->bytes_written = bytes_written;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(reply_portPoly, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reply_port;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 2903;

	return __mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, 40, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
}

/* SimpleRoutine device_read_reply */
mig_external kern_return_t ds_device_read_reply
(
	mach_port_t reply_port,
	mach_msg_type_name_t reply_portPoly,
	kern_return_t return_code,
	io_buf_ptr_t data,
	mach_msg_type_number_t dataCnt
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
		mach_msg_type_long_t dataType;
		io_buf_ptr_t data;
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

	auto const mach_msg_type_long_t dataType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		FALSE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		TRUE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	9,
		/* msgtl_size = */	8,
		/* msgtl_number = */	0,
	};

	InP->return_codeType = return_codeType;

	InP->return_code = return_code;

	InP->dataType = dataType;

	InP->data = data;

	InP->dataType.msgtl_number = dataCnt;

	InP->Head.msgh_bits = MACH_MSGH_BITS_COMPLEX|
		MACH_MSGH_BITS(reply_portPoly, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reply_port;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 2904;

	return __mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, 48, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
}

/* SimpleRoutine device_read_reply_inband */
mig_external kern_return_t ds_device_read_reply_inband
(
	mach_port_t reply_port,
	mach_msg_type_name_t reply_portPoly,
	kern_return_t return_code,
	io_buf_ptr_inband_t data,
	mach_msg_type_number_t dataCnt
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
		mach_msg_type_t dataType;
		char data[128];
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;

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

	auto const mach_msg_type_t dataType = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		128,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->return_codeType = return_codeType;

	InP->return_code = return_code;

	InP->dataType = dataType;

	if (dataCnt > 128) {
		return MIG_ARRAY_TOO_LARGE;
	}
	else {
		memcpy(InP->data, data, dataCnt);
	}

	InP->dataType.msgt_number = dataCnt;

	msgh_size = 36 + ((dataCnt + 3) & ~3);
	InP->Head.msgh_bits =
		MACH_MSGH_BITS(reply_portPoly, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reply_port;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 2905;

	return __mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, msgh_size, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
}
