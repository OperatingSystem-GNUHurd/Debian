/* Module io */

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

/* Routine io_write */
mig_internal void _Xio_write
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_long_t dataType;
		char data[2048];
		mach_msg_type_t offsetType;
		loff_t offset;
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
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_io_write
		(sock_user_t io_object, data_t data, mach_msg_type_number_t dataCnt, loff_t offset, vm_size_t *amount);

#if	TypeCheck
	boolean_t msgh_simple;
#endif	/* TypeCheck */

	unsigned int msgh_size;
	unsigned int msgh_size_delta;

	auto const mach_msg_type_t offsetCheck = {
		/* msgt_name = */		11,
		/* msgt_size = */		64,
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

	sock_user_t io_object;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size < 48))
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
	if (msgh_size != 48 + msgh_size_delta)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	In1P = (Request *) ((char *) In0P + msgh_size_delta - 2048);

#if	TypeCheck
	if (BAD_TYPECHECK(&In1P->offsetType, &offsetCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	io_object = begin_using_socket_port(In0P->Head.msgh_request_port);

	OutP->RetCode = S_io_write(io_object, (In0P->dataType.msgtl_header.msgt_inline) ? In0P->data : *((char **)In0P->data), In0P->dataType.msgtl_number, In1P->offset, &OutP->amount);
	if (OutP->RetCode == KERN_SUCCESS)
		if (!In0P->dataType.msgtl_header.msgt_inline)
			__mig_deallocate(* (vm_offset_t *) In0P->data,  In0P->dataType.msgtl_number);
	end_using_socket_port(io_object);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 40;

	OutP->amountType = amountType;
}

/* Routine io_read */
mig_internal void _Xio_read
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t offsetType;
		loff_t offset;
		mach_msg_type_t amountType;
		vm_size_t amount;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_long_t dataType;
		char data[2048];
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_io_read
		(sock_user_t io_object, data_t *data, mach_msg_type_number_t *dataCnt, loff_t offset, vm_size_t amount);

	boolean_t msgh_simple;
	auto const mach_msg_type_t offsetCheck = {
		/* msgt_name = */		11,
		/* msgt_size = */		64,
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

	sock_user_t io_object;
	mach_msg_type_number_t dataCnt;

	char *dataP;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 44) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->offsetType, &offsetCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->amountType, &amountCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	io_object = begin_using_socket_port(In0P->Head.msgh_request_port);

	dataP = OutP->data;
	dataCnt = 2048;

	OutP->RetCode = S_io_read(io_object, &dataP, &dataCnt, In0P->offset, In0P->amount);
	end_using_socket_port(io_object);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;

	OutP->dataType = dataType;
	if (dataP != OutP->data) {
		OutP->dataType.msgtl_header.msgt_inline = FALSE;
		OutP->dataType.msgtl_header.msgt_deallocate = TRUE;
		*((char **)OutP->data) = dataP;
		msgh_simple = FALSE;
	}

	OutP->dataType.msgtl_number = dataCnt;
	OutP->Head.msgh_size = 44 + ((OutP->dataType.msgtl_header.msgt_inline) ? (dataCnt + 3) & ~3 : sizeof(char *));

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
}

/* Routine io_seek */
mig_internal void _Xio_seek
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t offsetType;
		loff_t offset;
		mach_msg_type_t whenceType;
		int whence;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t newpType;
		loff_t newp;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_io_seek
		(sock_user_t io_object, loff_t offset, int whence, loff_t *newp);

	auto const mach_msg_type_t offsetCheck = {
		/* msgt_name = */		11,
		/* msgt_size = */		64,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t whenceCheck = {
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

	sock_user_t io_object;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 44) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->offsetType, &offsetCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->whenceType, &whenceCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	io_object = begin_using_socket_port(In0P->Head.msgh_request_port);

	OutP->RetCode = S_io_seek(io_object, In0P->offset, In0P->whence, &OutP->newp);
	end_using_socket_port(io_object);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 44;

	OutP->newpType = newpType;
}

