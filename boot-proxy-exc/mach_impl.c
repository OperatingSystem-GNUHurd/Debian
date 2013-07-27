/* 
   Copyright (C) 2009 Free Software Foundation, Inc.
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

/* This file implements the server-side RPC functions of mach_host. */

#include <string.h>
#include <mach.h>
#include <hurd.h>
#include <assert.h>

#include "util.h"
#include "mach_proxy.h"
#include "list.h"

/* Routine task_create */
kern_return_t
S_task_create (mach_port_t target_task, boolean_t inherit_memory,
	       mach_port_t *child_task, mach_msg_type_name_t *child_taskPoly)
{
  struct task_info *target_task_pi;
  task_t real_child_task;
  task_t pseudo_child_task;
  error_t err;

  target_task_pi = ports_lookup_port (port_bucket,
				      target_task, task_portclass);
  if (target_task_pi == NULL)
    return EOPNOTSUPP;
  err = task_create (target_task_pi->task_port,
		     inherit_memory, &real_child_task);
  debug ("get a request from task %d: %s",
	 target_task_pi->task_port, strerror (err));
  ports_port_deref (target_task_pi);
  if (err) 
    return err;

  err = create_pseudo_task (real_child_task, &pseudo_child_task);
  if (err)
    {
      task_terminate (real_child_task);
      return err;
    }
  
  *child_task = pseudo_child_task;
  *child_taskPoly = MACH_MSG_TYPE_COPY_SEND;

  debug ("new task: %d", real_child_task);
  return err;
}

/* Routine task_terminate */
kern_return_t
S_task_terminate (mach_port_t target_task)
{
  struct task_info *target_task_pi;
  error_t err;

  target_task_pi = ports_lookup_port (port_bucket,
				      target_task, task_portclass);
  if (target_task_pi == NULL)
    return EOPNOTSUPP;
  err = task_terminate (target_task_pi->task_port);
  debug ("get request from task %d: %s",
	 target_task_pi->task_port, strerror (err));
  ports_destroy_right (target_task_pi);
  ports_port_deref (target_task_pi);
  return err;
}

/* Routine task_info */
kern_return_t
S_task_info (mach_port_t target_task, int flavor,
	     task_info_t task_info_out,
	     mach_msg_type_number_t *task_info_outCnt)
{
  struct task_info *target_task_pi;
  error_t err;

  target_task_pi = ports_lookup_port (port_bucket,
				      target_task, task_portclass);
  if (target_task_pi == NULL)
    return EOPNOTSUPP;

  err = task_info (target_task_pi->task_port, flavor,
		   task_info_out, task_info_outCnt);
  debug ("%s", strerror (err));
  ports_port_deref (target_task_pi);
  return err;
}

/* Routine vm_allocate */
kern_return_t
S_vm_allocate (mach_port_t target_task, vm_address_t *address,
	       vm_size_t size, boolean_t anywhere)
{
  struct task_info *target_task_pi;
  error_t err;

  target_task_pi = ports_lookup_port (port_bucket,
				      target_task, task_portclass);
  if (target_task_pi == NULL)
    return EOPNOTSUPP;

  /* This should be enough. */
  err = vm_allocate (target_task_pi->task_port, address, size, anywhere);
  debug ("request is from task %d, address: %d, size: %d, anywhere: %d, %s",
	 target_task_pi->task_port, *address, size, anywhere, strerror (err));
  ports_port_deref (target_task_pi);
  return err;
}

/* Routine vm_deallocate */
kern_return_t
S_vm_deallocate (mach_port_t target_task,
		 vm_address_t address, vm_size_t size)
{
  struct task_info *target_task_pi;
  error_t err;

  target_task_pi = ports_lookup_port (port_bucket,
				      target_task, task_portclass);
  if (target_task_pi == NULL)
    return EOPNOTSUPP;
  err = vm_deallocate (target_task_pi->task_port, address, size);
  debug ("get a request from task %d: %s",
	 target_task_pi->task_port, strerror (err));
  ports_port_deref (target_task_pi);
  return err;
}

