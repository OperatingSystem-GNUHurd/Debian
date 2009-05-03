/*    Copyright (C) 2008 Free Software Foundation, Inc.
      Written by Zheng Da.

      This file is part of the GNU Hurd.

      The GNU Hurd is free software; you can redistribute it and/or modify
      it under the terms of the GNU General Public License as published by
      the Free Software Foundation; either version 2, or (at your option)
      any later version.

      The GNU Hurd is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
      GNU General Public License for more details.

      You should have received a copy of the GNU General Public License
      along with the GNU Hurd; see the file COPYING.  If not, write to
      the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <error.h>

#include "process_S.h"

kern_return_t 
S_proc_getprivports (vpstruct_t process, mach_port_t *host_priv,
		     mach_port_t *device_master)
{
  extern mach_port_t master_port;
  kern_return_t ret = 0;
  debug ("task id: %d\n", process->task_id);
  if (host_priv)
    {
      mach_port_t orig_device_master;
      ret = proc_getprivports (process->proc, host_priv, 
			       &orig_device_master);
      if (ret)
	{
	  error (0, ret, "proc_getprivports");
	  return ret;
	}
      mach_port_deallocate (mach_task_self (), orig_device_master);
    }

  if (device_master)
    *device_master = master_port;
  return ret;
}

kern_return_t 
S_proc_getallpids (vpstruct_t process, pidarray_t *pidarray,
		   mach_msg_type_number_t *pidarrayCnt)
{
  debug ("task id: %d\n", process->task_id);
  return proc_getallpids (getproc (), pidarray, pidarrayCnt);
}

kern_return_t 
S_proc_setexecdata (vpstruct_t process, portarray_t ports,
		    mach_msg_type_number_t portsCnt, intarray_t ints,
		    mach_msg_type_number_t intsCnt)
{
  debug ("task id: %d\n", process->task_id);
  return proc_setexecdata (process->proc, ports, MACH_MSG_TYPE_COPY_SEND,
			   portsCnt, ints, intsCnt);
}

kern_return_t 
S_proc_getexecdata (vpstruct_t process, portarray_t *ports,
		    mach_msg_type_name_t *portsPoly,
		    mach_msg_type_number_t *portsCnt,
		    intarray_t *ints, mach_msg_type_number_t *intsCnt)
{
  kern_return_t ret;
  debug ("task id: %d\n", process->task_id);
  ret = proc_getexecdata (process->proc, ports, portsCnt, ints, intsCnt);
  if (ret == 0)
    *portsPoly = MACH_MSG_TYPE_MOVE_SEND;
  return ret;
}

kern_return_t
S_proc_execdata_notify (vpstruct_t process, mach_port_t notify)
{
  debug ("task id: %d\n", process->task_id);
  return proc_execdata_notify (process->proc, notify, MACH_MSG_TYPE_MOVE_SEND);
}

kern_return_t 
S_proc_uname (vpstruct_t process, utsname_t *uname)
{
  debug ("task id: %d\n", process->task_id);
  return proc_uname (process->proc, uname);
}

kern_return_t 
S_proc_register_version (vpstruct_t process, mach_port_t credential,
			 string_t name, string_t release, string_t version)
{
  debug ("task id: %d\n", process->task_id);
  return proc_register_version (process->proc, credential,
				name, release, version);
}

kern_return_t 
S_proc_reauthenticate (vpstruct_t process, mach_port_t rendezvous2)
{
  debug ("task id: %d\n", process->task_id);
  return proc_reauthenticate (process->proc, rendezvous2,
			      MACH_MSG_TYPE_MOVE_SEND);
}

kern_return_t
S_proc_child (vpstruct_t process, mach_port_t child)
{
  debug ("task id: %d, child task: %d\n", process->task_id, child);
  return proc_child (process->proc, child);
}

kern_return_t
S_proc_setmsgport (vpstruct_t process, mach_port_t reply_port,
		   mach_msg_type_name_t reply_portPoly, mach_port_t newmsgport,
		   mach_port_t *oldmsgport, mach_msg_type_name_t *oldmsgportPoly)
{
  kern_return_t ret=0;
  debug ("task id: %d\n", process->task_id);
  ret = proc_setmsgport (process->proc, newmsgport, oldmsgport);
  if (ret == 0){
      *oldmsgportPoly = MACH_MSG_TYPE_MOVE_SEND;
  }
  return ret;
}

kern_return_t 
S_proc_getmsgport (vpstruct_t process, mach_port_t reply_port,
		   mach_msg_type_name_t reply_portPoly, pid_t pid,
		   mach_port_t *msgport)
{
  debug ("task id: %d\n", process->task_id);
  return proc_getmsgport (getproc(), pid, msgport);
}

kern_return_t 
S_proc_reassign (vpstruct_t process, mach_port_t newtask)
{
  debug ("task id: %d\n", process->task_id);
  return proc_reassign (process->proc, newtask);
}

kern_return_t 
S_proc_setowner (vpstruct_t process, uid_t owner, int clear)
{
  debug ("task id: %d\n", process->task_id);
  return proc_setowner (process->proc, owner, clear);
}

kern_return_t
S_proc_getpids (vpstruct_t process, pid_t *pid,
		pid_t *ppid, int *orphaned)
{
  debug ("task id: %d\n", process->task_id);
  return proc_getpids (process->proc, pid, ppid, orphaned);
}

kern_return_t 
S_proc_set_arg_locations (vpstruct_t process, vm_address_t argv,
			  vm_address_t envp)
{
  kern_return_t ret;
  debug ("task id: %d\n", process->task_id);
  ret = proc_set_arg_locations(process->proc , argv , envp);
  if (ret)
    error (4, ret, "proc_set_arg_locations");
  return ret;
}

kern_return_t 
S_proc_get_arg_locations (vpstruct_t process, vm_address_t *argv,
			  vm_address_t *envp)
{
  debug ("task id: %d\n", process->task_id);
  return proc_get_arg_locations (process->proc, argv, envp);
}

kern_return_t 
S_proc_wait (vpstruct_t process, mach_port_t reply_port,
	     mach_msg_type_name_t reply_portPoly, pid_t pid, int options,
	     int *status, int *sigcode, rusage_t *rusage, pid_t *pid_status)
{
  debug ("task id: %d\n", process->task_id);
  return proc_wait (process->proc, pid, options, status, sigcode, 
		    rusage, pid_status);
}

kern_return_t 
S_proc_dostop (vpstruct_t process, mach_port_t contthread)
{
  kern_return_t ret;

  debug ("task id: %d\n", process->task_id);
  ret = proc_dostop(process->proc, contthread);
  if (ret)
    error (4, ret, "proc_dostop");
  return ret;
}

kern_return_t 
S_proc_handle_exceptions (vpstruct_t process, mach_port_t msgport,
			  mach_port_t forwardport, int flavor,
			  thread_state_t new_state,
			  mach_msg_type_number_t new_stateCnt)
{
  debug ("task id: %d\n", process->task_id);
  return proc_handle_exceptions (process->proc, msgport, forwardport, 
				 MACH_MSG_TYPE_MOVE_SEND, flavor,
				 new_state, new_stateCnt);
}

kern_return_t 
S_proc_mark_stop (vpstruct_t process, int signo, int sigcode)
{
  debug ("task id: %d\n", process->task_id);
  return proc_mark_stop (process->proc, signo, sigcode);
}

kern_return_t 
S_proc_mark_cont (vpstruct_t process)
{
  debug ("task id: %d\n", process->task_id);
  return proc_mark_cont (process->proc);
}

kern_return_t 
S_proc_mark_exit (vpstruct_t process, int status, int sigcode)
{
  debug ("task id: %d\n", process->task_id);
  return proc_mark_exit (process->proc, status, sigcode);
}

kern_return_t 
S_proc_mark_traced (vpstruct_t process)
{
  debug ("task id: %d\n", process->task_id);
  return proc_mark_traced (process->proc);
}

kern_return_t 
S_proc_mark_exec (vpstruct_t process)
{
  debug ("task id: %d\n", process->task_id);
  return proc_mark_exec (process->proc);
}

kern_return_t 
S_proc_mod_stopchild (vpstruct_t process, int doit)
{
  debug ("task id: %d\n", process->task_id);
  return proc_mod_stopchild (process->proc , doit);
}

kern_return_t 
S_proc_pid2task (vpstruct_t process, pid_t pid, mach_port_t *task)
{
  kern_return_t ret;
  ret = proc_pid2task (getproc (), pid, task);
  debug ("pid: %d, task: %d\n", pid, *task);
  return ret;
}

kern_return_t
S_proc_task2pid (vpstruct_t process, mach_port_t task, pid_t *pid)
{
  debug ("task id: %d\n", task);
  return proc_task2pid (getproc (), task, pid);
}

kern_return_t 
S_proc_task2proc (vpstruct_t callerp, mach_port_t task,
		  mach_port_t *proc)
{
  kern_return_t err;
  process_t actual_proc;

  debug ("task id: %d\n", task);
  vpstruct_t process = find_task (task);
  *proc = ports_get_right (process);

  /* Get the actuall port to the proc server. */
  err = proc_task2proc (getproc (), task, &actual_proc);
  if (err)
    error (3, err, "proc_task2proc");
  process->proc = actual_proc;

  /* TODO Do I always need to deallocate task? */
  mach_port_deallocate (mach_task_self (), task);
  return 0;
}