/* Routine io_readable */
mig_internal void _Xio_readable
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t amountType;
		vm_size_t amount;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_io_readable
		(sock_user_t io_object, vm_size_t *amount);

	auto const mach_msg_type_t amountType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	sock_user_t io_object;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 24) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	io_object = begin_using_socket_port(In0P->Head.msgh_request_port);

	OutP->RetCode = S_io_readable(io_object, &OutP->amount);
	end_using_socket_port(io_object);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 40;

	OutP->amountType = amountType;
}

/* Routine io_set_all_openmodes */
mig_internal void _Xio_set_all_openmodes
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t newbitsType;
		int newbits;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_io_set_all_openmodes
		(sock_user_t io_object, int newbits);

	auto const mach_msg_type_t newbitsCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	sock_user_t io_object;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 32) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->newbitsType, &newbitsCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	io_object = begin_using_socket_port(In0P->Head.msgh_request_port);

	OutP->RetCode = S_io_set_all_openmodes(io_object, In0P->newbits);
	end_using_socket_port(io_object);
}

/* Routine io_get_openmodes */
mig_internal void _Xio_get_openmodes
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t bitsType;
		int bits;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_io_get_openmodes
		(sock_user_t io_object, int *bits);

	auto const mach_msg_type_t bitsType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	sock_user_t io_object;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 24) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	io_object = begin_using_socket_port(In0P->Head.msgh_request_port);

	OutP->RetCode = S_io_get_openmodes(io_object, &OutP->bits);
	end_using_socket_port(io_object);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 40;

	OutP->bitsType = bitsType;
}

/* Routine io_set_some_openmodes */
mig_internal void _Xio_set_some_openmodes
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t bits_to_setType;
		int bits_to_set;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_io_set_some_openmodes
		(sock_user_t io_object, int bits_to_set);

	auto const mach_msg_type_t bits_to_setCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	sock_user_t io_object;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 32) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->bits_to_setType, &bits_to_setCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	io_object = begin_using_socket_port(In0P->Head.msgh_request_port);

	OutP->RetCode = S_io_set_some_openmodes(io_object, In0P->bits_to_set);
	end_using_socket_port(io_object);
}

/* Routine io_clear_some_openmodes */
mig_internal void _Xio_clear_some_openmodes
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t bits_to_clearType;
		int bits_to_clear;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_io_clear_some_openmodes
		(sock_user_t io_object, int bits_to_clear);

	auto const mach_msg_type_t bits_to_clearCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	sock_user_t io_object;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 32) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->bits_to_clearType, &bits_to_clearCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	io_object = begin_using_socket_port(In0P->Head.msgh_request_port);

	OutP->RetCode = S_io_clear_some_openmodes(io_object, In0P->bits_to_clear);
	end_using_socket_port(io_object);
}

/* Routine io_async */
mig_internal void _Xio_async
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t notify_portType;
		mach_port_t notify_port;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t async_id_portType;
		mach_port_t async_id_port;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_io_async
		(sock_user_t io_object, mach_port_t notify_port, mach_port_t *async_id_port, mach_msg_type_name_t *async_id_portPoly);

	boolean_t msgh_simple;
	auto const mach_msg_type_t notify_portCheck = {
		/* msgt_name = */		17,
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

	sock_user_t io_object;
	mach_msg_type_name_t async_id_portPoly;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 32) ||
	    !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->notify_portType, &notify_portCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	io_object = begin_using_socket_port(In0P->Head.msgh_request_port);

	OutP->RetCode = S_io_async(io_object, In0P->notify_port, &OutP->async_id_port, &async_id_portPoly);
	end_using_socket_port(io_object);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	OutP->Head.msgh_size = 40;

	OutP->async_id_portType = async_id_portType;

	if (MACH_MSG_TYPE_PORT_ANY(async_id_portPoly))
		msgh_simple = FALSE;

	OutP->async_id_portType.msgt_name = async_id_portPoly;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
}