/* Routine vm_protect */
kern_return_t
S_vm_protect (mach_port_t target_task, vm_address_t address,
	      vm_size_t size, boolean_t set_maximum,
	      vm_prot_t new_protection)
{
  struct task_info *target_task_pi;
  error_t err;

  target_task_pi = ports_lookup_port (port_bucket,
				      target_task, task_portclass);
  if (target_task_pi == NULL)
    return EOPNOTSUPP;
  err = vm_protect (target_task_pi->task_port, address,
		    size, set_maximum, new_protection);
  debug ("request is from task %d: %s",
	 target_task_pi->task_port, strerror (err));
  ports_port_deref (target_task_pi);
  return err;
}

/* Routine vm_inherit */
kern_return_t
S_vm_inherit (mach_port_t target_task, vm_address_t address,
	      vm_size_t size, vm_inherit_t new_inheritance)
{
  struct task_info *target_task_pi;
  error_t err;

  target_task_pi = ports_lookup_port (port_bucket,
				      target_task, task_portclass);
  if (target_task_pi == NULL)
    return EOPNOTSUPP;
  err = vm_inherit (target_task_pi->task_port, address,
		    size, new_inheritance);
  debug ("%s", strerror (err));
  ports_port_deref (target_task_pi);
  return err;
}

/* Routine vm_read */
kern_return_t
S_vm_read (mach_port_t target_task, vm_address_t address,
	   vm_size_t size, vm_offset_t *data,
	   mach_msg_type_number_t *dataCnt)
{
  struct task_info *target_task_pi;
  error_t err;

  target_task_pi = ports_lookup_port (port_bucket,
				      target_task, task_portclass);
  if (target_task_pi == NULL)
    return EOPNOTSUPP;

  err = vm_read (target_task_pi->task_port, address, size, data, dataCnt);
  debug ("get a request from task %d: %s",
	 target_task_pi->task_port, strerror (err));
  ports_port_deref (target_task_pi);
  /* The deallocation bit is set.*/
  return err;
}

/* Routine vm_write */
kern_return_t
S_vm_write (mach_port_t target_task, vm_address_t address,
	    vm_offset_t data, mach_msg_type_number_t dataCnt)
{
  struct task_info *target_task_pi;
  error_t err;

  target_task_pi = ports_lookup_port (port_bucket,
				      target_task, task_portclass);
  if (target_task_pi == NULL)
    return EOPNOTSUPP;
  err = vm_write (target_task_pi->task_port, address, data, dataCnt);
  debug ("get a request from task %d: %s",
	 target_task_pi->task_port, strerror (err));
  ports_port_deref (target_task_pi);
  /* the proxy get the mapped pages from the client,
   * deallocate it here. */
  vm_deallocate (mach_task_self (), data, dataCnt);
  return err;
}

/* Routine vm_copy */
kern_return_t
S_vm_copy (mach_port_t target_task, vm_address_t source_address,
	   vm_size_t size, vm_address_t dest_address)
{
  struct task_info *target_task_pi;
  error_t err;

  target_task_pi = ports_lookup_port (port_bucket,
				      target_task, task_portclass);
  if (target_task_pi == NULL)
    return EOPNOTSUPP;

  err = vm_copy (target_task_pi->task_port, source_address,
		 size, dest_address);
  debug ("%s", strerror (err));
  ports_port_deref (target_task_pi);
  return err;
}

