/* Definitions of user-visible names for spin locks.
   Copyright (C) 1994, 1997, 2002 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#ifndef _BITS_SPIN_LOCK_H
#define _BITS_SPIN_LOCK_H	1

#include <features.h>
#include <machine-lock.h>	/* This does all the work.  */

__BEGIN_DECLS

/* The type of a spin lock object.  */
typedef __spin_lock_t __pthread_spinlock_t;

/* Initializer for a spin lock object.  */
#ifndef __SPIN_LOCK_INITIALIZER
#error __SPIN_LOCK_INITIALIZER undefined: should be defined by <lock-intern.h>.
#endif

#if defined __USE_EXTERN_INLINES || defined _FORCE_INLINES

# ifndef __EBUSY
#  include <errno.h>
#  define __EBUSY EBUSY
# endif

# ifndef __PT_SPIN_INLINE
#  define __PT_SPIN_INLINE extern __inline
# endif

__PT_SPIN_INLINE int __pthread_spin_destroy (__pthread_spinlock_t *__lock);

__PT_SPIN_INLINE int
__pthread_spin_destroy (__pthread_spinlock_t *__lock)
{
  return 0;
}

__PT_SPIN_INLINE int __pthread_spin_init (__pthread_spinlock_t *__lock,
					  int __pshared);

__PT_SPIN_INLINE int
__pthread_spin_init (__pthread_spinlock_t *__lock, int __pshared)
{
  *__lock = __SPIN_LOCK_INITIALIZER;
  return 0;
}

__PT_SPIN_INLINE int __pthread_spin_trylock (__pthread_spinlock_t *__lock);

__PT_SPIN_INLINE int
__pthread_spin_trylock (__pthread_spinlock_t *__lock)
{
  return __spin_try_lock (__lock) ? 0 : __EBUSY;
}

extern __inline int __pthread_spin_lock (__pthread_spinlock_t *__lock);
extern int _pthread_spin_lock (__pthread_spinlock_t *__lock);

extern __inline int
__pthread_spin_lock (__pthread_spinlock_t *__lock)
{
  if (__pthread_spin_trylock (__lock))
    return _pthread_spin_lock (__lock);
  return 0;
}

__PT_SPIN_INLINE int __pthread_spin_unlock (__pthread_spinlock_t *__lock);

__PT_SPIN_INLINE int
__pthread_spin_unlock (__pthread_spinlock_t *__lock)
{
  __spin_unlock (__lock);
  return 0;
}

#endif /* Use extern inlines or force inlines.  */

__END_DECLS

#endif /* bits/spin-lock.h */