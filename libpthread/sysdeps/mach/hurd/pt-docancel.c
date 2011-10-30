/* Cancel a thread.
   Copyright (C) 2002, 2007, 2008 Free Software Foundation, Inc.
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

#include <pthread.h>

#include <pt-internal.h>

static void
call_exit (void)
{
  pthread_exit (0);
}

int
__pthread_do_cancel (struct __pthread *p)
{
  mach_port_t ktid;
  int me;

  assert (p->cancel_pending == 1);
  assert (p->cancel_state == PTHREAD_CANCEL_ENABLE);

  ktid = __mach_thread_self ();
  me = p->kernel_thread == ktid;
  __mach_port_deallocate (__mach_task_self (), ktid);

  if (me)
    call_exit ();
  else
    {
      error_t err;
      
      err = __thread_suspend (p->kernel_thread);
      assert_perror (err);

      err = __thread_abort (p->kernel_thread);
      assert_perror (err);

      err = __thread_set_pcsptp (p->kernel_thread,
			       1, (void *) call_exit, 0, 0, 0, 0);
      assert_perror (err);

      err = __thread_resume (p->kernel_thread);
      assert_perror (err);
    }

  return 0;
}