/* Routine vm_region */
kern_return_t
S_vm_region (mach_port_t target_task, vm_address_t *address,
	     vm_size_t *size, vm_prot_t *protection,
	     vm_prot_t *max_protection, vm_inherit_t *inheritance,
	     boolean_t *is_shared, mach_port_t *object_name,
	     vm_offset_t *offset)
{
  struct task_info *target_task_pi;
  error_t err;

  target_task_pi = ports_lookup_port (port_bucket,
				      target_task, task_portclass);
  if (target_task_pi == NULL)
    return EOPNOTSUPP;

  err = vm_region (target_task_pi->task_port, address, size, protection,
		   max_protection, inheritance, is_shared, object_name, offset);
  debug ("get a request from task %d: %s, object: %d",
	 target_task_pi->task_port, strerror (err), *object_name);
  ports_port_deref (target_task_pi);
  return err;
}

/* Routine vm_statistics */
kern_return_t
S_vm_statistics (mach_port_t target_task, vm_statistics_data_t *vm_stats)
{
  struct task_info *target_task_pi;
  error_t err;

  target_task_pi = ports_lookup_port (port_bucket,
				      target_task, task_portclass);
  if (target_task_pi == NULL)
    return EOPNOTSUPP;

  err = vm_statistics (target_task_pi->task_port, vm_stats);
  debug ("request is from task %d: %s",
	 target_task_pi->task_port, strerror (err));
  ports_port_deref (target_task_pi);
  return err;
}

/* Routine mach_ports_register */
kern_return_t
S_mach_ports_register (mach_port_t target_task,
		       mach_port_array_t init_port_set,
		       mach_msg_type_number_t init_port_setCnt)
{
  debug ("");
  assert (0);
  // not used by Hurd
  return EOPNOTSUPP;
}

/* Routine mach_ports_lookup */
kern_return_t
S_mach_ports_lookup (mach_port_t target_task,
		     mach_port_array_t *init_port_set,
		     mach_msg_type_number_t *init_port_setCnt)
{
  debug ("");
  assert (0);
  // not used by Hurd
  return EOPNOTSUPP;
}

/* Routine vm_set_default_memory_manager */
kern_return_t
S_vm_set_default_memory_manager (mach_port_t host_priv,
				 mach_port_t *default_manager)
{
  extern mach_port_t defpager;
  debug ("");
  *default_manager = defpager;
  return 0;
}


/* Routine xxx_task_info */
kern_return_t
S_xxx_task_info (mach_port_t target_task, int flavor,
		 task_info_t task_info_out,
		 mach_msg_type_number_t *task_info_outCnt)
{
  debug ("");
  assert (0);
  return EOPNOTSUPP;
}

/* Routine xxx_host_info */
kern_return_t
S_xxx_host_info (mach_port_t target_task, machine_info_data_t *info)
{
  debug ("");
  assert (0);
  return EOPNOTSUPP;
}

/* Routine xxx_slot_info */
kern_return_t
S_xxx_slot_info (mach_port_t target_task, int slot, machine_slot_data_t *info)
{
  debug ("");
  assert (0);
  return EOPNOTSUPP;
}

/* Routine xxx_cpu_control */
kern_return_t
S_xxx_cpu_control (mach_port_t target_task, int cpu, boolean_t running)
{
  debug ("");
  assert (0);
  return EOPNOTSUPP;
}

/* Routine task_suspend */
kern_return_t
S_task_suspend (mach_port_t target_task)
{
  struct task_info *target_task_pi;
  error_t err;

  target_task_pi = ports_lookup_port (port_bucket,
				      target_task, task_portclass);
  if (target_task_pi == NULL)
    return EOPNOTSUPP;
  err = task_suspend (target_task_pi->task_port);
  debug ("get request from task %d: %s",
	 target_task_pi->task_port, strerror (err));
  ports_port_deref (target_task_pi);
  return err;
}

/* Routine task_resume */
kern_return_t
S_task_resume (mach_port_t target_task)
{
  struct task_info *target_task_pi;
  error_t err;

  target_task_pi = ports_lookup_port (port_bucket,
				      target_task, task_portclass);
  if (target_task_pi == NULL)
    return EOPNOTSUPP;

  err = task_resume (target_task_pi->task_port);
  debug ("get request from task %d: %s",
	 target_task_pi->task_port, strerror (err));
  ports_port_deref (target_task_pi);
  return err;
}