kern_return_t 
S_proc_proc2task (vpstruct_t process, mach_port_t *task)
{
  debug ("task id: %d\n", process->task_id);
  *task = process->task_id;
  return 0;
}

kern_return_t 
S_proc_pid2proc (vpstruct_t process, pid_t pid, mach_port_t *proc)
{
  debug ("pid id: %d\n", pid);
  return proc_pid2proc (getproc (), pid, proc);
}

kern_return_t 
S_proc_getprocinfo (vpstruct_t process, pid_t which, int *flags,
		    procinfo_t *procinfo, mach_msg_type_number_t *procinfoCnt,
		    data_t *threadwaits, mach_msg_type_number_t *threadwaitsCnt)
{
  debug ("pid id: %d\n", which);
  return proc_getprocinfo (getproc (), which, flags, procinfo,
			   procinfoCnt, threadwaits, threadwaitsCnt);
}

kern_return_t 
S_proc_getprocargs (vpstruct_t process, pid_t which,
		    data_t *procargs, mach_msg_type_number_t *procargsCnt)
{
  debug ("pid id: %d\n", which);
  return proc_getprocargs (getproc (), which, procargs, procargsCnt);
}

kern_return_t 
S_proc_getprocenv (vpstruct_t process, pid_t which,
		   data_t *procenv, mach_msg_type_number_t *procenvCnt)
{
  debug ("pid id: %d\n", which);
  return proc_getprocenv (getproc (), which, procenv, procenvCnt);
}

