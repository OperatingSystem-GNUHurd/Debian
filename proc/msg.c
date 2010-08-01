/* Message port manipulations
   Copyright (C) 1994, 1995, 1996, 1999, 2001 Free Software Foundation

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2, or (at
   your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. */

#include <mach.h>
#include <hurd.h>
#include "proc.h"
#include <hurd/startup.h>
#include <assert.h>
#include <stdlib.h>

/* Check to see if process P is blocked trying to get the message
   port of process AVAILP; if so, return its call.  */
void
check_message_return (struct proc *p, void *availpaddr)
{
  if (p->p_msgportwait)
    {
      condition_broadcast (&p->p_wakeup);
      p->p_msgportwait = 0;
    }
}

/* Register ourselves with init. */
static any_t
tickle_init (any_t initport)
{
  startup_essential_task ((mach_port_t) initport, mach_task_self (),
			  MACH_PORT_NULL, "proc", master_host_port);
  return 0;
}

error_t
S_proc_setmsgport (struct proc *p,
		   mach_port_t reply, mach_msg_type_name_t replytype,
		   mach_port_t msgport,
		   mach_port_t *oldmsgport,
		   mach_msg_type_name_t *oldmsgport_type)
{
  if (!p)
    return EOPNOTSUPP;

  *oldmsgport = p->p_msgport;
  *oldmsgport_type = MACH_MSG_TYPE_MOVE_SEND;

  p->p_msgport = msgport;
  p->p_deadmsg = 0;
  if (p->p_checkmsghangs)
    prociterate (check_message_return, p);
  p->p_checkmsghangs = 0;

  if (p == startup_proc)
    /* Init is single threaded, so we can't delay our reply for
       the essential task RPC; spawn a thread to do it. */
    cthread_detach (cthread_fork (tickle_init, (any_t) msgport));
      
  return 0;
}

/* Check to see if process P is blocked trying to get the message port of 
   process DYINGP; if so, wake it up. */
void
check_message_dying (struct proc *p, struct proc *dyingp)
{
  if (p->p_msgportwait)
    {
      condition_broadcast (&p->p_wakeup);
      p->p_msgportwait = 0;
    }
}

/* Check if the message port of process P has died.  Return nonzero if
   this has indeed happened.  */
int
check_msgport_death (struct proc *p)
{
  /* Only check if the message port passed away, if we know that it
     was ever alive.  */
  if (p->p_msgport != MACH_PORT_NULL)
    {
      mach_port_type_t type;
      error_t err;
      
      err = mach_port_type (mach_task_self (), p->p_msgport, &type);
      if (err || (type & MACH_PORT_TYPE_DEAD_NAME))
	{
	  /* The port appears to be dead; throw it away. */
	  mach_port_deallocate (mach_task_self (), p->p_msgport);
	  p->p_msgport = MACH_PORT_NULL;
	  p->p_deadmsg = 1;
	  return 1;
	}
    }

  return 0;
}

error_t
S_proc_getmsgport (struct proc *callerp,
		   mach_port_t reply_port,
		   mach_msg_type_name_t reply_port_type,
		   pid_t pid,
		   mach_port_t *msgport)
{
  int cancel;
  struct proc *p;

  if (!callerp)
    return EOPNOTSUPP;

  p = pid_find_allow_zombie (pid);

restart:  
  while (p && p->p_deadmsg && !p->p_dead)
    {
      callerp->p_msgportwait = 1;
      p->p_checkmsghangs = 1;
      cancel = hurd_condition_wait (&callerp->p_wakeup, &global_lock);
      if (callerp->p_dead)
	return EOPNOTSUPP;
      if (cancel)
	return EINTR;

      /* Refetch P in case it went away while we were waiting.  */
      p = pid_find_allow_zombie (pid);
    }

  if (!p)
    return ESRCH;

  if (check_msgport_death (p))
    goto restart;
  
  *msgport = p->p_msgport;

  return 0;
}