/* Routine io_mod_owner */
mig_internal void _Xio_mod_owner
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t ownerType;
		pid_t owner;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_io_mod_owner
		(sock_user_t io_object, pid_t owner);

	auto const mach_msg_type_t ownerCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	sock_user_t io_object;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 32) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->ownerType, &ownerCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	io_object = begin_using_socket_port(In0P->Head.msgh_request_port);

	OutP->RetCode = S_io_mod_owner(io_object, In0P->owner);
	end_using_socket_port(io_object);
}

/* Routine io_get_owner */
mig_internal void _Xio_get_owner
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t ownerType;
		pid_t owner;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_io_get_owner
		(sock_user_t io_object, pid_t *owner);

	auto const mach_msg_type_t ownerType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	sock_user_t io_object;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 24) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	io_object = begin_using_socket_port(In0P->Head.msgh_request_port);

	OutP->RetCode = S_io_get_owner(io_object, &OutP->owner);
	end_using_socket_port(io_object);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 40;

	OutP->ownerType = ownerType;
}

/* Routine io_get_icky_async_id */
mig_internal void _Xio_get_icky_async_id
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t icky_async_id_portType;
		mach_port_t icky_async_id_port;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_io_get_icky_async_id
		(sock_user_t io_object, mach_port_t *icky_async_id_port, mach_msg_type_name_t *icky_async_id_portPoly);

	boolean_t msgh_simple;
	auto const mach_msg_type_t icky_async_id_portType = {
		/* msgt_name = */		-1,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	sock_user_t io_object;
	mach_msg_type_name_t icky_async_id_portPoly;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 24) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	io_object = begin_using_socket_port(In0P->Head.msgh_request_port);

	OutP->RetCode = S_io_get_icky_async_id(io_object, &OutP->icky_async_id_port, &icky_async_id_portPoly);
	end_using_socket_port(io_object);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	OutP->Head.msgh_size = 40;

	OutP->icky_async_id_portType = icky_async_id_portType;

	if (MACH_MSG_TYPE_PORT_ANY(icky_async_id_portPoly))
		msgh_simple = FALSE;

	OutP->icky_async_id_portType.msgt_name = icky_async_id_portPoly;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
}

/* Routine io_select */
mig_internal void _Xio_select
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t select_typeType;
		int select_type;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t select_typeType;
		int select_type;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_io_select
		(sock_user_t io_object, mach_port_t reply, mach_msg_type_name_t replyPoly, int *select_type);

	auto const mach_msg_type_t select_typeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t select_typeType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	sock_user_t io_object;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 32) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->select_typeType, &select_typeCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	io_object = begin_using_socket_port(In0P->Head.msgh_request_port);

	OutP->RetCode = S_io_select(io_object, In0P->Head.msgh_reply_port, MACH_MSGH_BITS_REPLY(In0P->Head.msgh_bits), &In0P->select_type);
	end_using_socket_port(io_object);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 40;

	OutP->select_typeType = select_typeType;

	OutP->select_type = In0P->select_type;
}

/* Routine io_stat */
mig_internal void _Xio_stat
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t stat_infoType;
		io_statbuf_t stat_info;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_io_stat
		(sock_user_t stat_object, io_statbuf_t *stat_info);

	auto const mach_msg_type_t stat_infoType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		32,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	sock_user_t stat_object;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 24) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	stat_object = begin_using_socket_port(In0P->Head.msgh_request_port);

	OutP->RetCode = S_io_stat(stat_object, &OutP->stat_info);
	end_using_socket_port(stat_object);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 164;

	OutP->stat_infoType = stat_infoType;
}

/* SimpleRoutine io_reauthenticate */
mig_internal void _Xio_reauthenticate
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t rendezvous2Type;
		mach_port_t rendezvous2;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_io_reauthenticate
		(sock_user_t auth_object, mach_port_t rendezvous2);

	auto const mach_msg_type_t rendezvous2Check = {
		/* msgt_name = */		17,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	sock_user_t auth_object;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 32) ||
	    !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->rendezvous2Type, &rendezvous2Check))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	auth_object = begin_using_socket_port(In0P->Head.msgh_request_port);

	OutP->RetCode = S_io_reauthenticate(auth_object, In0P->rendezvous2);
	end_using_socket_port(auth_object);
}