/* Routine task_get_special_port */
kern_return_t
S_task_get_special_port (mach_port_t target_task, int which_port,
			 mach_port_t *special_port,
			 mach_msg_type_name_t *special_portPoly)
{
  struct task_info *target_task_pi;
  error_t err;

  target_task_pi = ports_lookup_port (port_bucket,
				      target_task, task_portclass);
  if (target_task_pi == NULL)
    return EOPNOTSUPP;

  err = task_get_special_port (target_task_pi->task_port,
			       which_port, special_port);
  debug ("request is from task %d (%d, %d): %s\n",
	 target_task_pi->task_port, which_port, *special_port, strerror (err));
  ports_port_deref (target_task_pi);
  if (err)
    return err;
  *special_portPoly = MACH_MSG_TYPE_MOVE_SEND;
  return err;
}

/* Routine task_set_special_port */
kern_return_t
S_task_set_special_port (mach_port_t target_task, int which_port,
			 mach_port_t special_port)
{
  struct task_info *target_task_pi;
  error_t err;

  target_task_pi = ports_lookup_port (port_bucket,
				      target_task, task_portclass);
  if (target_task_pi == NULL)
    return EOPNOTSUPP;

  if (which_port == TASK_EXCEPTION_PORT)
    {
      struct port_info *new_exc_pi;
      struct port_info *old_exc_pi;
      mach_port_t old_user_exc_port;
      mach_port_t new_exc_port;

      err = ports_create_port (exc_portclass, port_bucket,
			       sizeof (new_exc_pi), &new_exc_pi);
      if (err)
	{
	  ports_port_deref (target_task_pi);
	  return err;
	}
      new_exc_port = ports_get_send_right (new_exc_pi);

      pthread_mutex_lock (&target_task_pi->lock);
      old_exc_pi = target_task_pi->exc_pi;
      old_user_exc_port = target_task_pi->user_exc_port;
      err = task_set_special_port (target_task_pi->task_port,
				   which_port, new_exc_port);
      target_task_pi->user_exc_port = special_port;
      target_task_pi->exc_pi = new_exc_pi;
      pthread_mutex_unlock (&target_task_pi->lock);

      ports_port_deref (new_exc_pi);
      if (old_user_exc_port)
	mach_port_deallocate (mach_task_self (), old_user_exc_port);
      if (old_exc_pi)
	ports_destroy_right (old_exc_pi);
      debug ("get request from task %d (%d, %d): %s",
	     target_task_pi->task_port, which_port,
	     special_port, strerror (err));
    }
  else
    {
      err = task_set_special_port (target_task_pi->task_port,
				   which_port, special_port);
      debug ("get request from task %d (%d, %d): %s",
	     target_task_pi->task_port, which_port,
	     special_port, strerror (err));
      mach_port_deallocate (mach_task_self (), special_port);
    }
  ports_port_deref (target_task_pi);
  return err;
}

/* Routine task_ras_control */
kern_return_t
S_task_ras_control (mach_port_t target_task, vm_address_t basepc,
		    vm_address_t boundspc, int flavor)
{
  struct task_info *target_task_pi;
  error_t err;

  target_task_pi = ports_lookup_port (port_bucket,
				      target_task, task_portclass);
  if (target_task_pi == NULL)
    return EOPNOTSUPP;
  err = task_ras_control (target_task_pi->task_port, basepc, boundspc, flavor);
  debug ("%s", strerror (err));
  ports_port_deref (target_task_pi);
  return err;
}

