/* Internal defenitions for pthreads library.
   Copyright (C) 2000 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#ifndef _PT_SYSDEP_H
#define _PT_SYSDEP_H	1

#include <mach.h>

#include <hurd/threadvar.h>

/* XXX */
#define _POSIX_THREAD_THREADS_MAX	64

/* The default stack size.  */
#define PTHREAD_STACK_DEFAULT	(2 * 1024 * 1024)

#define PTHREAD_SYSDEP_MEMBERS \
  thread_t kernel_thread;      \
  mach_msg_header_t wakeupmsg;

#define _HURD_THREADVAR_THREAD _HURD_THREADVAR_DYNAMIC_USER

#define _pthread_self()                                            \
	({                                                         \
	  struct __pthread *thread;                                \
	                                                           \
	  assert (__pthread_threads);                              \
	  thread = *(struct __pthread **)                          \
	    __hurd_threadvar_location (_HURD_THREADVAR_THREAD);    \
	                                                           \
	  assert (thread);                                         \
	  assert (thread->kernel_thread == __mach_thread_self ()); \
          thread;                                                  \
         })

extern inline void
__pthread_stack_dealloc (void *stackaddr, size_t stacksize)
{
  __vm_deallocate (__mach_task_self (), (vm_offset_t) stackaddr, stacksize);
}

/* Change thread THREAD's program counter to PC if SET_PC is true and
   its stack pointer to SP if SET_IP is true.  */
extern int __thread_set_pcsp (thread_t thread,
			      int set_pc, void *pc,
			      int set_sp, void *sp);


#endif /* pt-sysdep.h */