/* Routine io_restrict_auth */
mig_internal void _Xio_restrict_auth
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_long_t uidsType;
		uid_t uids[512];
		mach_msg_type_long_t gidsType;
		uid_t gids[512];
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t new_objectType;
		mach_port_t new_object;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Request *In1P;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_io_restrict_auth
		(sock_user_t io_object, mach_port_t *new_object, mach_msg_type_name_t *new_objectPoly, idarray_t uids, mach_msg_type_number_t uidsCnt, idarray_t gids, mach_msg_type_number_t gidsCnt);

	boolean_t msgh_simple;
	unsigned int msgh_size;
	unsigned int msgh_size_delta;

	auto const mach_msg_type_t new_objectType = {
		/* msgt_name = */		-1,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	sock_user_t io_object;
	mach_msg_type_name_t new_objectPoly;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size < 48))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if ((In0P->uidsType.msgtl_header.msgt_longform != TRUE) ||
	    (In0P->uidsType.msgtl_name != 2) ||
	    (In0P->uidsType.msgtl_size != 32))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	msgh_size_delta = (In0P->uidsType.msgtl_header.msgt_inline) ? 4 * In0P->uidsType.msgtl_number : sizeof(uid_t *);
#if	TypeCheck
	if (msgh_size < 48 + msgh_size_delta)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
	msgh_size -= msgh_size_delta;
#endif	/* TypeCheck */

	In1P = (Request *) ((char *) In0P + msgh_size_delta - 2048);

#if	TypeCheck
	if ((In1P->gidsType.msgtl_header.msgt_longform != TRUE) ||
	    (In1P->gidsType.msgtl_name != 2) ||
	    (In1P->gidsType.msgtl_size != 32))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (msgh_size != 48 + ((In1P->gidsType.msgtl_header.msgt_inline) ? 4 * In1P->gidsType.msgtl_number : sizeof(uid_t *)))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	io_object = begin_using_socket_port(In0P->Head.msgh_request_port);

	OutP->RetCode = S_io_restrict_auth(io_object, &OutP->new_object, &new_objectPoly, (In0P->uidsType.msgtl_header.msgt_inline) ? In0P->uids : *((uid_t **)In0P->uids), In0P->uidsType.msgtl_number, (In1P->gidsType.msgtl_header.msgt_inline) ? In1P->gids : *((uid_t **)In1P->gids), In1P->gidsType.msgtl_number);
	if (OutP->RetCode == KERN_SUCCESS)
		if (!In1P->gidsType.msgtl_header.msgt_inline)
			__mig_deallocate(* (vm_offset_t *) In1P->gids, 4 *  In1P->gidsType.msgtl_number);
	if (OutP->RetCode == KERN_SUCCESS)
		if (!In0P->uidsType.msgtl_header.msgt_inline)
			__mig_deallocate(* (vm_offset_t *) In0P->uids, 4 *  In0P->uidsType.msgtl_number);
	end_using_socket_port(io_object);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	OutP->Head.msgh_size = 40;

	OutP->new_objectType = new_objectType;

	if (MACH_MSG_TYPE_PORT_ANY(new_objectPoly))
		msgh_simple = FALSE;

	OutP->new_objectType.msgt_name = new_objectPoly;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
}

/* Routine io_duplicate */
mig_internal void _Xio_duplicate
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t newportType;
		mach_port_t newport;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_io_duplicate
		(sock_user_t io_object, mach_port_t *newport, mach_msg_type_name_t *newportPoly);

	boolean_t msgh_simple;
	auto const mach_msg_type_t newportType = {
		/* msgt_name = */		-1,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	sock_user_t io_object;
	mach_msg_type_name_t newportPoly;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 24) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	io_object = begin_using_socket_port(In0P->Head.msgh_request_port);

	OutP->RetCode = S_io_duplicate(io_object, &OutP->newport, &newportPoly);
	end_using_socket_port(io_object);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	OutP->Head.msgh_size = 40;

	OutP->newportType = newportType;

	if (MACH_MSG_TYPE_PORT_ANY(newportPoly))
		msgh_simple = FALSE;

	OutP->newportType.msgt_name = newportPoly;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
}