/* Routine vm_map */
kern_return_t
S_vm_map (mach_port_t target_task, vm_address_t *address, vm_size_t size,
	  vm_address_t mask, boolean_t anywhere, mach_port_t memory_object,
	  vm_offset_t offset, boolean_t copy, vm_prot_t cur_protection,
	  vm_prot_t max_protection, vm_inherit_t inheritance)
{
  struct task_info *target_task_pi;
  error_t err;

  target_task_pi = ports_lookup_port (port_bucket,
				      target_task, task_portclass);
  if (target_task_pi == NULL)
    return EOPNOTSUPP;
  err = vm_map (target_task_pi->task_port, address, size, mask,
		anywhere, memory_object, offset, copy,
		cur_protection, max_protection, inheritance);
  debug ("request is from task %d, anywhere: %d: %s",
	 target_task_pi->task_port, anywhere, strerror (err));
  ports_port_deref (target_task_pi);
  mach_port_deallocate (mach_task_self (), memory_object);
  return err;
}

/* Routine vm_machine_attribute */
kern_return_t
S_vm_machine_attribute (mach_port_t target_task, vm_address_t address,
			vm_size_t size, vm_machine_attribute_t attribute,
			vm_machine_attribute_val_t *value)
{
  struct task_info *target_task_pi;
  error_t err;

  target_task_pi = ports_lookup_port (port_bucket,
				      target_task, task_portclass);
  if (target_task_pi == NULL)
    return EOPNOTSUPP;
  err = vm_machine_attribute (target_task_pi->task_port, address,
			      size, attribute, value);
  debug ("%s", strerror (err));
  ports_port_deref (target_task_pi);
  return err;
}

/* Routine task_threads */
kern_return_t
S_task_threads (mach_port_t target_task, thread_array_t *thread_list,
		mach_msg_type_number_t *thread_listCnt)
{
  struct task_info *target_task_pi;
  error_t err;

  target_task_pi = ports_lookup_port (port_bucket,
				      target_task, task_portclass);
  if (target_task_pi == NULL)
    return EOPNOTSUPP;

  // TODO: use move send
  err = task_threads (target_task_pi->task_port,
		      thread_list, thread_listCnt);
  debug ("get a request from task %d: %s",
	 target_task_pi->task_port, strerror (err));
//  info ("task_threads: %s, number of threads: %d",
//	 strerror (err), *thread_listCnt);
  ports_port_deref (target_task_pi);
  return err;
}

/* Routine thread_create */
kern_return_t
S_thread_create (mach_port_t parent_task, mach_port_t *child_thread,
		 mach_msg_type_name_t *child_threadPoly)
{
  struct task_info *target_task_pi;
  error_t err;

  target_task_pi = ports_lookup_port (port_bucket,
				      parent_task, task_portclass);
  if (target_task_pi == NULL)
    return EOPNOTSUPP;
  err = thread_create (target_task_pi->task_port, child_thread);
  debug ("get a request from task %d: thread port: %d: %s",
	 target_task_pi->task_port, *child_thread, strerror (err));
  ports_port_deref (target_task_pi);
  *child_threadPoly = MACH_MSG_TYPE_MOVE_SEND;
  return err;
}

////////////////////it doesn't forward thread request////////////////

/* Routine thread_suspend */
kern_return_t
S_thread_suspend (mach_port_t target_thread)
{
  debug ("");
  assert (0);
  return EOPNOTSUPP;
}

/* Routine thread_resume */
kern_return_t
S_thread_resume (mach_port_t target_thread)
{
  debug ("");
  assert (0);
  return EOPNOTSUPP;
}

/* Routine thread_abort */
kern_return_t
S_thread_abort (mach_port_t target_thread)
{
  debug ("");
  assert (0);
  return EOPNOTSUPP;
}

/* Routine xxx_thread_get_state */
kern_return_t
S_xxx_thread_get_state (mach_port_t target_thread, int flavor,
			thread_state_t old_state,
			mach_msg_type_number_t *old_stateCnt)
{
  debug ("");
  assert (0);
  return EOPNOTSUPP;
}