kern_return_t 
S_proc_make_login_coll (vpstruct_t process)
{
  debug ("task id: %d\n", process->task_id);
  return proc_make_login_coll (process->proc);
}

kern_return_t 
S_proc_getloginid (vpstruct_t process, pid_t pid, pid_t *login_id)
{
  debug ("pid id: %d\n", pid);
  return proc_getloginid (getproc (), pid, login_id);
}

kern_return_t 
S_proc_getloginpids (vpstruct_t process, pid_t id,
		     pidarray_t *pids, mach_msg_type_number_t *pidsCnt)
{
  debug ("pid id: %d\n", id);
  return proc_getloginpids (getproc (), id, pids, pidsCnt);
}

kern_return_t 
S_proc_setlogin (vpstruct_t process, string_t logname)
{
  debug ("task id: %d\n", process->task_id);
  return proc_setlogin (process->proc, logname);
}

kern_return_t 
S_proc_getlogin (vpstruct_t process, string_t logname)
{
  debug ("task id: %d\n", process->task_id);
  return proc_getlogin (process->proc, logname);
}

kern_return_t 
S_proc_setsid (vpstruct_t process)
{
  debug ("task id: %d\n", process->task_id);
  return proc_setsid (process->proc);
}

kern_return_t
S_proc_getsid (vpstruct_t process, pid_t pid, pid_t *sid)
{
  debug ("pid id: %d\n", pid);
  return proc_getsid (getproc (), pid, sid);
}

kern_return_t 
S_proc_getsessionpgids (vpstruct_t process, pid_t sid,
			pidarray_t *pgidset, mach_msg_type_number_t *pgidsetCnt)
{
  debug ("pid id: %d\n", sid);
  return proc_getsessionpgids (getproc (), sid, pgidset, pgidsetCnt);
}

kern_return_t 
S_proc_getsessionpids (vpstruct_t process, pid_t sid,
		       pidarray_t *pidset, mach_msg_type_number_t *pidsetCnt)
{
  debug ("pid id: %d\n", sid);
  return proc_getsessionpids (getproc (), sid, pidset, pidsetCnt);
}

kern_return_t 
S_proc_getsidport (vpstruct_t process, mach_port_t *sessport,
		   mach_msg_type_name_t *sessportPoly)
{
  kern_return_t ret;
  debug ("task id: %d\n", process->task_id);
  ret = proc_getsidport (process->proc, sessport);
  if (ret == 0)
    *sessportPoly = MACH_MSG_TYPE_MOVE_SEND;
  return ret;
}

kern_return_t 
S_proc_setpgrp (vpstruct_t process, pid_t pid, pid_t pgrp)
{
  debug ("pid id: %d\n", pid);
  return proc_setpgrp (getproc (), pid, pgrp);
}

kern_return_t 
S_proc_getpgrp (vpstruct_t process, pid_t pid, pid_t *pgrp)
{
  kern_return_t ret;
  debug ("pid id: %d\n", pid);
  ret = proc_getpgrp(getproc(), pid, pgrp);
  if (ret)
    error (0, ret, "proc_proxy: proc_getpgrp");
  return ret;
}

kern_return_t 
S_proc_getpgrppids (vpstruct_t process, pid_t pgrp,
		    pidarray_t *pidset, mach_msg_type_number_t *pidsetCnt)
{
  debug ("pgrp id: %d\n", pgrp);
  return proc_getpgrppids (getproc (), pgrp, pidset, pidsetCnt);
}

kern_return_t 
S_proc_get_tty (vpstruct_t calling_process, pid_t target_process,
		mach_port_t *tty, mach_msg_type_name_t *ttyPoly)
{
  kern_return_t ret;
  debug ("");
  ret = proc_get_tty (getproc (), target_process, tty);
  if (ret == 0)
    *ttyPoly = MACH_MSG_TYPE_MOVE_SEND;
  return ret;
}