/* Routine io_server_version */
mig_internal void _Xio_server_version
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t server_nameType;
		string_t server_name;
		mach_msg_type_t server_major_versionType;
		int server_major_version;
		mach_msg_type_t server_minor_versionType;
		int server_minor_version;
		mach_msg_type_t server_edit_levelType;
		int server_edit_level;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_io_server_version
		(sock_user_t vers_object, string_t server_name, int *server_major_version, int *server_minor_version, int *server_edit_level);

	auto const mach_msg_type_t server_nameType = {
		/* msgt_name = */		MACH_MSG_TYPE_STRING_C,
		/* msgt_size = */		8,
		/* msgt_number = */		1024,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t server_major_versionType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t server_minor_versionType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t server_edit_levelType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	sock_user_t vers_object;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 24) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	vers_object = begin_using_socket_port(In0P->Head.msgh_request_port);

	OutP->RetCode = S_io_server_version(vers_object, OutP->server_name, &OutP->server_major_version, &OutP->server_minor_version, &OutP->server_edit_level);
	end_using_socket_port(vers_object);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 1084;

	OutP->server_nameType = server_nameType;

	OutP->server_major_versionType = server_major_versionType;

	OutP->server_minor_versionType = server_minor_versionType;

	OutP->server_edit_levelType = server_edit_levelType;
}

/* Routine io_map */
mig_internal void _Xio_map
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t memobjrdType;
		mach_port_t memobjrd;
		mach_msg_type_t memobjwtType;
		mach_port_t memobjwt;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_io_map
		(sock_user_t io_object, mach_port_t *memobjrd, mach_msg_type_name_t *memobjrdPoly, mach_port_t *memobjwt, mach_msg_type_name_t *memobjwtPoly);

	boolean_t msgh_simple;
	auto const mach_msg_type_t memobjrdType = {
		/* msgt_name = */		-1,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t memobjwtType = {
		/* msgt_name = */		-1,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	sock_user_t io_object;
	mach_msg_type_name_t memobjrdPoly;
	mach_msg_type_name_t memobjwtPoly;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 24) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	io_object = begin_using_socket_port(In0P->Head.msgh_request_port);

	OutP->RetCode = S_io_map(io_object, &OutP->memobjrd, &memobjrdPoly, &OutP->memobjwt, &memobjwtPoly);
	end_using_socket_port(io_object);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	OutP->Head.msgh_size = 48;

	OutP->memobjrdType = memobjrdType;

	if (MACH_MSG_TYPE_PORT_ANY(memobjrdPoly))
		msgh_simple = FALSE;

	OutP->memobjrdType.msgt_name = memobjrdPoly;

	OutP->memobjwtType = memobjwtType;

	if (MACH_MSG_TYPE_PORT_ANY(memobjwtPoly))
		msgh_simple = FALSE;

	OutP->memobjwtType.msgt_name = memobjwtPoly;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
}

/* Routine io_map_cntl */
mig_internal void _Xio_map_cntl
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t memobjType;
		mach_port_t memobj;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_io_map_cntl
		(sock_user_t io_object, mach_port_t *memobj, mach_msg_type_name_t *memobjPoly);

	boolean_t msgh_simple;
	auto const mach_msg_type_t memobjType = {
		/* msgt_name = */		-1,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	sock_user_t io_object;
	mach_msg_type_name_t memobjPoly;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 24) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	io_object = begin_using_socket_port(In0P->Head.msgh_request_port);

	OutP->RetCode = S_io_map_cntl(io_object, &OutP->memobj, &memobjPoly);
	end_using_socket_port(io_object);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	OutP->Head.msgh_size = 40;

	OutP->memobjType = memobjType;

	if (MACH_MSG_TYPE_PORT_ANY(memobjPoly))
		msgh_simple = FALSE;

	OutP->memobjType.msgt_name = memobjPoly;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
}