/* Routine xxx_thread_set_state */
kern_return_t
S_xxx_thread_set_state (mach_port_t target_thread, int flavor,
			thread_state_t new_state,
			mach_msg_type_number_t new_stateCnt)
{
  debug ("");
  assert (0);
  return EOPNOTSUPP;
}

/* Routine xxx_thread_info */
kern_return_t
S_xxx_thread_info (mach_port_t target_thread, int flavor,
		   thread_info_t thread_info_out,
		   mach_msg_type_number_t *thread_info_outCnt)
{
  debug ("");
  assert (0);
  return EOPNOTSUPP;
}

/* Routine thread_get_special_port */
kern_return_t
S_thread_get_special_port (mach_port_t thread, int which_port,
			   mach_port_t *special_port)
{
  debug ("");
  assert (0);
  return EOPNOTSUPP;
}

/* Routine thread_set_special_port */
kern_return_t
S_thread_set_special_port (mach_port_t thread, int which_port,
			   mach_port_t special_port)
{
  debug ("");
  assert (0);
  return EOPNOTSUPP;
}

/* Routine thread_terminate */
kern_return_t
S_thread_terminate (mach_port_t target_thread)
{
  debug ("");
  assert (0);
  return EOPNOTSUPP;
}

/* Routine thread_get_state */
kern_return_t
S_thread_get_state (mach_port_t target_thread, int flavor,
		    thread_state_t old_state,
		    mach_msg_type_number_t *old_stateCnt)
{
  debug ("");
  assert (0);
  return EOPNOTSUPP;
}

/* Routine thread_set_state */
kern_return_t
S_thread_set_state (mach_port_t target_thread, int flavor,
		    thread_state_t new_state,
		    mach_msg_type_number_t new_stateCnt)
{
  debug ("");
  assert (0);
  return EOPNOTSUPP;
}

/* Routine thread_info */
kern_return_t
S_thread_info (mach_port_t target_thread, int flavor,
	       thread_info_t thread_info_out,
	       mach_msg_type_number_t *thread_info_outCnt)
{
  debug ("");
  assert (0);
  return EOPNOTSUPP;
}

/////////////////////RPC related to memory object////////////////////
//////////////////they are not needed to be forwarded////////////////

/* SimpleRoutine memory_object_data_provided */
kern_return_t
S_memory_object_data_provided (mach_port_t memory_control,
			       vm_offset_t offset, vm_offset_t data,
			       mach_msg_type_number_t dataCnt,
			       vm_prot_t lock_value)
{
  debug ("");
  assert (0);
  return EOPNOTSUPP;
}

/* SimpleRoutine memory_object_data_unavailable */
kern_return_t
S_memory_object_data_unavailable (mach_port_t memory_control,
				  vm_offset_t offset, vm_size_t size)
{
  debug ("");
  assert (0);
  return EOPNOTSUPP;
}

/* Routine memory_object_get_attributes */
kern_return_t
S_memory_object_get_attributes (mach_port_t memory_control,
				boolean_t *object_ready, boolean_t *may_cache,
				memory_object_copy_strategy_t *copy_strategy)
{
  debug ("");
  assert (0);
  return EOPNOTSUPP;
}

/* SimpleRoutine xxx_memory_object_lock_request */
kern_return_t
S_xxx_memory_object_lock_request (mach_port_t memory_control,
				  vm_offset_t offset, vm_size_t size,
				  boolean_t should_clean,
				  boolean_t should_flush,
				  vm_prot_t lock_value,
				  mach_port_t reply_to,
				  mach_msg_type_name_t reply_toPoly)
{
  debug ("");
  assert (0);
  return EOPNOTSUPP;
}

/* SimpleRoutine memory_object_lock_request */
kern_return_t
S_memory_object_lock_request ( mach_port_t memory_control,
			      vm_offset_t offset, vm_size_t size,
			      memory_object_return_t should_return,
			      boolean_t should_flush,
			      vm_prot_t lock_value, mach_port_t reply_to,
			      mach_msg_type_name_t reply_toPoly)
{
  debug ("");
  assert (0);
  return EOPNOTSUPP;
}

