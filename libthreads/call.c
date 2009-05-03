/* 
 * Mach Operating System
 * Copyright (c) 1992,1991,1990,1989 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log: call.c,v $
 * Revision 1.3  2002/05/27 02:50:10  roland
 * 2002-05-26  Roland McGrath  <roland@frob.com>
 *
 * 	Changes merged from CMU MK83a version:
 * 	* cthreads.h, options.h: Various cleanups.
 * 	* call.c, cthread_data.c, sync.c, mig_support.c: Likewise.
 * 	* i386/cthreads.h, i386/thread.c, i386/lock.s: Likewise.
 * 	* cthread_internals.h: Add decls for internal functions.
 * 	(struct cproc): Use vm_offset_t for stack_base and stack_size members.
 * 	Use natural_t for context member.
 * 	* cprocs.c: Use prototypes for all defns.
 * 	* cthreads.c: Likewise.
 * 	(cthread_exit): Cast any_t to integer_t before int.
 *
 * Revision 2.5  93/01/14  18:04:38  danner
 * 	Converted file to ANSI C.
 * 	[92/12/18            pds]
 * 
 * Revision 2.4  91/05/14  17:56:00  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/14  14:19:20  mrt
 * 	Added new Mach copyright
 * 	[91/02/13  12:40:44  mrt]
 * 
 * Revision 2.2  90/01/19  14:36:50  rwd
 * 	Created.  Routines to replace thread_* and cthread_call_on.
 * 	[90/01/03            rwd]
 * 
 */

#include <cthreads.h>
#include "cthread_internals.h"

#if	defined(THREAD_CALLS)
kern_return_t cthread_get_state(cthread_t thread)
{
	cproc_t	p = thread->ur;
}

kern_return_t cthread_set_state(cthread_t thread)
{
	cproc_t	p = thread->ur;
}

kern_return_t cthread_abort(cthread_t thread)
{
	cproc_t	p = thread->ur;
}

kern_return_t cthread_resume(cthread_t thread)
{
	cproc_t	p = thread->ur;
}

kern_return_t cthread_suspend(cthread_t thread)
{
	cproc_t	p = thread->ur;
}

kern_return_t cthread_call_on(cthread_t thread)
{
	cproc_t	p = thread->ur;
}
#endif	/* defined(THREAD_CALLS) */