/* Routine io_get_conch */
mig_internal void _Xio_get_conch
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_io_get_conch
		(sock_user_t io_object);

	sock_user_t io_object;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 24) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	io_object = begin_using_socket_port(In0P->Head.msgh_request_port);

	OutP->RetCode = S_io_get_conch(io_object);
	end_using_socket_port(io_object);
}

/* Routine io_release_conch */
mig_internal void _Xio_release_conch
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_io_release_conch
		(sock_user_t io_object);

	sock_user_t io_object;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 24) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	io_object = begin_using_socket_port(In0P->Head.msgh_request_port);

	OutP->RetCode = S_io_release_conch(io_object);
	end_using_socket_port(io_object);
}

/* Routine io_eofnotify */
mig_internal void _Xio_eofnotify
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_io_eofnotify
		(sock_user_t io_object);

	sock_user_t io_object;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 24) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	io_object = begin_using_socket_port(In0P->Head.msgh_request_port);

	OutP->RetCode = S_io_eofnotify(io_object);
	end_using_socket_port(io_object);
}

/* Routine io_prenotify */
mig_internal void _Xio_prenotify
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t write_startType;
		vm_offset_t write_start;
		mach_msg_type_t write_endType;
		vm_offset_t write_end;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_io_prenotify
		(sock_user_t io_object, vm_offset_t write_start, vm_offset_t write_end);

	auto const mach_msg_type_t write_startCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t write_endCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	sock_user_t io_object;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 40) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->write_startType, &write_startCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->write_endType, &write_endCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	io_object = begin_using_socket_port(In0P->Head.msgh_request_port);

	OutP->RetCode = S_io_prenotify(io_object, In0P->write_start, In0P->write_end);
	end_using_socket_port(io_object);
}

/* Routine io_postnotify */
mig_internal void _Xio_postnotify
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t write_startType;
		vm_offset_t write_start;
		mach_msg_type_t write_endType;
		vm_offset_t write_end;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_io_postnotify
		(sock_user_t io_object, vm_offset_t write_start, vm_offset_t write_end);

	auto const mach_msg_type_t write_startCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t write_endCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	sock_user_t io_object;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 40) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->write_startType, &write_startCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->write_endType, &write_endCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	io_object = begin_using_socket_port(In0P->Head.msgh_request_port);

	OutP->RetCode = S_io_postnotify(io_object, In0P->write_start, In0P->write_end);
	end_using_socket_port(io_object);
}

/* Routine io_readnotify */
mig_internal void _Xio_readnotify
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_io_readnotify
		(sock_user_t io_object);

	sock_user_t io_object;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 24) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	io_object = begin_using_socket_port(In0P->Head.msgh_request_port);

	OutP->RetCode = S_io_readnotify(io_object);
	end_using_socket_port(io_object);
}

/* Routine io_readsleep */
mig_internal void _Xio_readsleep
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_io_readsleep
		(sock_user_t io_object);

	sock_user_t io_object;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 24) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	io_object = begin_using_socket_port(In0P->Head.msgh_request_port);

	OutP->RetCode = S_io_readsleep(io_object);
	end_using_socket_port(io_object);
}

/* Routine io_sigio */
mig_internal void _Xio_sigio
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_io_sigio
		(sock_user_t io_object);

	sock_user_t io_object;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 24) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	io_object = begin_using_socket_port(In0P->Head.msgh_request_port);

	OutP->RetCode = S_io_sigio(io_object);
	end_using_socket_port(io_object);
}

/* Routine io_pathconf */
mig_internal void _Xio_pathconf
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t nameType;
		int name;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t valueType;
		int value;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_io_pathconf
		(sock_user_t io_object, int name, int *value);

	auto const mach_msg_type_t nameCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t valueType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	sock_user_t io_object;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 32) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (BAD_TYPECHECK(&In0P->nameType, &nameCheck))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	io_object = begin_using_socket_port(In0P->Head.msgh_request_port);

	OutP->RetCode = S_io_pathconf(io_object, In0P->name, &OutP->value);
	end_using_socket_port(io_object);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 40;

	OutP->valueType = valueType;
}