/* SimpleRoutine memory_object_data_error */
kern_return_t
S_memory_object_data_error (mach_port_t memory_control,
			    vm_offset_t offset, vm_size_t size,
			    kern_return_t error_value)
{
  debug ("");
  assert (0);
  return EOPNOTSUPP;
}

/* SimpleRoutine memory_object_set_attributes */
kern_return_t
S_memory_object_set_attributes (mach_port_t memory_control,
				boolean_t object_ready, boolean_t may_cache,
				memory_object_copy_strategy_t copy_strategy)
{
  debug ("");
  assert (0);
  return EOPNOTSUPP;
}

/* SimpleRoutine memory_object_destroy */
kern_return_t
S_memory_object_destroy (mach_port_t memory_control, kern_return_t reason)
{
  debug ("");
  assert (0);
  return EOPNOTSUPP;
}

/* SimpleRoutine memory_object_data_supply */
kern_return_t
S_memory_object_data_supply (mach_port_t memory_control,
			     vm_offset_t offset, vm_offset_t data,
			     mach_msg_type_number_t dataCnt,
			     vm_prot_t lock_value, boolean_t precious,
			     mach_port_t reply_to,
			     mach_msg_type_name_t reply_toPoly)
{
  debug ("");
  assert (0);
  return EOPNOTSUPP;
}

/* SimpleRoutine memory_object_ready */
kern_return_t
S_memory_object_ready (mach_port_t memory_control, boolean_t may_cache,
		       memory_object_copy_strategy_t copy_strategy)
{
  debug ("");
  assert (0);
  return EOPNOTSUPP;
}

/* SimpleRoutine memory_object_change_attributes */
kern_return_t
S_memory_object_change_attributes (mach_port_t memory_control,
				   boolean_t may_cache,
				   memory_object_copy_strategy_t copy_strategy,
				   mach_port_t reply_to,
				   mach_msg_type_name_t reply_toPoly)
{
  debug ("");
  assert (0);
  return EOPNOTSUPP;
}

///////////////////////RPCs related to emulation/////////////////////
//////////////////////subhurd doesn't support it.////////////////////

/* Routine task_get_emulation_vector */
kern_return_t
S_task_get_emulation_vector (mach_port_t task, int *vector_start,
			     emulation_vector_t *emulation_vector,
			     mach_msg_type_number_t *emulation_vectorCnt)
{
  debug ("");
  assert (0);
  return EOPNOTSUPP;
}

/* Routine task_set_emulation_vector */
kern_return_t
S_task_set_emulation_vector (mach_port_t task, int vector_start,
			     emulation_vector_t emulation_vector,
			     mach_msg_type_number_t emulation_vectorCnt)
{
  debug ("");
  assert (0);
  return EOPNOTSUPP;
}

/* Routine xxx_task_get_emulation_vector */
kern_return_t
S_xxx_task_get_emulation_vector (mach_port_t task, int *vector_start,
				 emulation_vector_t emulation_vector,
				 mach_msg_type_number_t *emulation_vectorCnt)
{
  debug ("");
  assert (0);
  return EOPNOTSUPP;
}

/* Routine xxx_task_set_emulation_vector */
kern_return_t
S_xxx_task_set_emulation_vector (mach_port_t task, int vector_start,
				 emulation_vector_t emulation_vector,
				 mach_msg_type_number_t emulation_vectorCnt)
{
  debug ("");
  assert (0);
  return EOPNOTSUPP;
}

/* Routine task_set_emulation */
kern_return_t
S_task_set_emulation (mach_port_t target_port,
		      vm_address_t routine_entry_pt, int routine_number)
{
  debug ("");
  assert (0);
  return EOPNOTSUPP;
}
