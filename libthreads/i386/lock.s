/* 
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
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
 * $Log: lock.s,v $
 * Revision 1.2  2002/05/27 02:50:10  roland
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
 * Revision 2.6  93/05/10  17:51:38  rvb
 * 	Use C Comment
 * 	[93/05/04  18:14:05  rvb]
 * 
 * Revision 2.5  91/05/14  17:57:20  mrt
 * 	Correcting copyright
 * 
 * Revision 2.4  91/05/08  13:36:15  dbg
 * 	Unlock lock with a locked instruction (xchg).
 * 	[91/03/20            dbg]
 * 
 * Revision 2.3  91/02/14  14:20:18  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/13  12:20:06  mrt]
 * 
 * Revision 2.2  90/05/03  15:54:59  dbg
 * 	Created.
 * 	[90/02/05            dbg]
 * 
 */

#include <i386/asm.h>

/*
 * boolean_t spin_try_lock(int *m)
 */
ENTRY(spin_try_lock)
	movl	4(%esp),%ecx		/* point at mutex */
	movl	$1,%eax			/* set locked value in acc */
	xchg	%eax,(%ecx)		/* swap with mutex */
					/* xchg with memory is automatically */
					/* locked */
	xorl	$1,%eax			/* 1 (locked) => FALSE */
					/* 0 (locked) => TRUE */
	ret

/*
 * void spin_unlock(int *m)
 */
ENTRY(spin_unlock)
	movl	4(%esp),%ecx		/* point at mutex */
	xorl	%eax,%eax		/* set unlocked value in acc */
	xchg	%eax,(%ecx)		/* swap with mutex */
					/* xchg with memory is automatically */
					/* locked */
	ret