/* Routine io_identity */
mig_internal void _Xio_identity
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t idportType;
		mach_port_t idport;
		mach_msg_type_t fsidportType;
		mach_port_t fsidport;
		mach_msg_type_t filenoType;
		ino64_t fileno;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_io_identity
		(sock_user_t io_object, mach_port_t *idport, mach_msg_type_name_t *idportPoly, mach_port_t *fsidport, mach_msg_type_name_t *fsidportPoly, ino64_t *fileno);

	boolean_t msgh_simple;
	auto const mach_msg_type_t idportType = {
		/* msgt_name = */		-1,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t fsidportType = {
		/* msgt_name = */		-1,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	auto const mach_msg_type_t filenoType = {
		/* msgt_name = */		11,
		/* msgt_size = */		64,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	sock_user_t io_object;
	mach_msg_type_name_t idportPoly;
	mach_msg_type_name_t fsidportPoly;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 24) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	io_object = begin_using_socket_port(In0P->Head.msgh_request_port);

	OutP->RetCode = S_io_identity(io_object, &OutP->idport, &idportPoly, &OutP->fsidport, &fsidportPoly, &OutP->fileno);
	end_using_socket_port(io_object);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	OutP->Head.msgh_size = 60;

	OutP->idportType = idportType;

	if (MACH_MSG_TYPE_PORT_ANY(idportPoly))
		msgh_simple = FALSE;

	OutP->idportType.msgt_name = idportPoly;

	OutP->fsidportType = fsidportType;

	if (MACH_MSG_TYPE_PORT_ANY(fsidportPoly))
		msgh_simple = FALSE;

	OutP->fsidportType.msgt_name = fsidportPoly;

	OutP->filenoType = filenoType;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
}

/* Routine io_revoke */
mig_internal void _Xio_revoke
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t S_io_revoke
		(sock_user_t io_object);

	sock_user_t io_object;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 24) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	io_object = begin_using_socket_port(In0P->Head.msgh_request_port);

	OutP->RetCode = S_io_revoke(io_object);
	end_using_socket_port(io_object);
}

static mig_routine_t io_server_routines[] = {
		_Xio_write,
		_Xio_read,
		_Xio_seek,
		_Xio_readable,
		_Xio_set_all_openmodes,
		_Xio_get_openmodes,
		_Xio_set_some_openmodes,
		_Xio_clear_some_openmodes,
		_Xio_async,
		_Xio_mod_owner,
		_Xio_get_owner,
		_Xio_get_icky_async_id,
		_Xio_select,
		_Xio_stat,
		_Xio_reauthenticate,
		_Xio_restrict_auth,
		_Xio_duplicate,
		_Xio_server_version,
		_Xio_map,
		_Xio_map_cntl,
		_Xio_get_conch,
		_Xio_release_conch,
		_Xio_eofnotify,
		_Xio_prenotify,
		_Xio_postnotify,
		_Xio_readnotify,
		_Xio_readsleep,
		_Xio_sigio,
		_Xio_pathconf,
		_Xio_identity,
		_Xio_revoke,
};

mig_external boolean_t io_server
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

	if ((InP->msgh_id > 21030) || (InP->msgh_id < 21000) ||
	    ((routine = io_server_routines[InP->msgh_id - 21000]) == 0)) {
		OutP->RetCode = MIG_BAD_ID;
		return FALSE;
	}
	(*routine) (InP, &OutP->Head);
	return TRUE;
}

mig_external mig_routine_t io_server_routine
	(const mach_msg_header_t *InHeadP)
{
	register int msgh_id;

	msgh_id = InHeadP->msgh_id - 21000;

	if ((msgh_id > 30) || (msgh_id < 0))
		return 0;

	return io_server_routines[msgh_id];
}

