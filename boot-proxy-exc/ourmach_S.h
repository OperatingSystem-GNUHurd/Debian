#ifndef	_mach_server_
#define	_mach_server_

/* Module mach */

#include <mach/kern_return.h>
#include <mach/port.h>
#include <mach/message.h>

#include <mach/std_types.h>
#include <mach/mach_types.h>

/* Routine task_create */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_task_create
#if	defined(LINTLIBRARY)
    (target_task, inherit_memory, child_task, child_taskPoly)
	mach_port_t target_task;
	boolean_t inherit_memory;
	mach_port_t *child_task;
	mach_msg_type_name_t *child_taskPoly;
{ return S_task_create(target_task, inherit_memory, child_task, child_taskPoly); }
#else
(
	mach_port_t target_task,
	boolean_t inherit_memory,
	mach_port_t *child_task,
	mach_msg_type_name_t *child_taskPoly
);
#endif

/* Routine task_terminate */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_task_terminate
#if	defined(LINTLIBRARY)
    (target_task)
	mach_port_t target_task;
{ return S_task_terminate(target_task); }
#else
(
	mach_port_t target_task
);
#endif

/* Routine task_get_emulation_vector */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_task_get_emulation_vector
#if	defined(LINTLIBRARY)
    (task, vector_start, emulation_vector, emulation_vectorCnt)
	mach_port_t task;
	int *vector_start;
	emulation_vector_t *emulation_vector;
	mach_msg_type_number_t *emulation_vectorCnt;
{ return S_task_get_emulation_vector(task, vector_start, emulation_vector, emulation_vectorCnt); }
#else
(
	mach_port_t task,
	int *vector_start,
	emulation_vector_t *emulation_vector,
	mach_msg_type_number_t *emulation_vectorCnt
);
#endif

/* Routine task_set_emulation_vector */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_task_set_emulation_vector
#if	defined(LINTLIBRARY)
    (task, vector_start, emulation_vector, emulation_vectorCnt)
	mach_port_t task;
	int vector_start;
	emulation_vector_t emulation_vector;
	mach_msg_type_number_t emulation_vectorCnt;
{ return S_task_set_emulation_vector(task, vector_start, emulation_vector, emulation_vectorCnt); }
#else
(
	mach_port_t task,
	int vector_start,
	emulation_vector_t emulation_vector,
	mach_msg_type_number_t emulation_vectorCnt
);
#endif

/* Routine task_threads */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_task_threads
#if	defined(LINTLIBRARY)
    (target_task, thread_list, thread_listCnt)
	mach_port_t target_task;
	thread_array_t *thread_list;
	mach_msg_type_number_t *thread_listCnt;
{ return S_task_threads(target_task, thread_list, thread_listCnt); }
#else
(
	mach_port_t target_task,
	thread_array_t *thread_list,
	mach_msg_type_number_t *thread_listCnt
);
#endif

/* Routine task_info */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_task_info
#if	defined(LINTLIBRARY)
    (target_task, flavor, task_info_out, task_info_outCnt)
	mach_port_t target_task;
	int flavor;
	task_info_t task_info_out;
	mach_msg_type_number_t *task_info_outCnt;
{ return S_task_info(target_task, flavor, task_info_out, task_info_outCnt); }
#else
(
	mach_port_t target_task,
	int flavor,
	task_info_t task_info_out,
	mach_msg_type_number_t *task_info_outCnt
);
#endif

/* Routine thread_terminate */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_thread_terminate
#if	defined(LINTLIBRARY)
    (target_thread)
	mach_port_t target_thread;
{ return S_thread_terminate(target_thread); }
#else
(
	mach_port_t target_thread
);
#endif

/* Routine thread_get_state */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_thread_get_state
#if	defined(LINTLIBRARY)
    (target_thread, flavor, old_state, old_stateCnt)
	mach_port_t target_thread;
	int flavor;
	thread_state_t old_state;
	mach_msg_type_number_t *old_stateCnt;
{ return S_thread_get_state(target_thread, flavor, old_state, old_stateCnt); }
#else
(
	mach_port_t target_thread,
	int flavor,
	thread_state_t old_state,
	mach_msg_type_number_t *old_stateCnt
);
#endif

/* Routine thread_set_state */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_thread_set_state
#if	defined(LINTLIBRARY)
    (target_thread, flavor, new_state, new_stateCnt)
	mach_port_t target_thread;
	int flavor;
	thread_state_t new_state;
	mach_msg_type_number_t new_stateCnt;
{ return S_thread_set_state(target_thread, flavor, new_state, new_stateCnt); }
#else
(
	mach_port_t target_thread,
	int flavor,
	thread_state_t new_state,
	mach_msg_type_number_t new_stateCnt
);
#endif

/* Routine thread_info */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_thread_info
#if	defined(LINTLIBRARY)
    (target_thread, flavor, thread_info_out, thread_info_outCnt)
	mach_port_t target_thread;
	int flavor;
	thread_info_t thread_info_out;
	mach_msg_type_number_t *thread_info_outCnt;
{ return S_thread_info(target_thread, flavor, thread_info_out, thread_info_outCnt); }
#else
(
	mach_port_t target_thread,
	int flavor,
	thread_info_t thread_info_out,
	mach_msg_type_number_t *thread_info_outCnt
);
#endif

/* Routine vm_allocate */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_vm_allocate
#if	defined(LINTLIBRARY)
    (target_task, address, size, anywhere)
	mach_port_t target_task;
	vm_address_t *address;
	vm_size_t size;
	boolean_t anywhere;
{ return S_vm_allocate(target_task, address, size, anywhere); }
#else
(
	mach_port_t target_task,
	vm_address_t *address,
	vm_size_t size,
	boolean_t anywhere
);
#endif

/* Routine vm_deallocate */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_vm_deallocate
#if	defined(LINTLIBRARY)
    (target_task, address, size)
	mach_port_t target_task;
	vm_address_t address;
	vm_size_t size;
{ return S_vm_deallocate(target_task, address, size); }
#else
(
	mach_port_t target_task,
	vm_address_t address,
	vm_size_t size
);
#endif

/* Routine vm_protect */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_vm_protect
#if	defined(LINTLIBRARY)
    (target_task, address, size, set_maximum, new_protection)
	mach_port_t target_task;
	vm_address_t address;
	vm_size_t size;
	boolean_t set_maximum;
	vm_prot_t new_protection;
{ return S_vm_protect(target_task, address, size, set_maximum, new_protection); }
#else
(
	mach_port_t target_task,
	vm_address_t address,
	vm_size_t size,
	boolean_t set_maximum,
	vm_prot_t new_protection
);
#endif

/* Routine vm_inherit */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_vm_inherit
#if	defined(LINTLIBRARY)
    (target_task, address, size, new_inheritance)
	mach_port_t target_task;
	vm_address_t address;
	vm_size_t size;
	vm_inherit_t new_inheritance;
{ return S_vm_inherit(target_task, address, size, new_inheritance); }
#else
(
	mach_port_t target_task,
	vm_address_t address,
	vm_size_t size,
	vm_inherit_t new_inheritance
);
#endif

/* Routine vm_read */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_vm_read
#if	defined(LINTLIBRARY)
    (target_task, address, size, data, dataCnt)
	mach_port_t target_task;
	vm_address_t address;
	vm_size_t size;
	vm_offset_t *data;
	mach_msg_type_number_t *dataCnt;
{ return S_vm_read(target_task, address, size, data, dataCnt); }
#else
(
	mach_port_t target_task,
	vm_address_t address,
	vm_size_t size,
	vm_offset_t *data,
	mach_msg_type_number_t *dataCnt
);
#endif

/* Routine vm_write */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_vm_write
#if	defined(LINTLIBRARY)
    (target_task, address, data, dataCnt)
	mach_port_t target_task;
	vm_address_t address;
	vm_offset_t data;
	mach_msg_type_number_t dataCnt;
{ return S_vm_write(target_task, address, data, dataCnt); }
#else
(
	mach_port_t target_task,
	vm_address_t address,
	vm_offset_t data,
	mach_msg_type_number_t dataCnt
);
#endif

/* Routine vm_copy */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_vm_copy
#if	defined(LINTLIBRARY)
    (target_task, source_address, size, dest_address)
	mach_port_t target_task;
	vm_address_t source_address;
	vm_size_t size;
	vm_address_t dest_address;
{ return S_vm_copy(target_task, source_address, size, dest_address); }
#else
(
	mach_port_t target_task,
	vm_address_t source_address,
	vm_size_t size,
	vm_address_t dest_address
);
#endif

/* Routine vm_region */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_vm_region
#if	defined(LINTLIBRARY)
    (target_task, address, size, protection, max_protection, inheritance, is_shared, object_name, offset)
	mach_port_t target_task;
	vm_address_t *address;
	vm_size_t *size;
	vm_prot_t *protection;
	vm_prot_t *max_protection;
	vm_inherit_t *inheritance;
	boolean_t *is_shared;
	mach_port_t *object_name;
	vm_offset_t *offset;
{ return S_vm_region(target_task, address, size, protection, max_protection, inheritance, is_shared, object_name, offset); }
#else
(
	mach_port_t target_task,
	vm_address_t *address,
	vm_size_t *size,
	vm_prot_t *protection,
	vm_prot_t *max_protection,
	vm_inherit_t *inheritance,
	boolean_t *is_shared,
	mach_port_t *object_name,
	vm_offset_t *offset
);
#endif

/* Routine vm_statistics */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_vm_statistics
#if	defined(LINTLIBRARY)
    (target_task, vm_stats)
	mach_port_t target_task;
	vm_statistics_data_t *vm_stats;
{ return S_vm_statistics(target_task, vm_stats); }
#else
(
	mach_port_t target_task,
	vm_statistics_data_t *vm_stats
);
#endif

/* Routine mach_ports_register */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_mach_ports_register
#if	defined(LINTLIBRARY)
    (target_task, init_port_set, init_port_setCnt)
	mach_port_t target_task;
	mach_port_array_t init_port_set;
	mach_msg_type_number_t init_port_setCnt;
{ return S_mach_ports_register(target_task, init_port_set, init_port_setCnt); }
#else
(
	mach_port_t target_task,
	mach_port_array_t init_port_set,
	mach_msg_type_number_t init_port_setCnt
);
#endif

/* Routine mach_ports_lookup */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_mach_ports_lookup
#if	defined(LINTLIBRARY)
    (target_task, init_port_set, init_port_setCnt)
	mach_port_t target_task;
	mach_port_array_t *init_port_set;
	mach_msg_type_number_t *init_port_setCnt;
{ return S_mach_ports_lookup(target_task, init_port_set, init_port_setCnt); }
#else
(
	mach_port_t target_task,
	mach_port_array_t *init_port_set,
	mach_msg_type_number_t *init_port_setCnt
);
#endif

/* SimpleRoutine memory_object_data_provided */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_memory_object_data_provided
#if	defined(LINTLIBRARY)
    (memory_control, offset, data, dataCnt, lock_value)
	mach_port_t memory_control;
	vm_offset_t offset;
	vm_offset_t data;
	mach_msg_type_number_t dataCnt;
	vm_prot_t lock_value;
{ return S_memory_object_data_provided(memory_control, offset, data, dataCnt, lock_value); }
#else
(
	mach_port_t memory_control,
	vm_offset_t offset,
	vm_offset_t data,
	mach_msg_type_number_t dataCnt,
	vm_prot_t lock_value
);
#endif

/* SimpleRoutine memory_object_data_unavailable */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_memory_object_data_unavailable
#if	defined(LINTLIBRARY)
    (memory_control, offset, size)
	mach_port_t memory_control;
	vm_offset_t offset;
	vm_size_t size;
{ return S_memory_object_data_unavailable(memory_control, offset, size); }
#else
(
	mach_port_t memory_control,
	vm_offset_t offset,
	vm_size_t size
);
#endif

/* Routine memory_object_get_attributes */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_memory_object_get_attributes
#if	defined(LINTLIBRARY)
    (memory_control, object_ready, may_cache, copy_strategy)
	mach_port_t memory_control;
	boolean_t *object_ready;
	boolean_t *may_cache;
	memory_object_copy_strategy_t *copy_strategy;
{ return S_memory_object_get_attributes(memory_control, object_ready, may_cache, copy_strategy); }
#else
(
	mach_port_t memory_control,
	boolean_t *object_ready,
	boolean_t *may_cache,
	memory_object_copy_strategy_t *copy_strategy
);
#endif

/* Routine vm_set_default_memory_manager */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_vm_set_default_memory_manager
#if	defined(LINTLIBRARY)
    (host_priv, default_manager)
	mach_port_t host_priv;
	mach_port_t *default_manager;
{ return S_vm_set_default_memory_manager(host_priv, default_manager); }
#else
(
	mach_port_t host_priv,
	mach_port_t *default_manager
);
#endif

/* SimpleRoutine xxx_memory_object_lock_request */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_xxx_memory_object_lock_request
#if	defined(LINTLIBRARY)
    (memory_control, offset, size, should_clean, should_flush, lock_value, reply_to, reply_toPoly)
	mach_port_t memory_control;
	vm_offset_t offset;
	vm_size_t size;
	boolean_t should_clean;
	boolean_t should_flush;
	vm_prot_t lock_value;
	mach_port_t reply_to;
	mach_msg_type_name_t reply_toPoly;
{ return S_xxx_memory_object_lock_request(memory_control, offset, size, should_clean, should_flush, lock_value, reply_to, reply_toPoly); }
#else
(
	mach_port_t memory_control,
	vm_offset_t offset,
	vm_size_t size,
	boolean_t should_clean,
	boolean_t should_flush,
	vm_prot_t lock_value,
	mach_port_t reply_to,
	mach_msg_type_name_t reply_toPoly
);
#endif

/* SimpleRoutine memory_object_lock_request */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_memory_object_lock_request
#if	defined(LINTLIBRARY)
    (memory_control, offset, size, should_return, should_flush, lock_value, reply_to, reply_toPoly)
	mach_port_t memory_control;
	vm_offset_t offset;
	vm_size_t size;
	memory_object_return_t should_return;
	boolean_t should_flush;
	vm_prot_t lock_value;
	mach_port_t reply_to;
	mach_msg_type_name_t reply_toPoly;
{ return S_memory_object_lock_request(memory_control, offset, size, should_return, should_flush, lock_value, reply_to, reply_toPoly); }
#else
(
	mach_port_t memory_control,
	vm_offset_t offset,
	vm_size_t size,
	memory_object_return_t should_return,
	boolean_t should_flush,
	vm_prot_t lock_value,
	mach_port_t reply_to,
	mach_msg_type_name_t reply_toPoly
);
#endif

/* Routine xxx_task_get_emulation_vector */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_xxx_task_get_emulation_vector
#if	defined(LINTLIBRARY)
    (task, vector_start, emulation_vector, emulation_vectorCnt)
	mach_port_t task;
	int *vector_start;
	emulation_vector_t emulation_vector;
	mach_msg_type_number_t *emulation_vectorCnt;
{ return S_xxx_task_get_emulation_vector(task, vector_start, emulation_vector, emulation_vectorCnt); }
#else
(
	mach_port_t task,
	int *vector_start,
	emulation_vector_t emulation_vector,
	mach_msg_type_number_t *emulation_vectorCnt
);
#endif

/* Routine xxx_task_set_emulation_vector */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_xxx_task_set_emulation_vector
#if	defined(LINTLIBRARY)
    (task, vector_start, emulation_vector, emulation_vectorCnt)
	mach_port_t task;
	int vector_start;
	emulation_vector_t emulation_vector;
	mach_msg_type_number_t emulation_vectorCnt;
{ return S_xxx_task_set_emulation_vector(task, vector_start, emulation_vector, emulation_vectorCnt); }
#else
(
	mach_port_t task,
	int vector_start,
	emulation_vector_t emulation_vector,
	mach_msg_type_number_t emulation_vectorCnt
);
#endif

/* Routine xxx_host_info */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_xxx_host_info
#if	defined(LINTLIBRARY)
    (target_task, info)
	mach_port_t target_task;
	machine_info_data_t *info;
{ return S_xxx_host_info(target_task, info); }
#else
(
	mach_port_t target_task,
	machine_info_data_t *info
);
#endif

/* Routine xxx_slot_info */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_xxx_slot_info
#if	defined(LINTLIBRARY)
    (target_task, slot, info)
	mach_port_t target_task;
	int slot;
	machine_slot_data_t *info;
{ return S_xxx_slot_info(target_task, slot, info); }
#else
(
	mach_port_t target_task,
	int slot,
	machine_slot_data_t *info
);
#endif

/* Routine xxx_cpu_control */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_xxx_cpu_control
#if	defined(LINTLIBRARY)
    (target_task, cpu, running)
	mach_port_t target_task;
	int cpu;
	boolean_t running;
{ return S_xxx_cpu_control(target_task, cpu, running); }
#else
(
	mach_port_t target_task,
	int cpu,
	boolean_t running
);
#endif

/* Routine task_suspend */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_task_suspend
#if	defined(LINTLIBRARY)
    (target_task)
	mach_port_t target_task;
{ return S_task_suspend(target_task); }
#else
(
	mach_port_t target_task
);
#endif

/* Routine task_resume */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_task_resume
#if	defined(LINTLIBRARY)
    (target_task)
	mach_port_t target_task;
{ return S_task_resume(target_task); }
#else
(
	mach_port_t target_task
);
#endif

/* Routine task_get_special_port */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_task_get_special_port
#if	defined(LINTLIBRARY)
    (task, which_port, special_port, special_portPoly)
	mach_port_t task;
	int which_port;
	mach_port_t *special_port;
	mach_msg_type_name_t *special_portPoly;
{ return S_task_get_special_port(task, which_port, special_port, special_portPoly); }
#else
(
	mach_port_t task,
	int which_port,
	mach_port_t *special_port,
	mach_msg_type_name_t *special_portPoly
);
#endif

/* Routine task_set_special_port */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_task_set_special_port
#if	defined(LINTLIBRARY)
    (task, which_port, special_port)
	mach_port_t task;
	int which_port;
	mach_port_t special_port;
{ return S_task_set_special_port(task, which_port, special_port); }
#else
(
	mach_port_t task,
	int which_port,
	mach_port_t special_port
);
#endif

/* Routine xxx_task_info */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_xxx_task_info
#if	defined(LINTLIBRARY)
    (target_task, flavor, task_info_out, task_info_outCnt)
	mach_port_t target_task;
	int flavor;
	task_info_t task_info_out;
	mach_msg_type_number_t *task_info_outCnt;
{ return S_xxx_task_info(target_task, flavor, task_info_out, task_info_outCnt); }
#else
(
	mach_port_t target_task,
	int flavor,
	task_info_t task_info_out,
	mach_msg_type_number_t *task_info_outCnt
);
#endif

/* Routine thread_create */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_thread_create
#if	defined(LINTLIBRARY)
    (parent_task, child_thread, child_threadPoly)
	mach_port_t parent_task;
	mach_port_t *child_thread;
	mach_msg_type_name_t *child_threadPoly;
{ return S_thread_create(parent_task, child_thread, child_threadPoly); }
#else
(
	mach_port_t parent_task,
	mach_port_t *child_thread,
	mach_msg_type_name_t *child_threadPoly
);
#endif

/* Routine thread_suspend */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_thread_suspend
#if	defined(LINTLIBRARY)
    (target_thread)
	mach_port_t target_thread;
{ return S_thread_suspend(target_thread); }
#else
(
	mach_port_t target_thread
);
#endif

/* Routine thread_resume */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_thread_resume
#if	defined(LINTLIBRARY)
    (target_thread)
	mach_port_t target_thread;
{ return S_thread_resume(target_thread); }
#else
(
	mach_port_t target_thread
);
#endif

/* Routine thread_abort */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_thread_abort
#if	defined(LINTLIBRARY)
    (target_thread)
	mach_port_t target_thread;
{ return S_thread_abort(target_thread); }
#else
(
	mach_port_t target_thread
);
#endif

/* Routine xxx_thread_get_state */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_xxx_thread_get_state
#if	defined(LINTLIBRARY)
    (target_thread, flavor, old_state, old_stateCnt)
	mach_port_t target_thread;
	int flavor;
	thread_state_t old_state;
	mach_msg_type_number_t *old_stateCnt;
{ return S_xxx_thread_get_state(target_thread, flavor, old_state, old_stateCnt); }
#else
(
	mach_port_t target_thread,
	int flavor,
	thread_state_t old_state,
	mach_msg_type_number_t *old_stateCnt
);
#endif

/* Routine xxx_thread_set_state */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_xxx_thread_set_state
#if	defined(LINTLIBRARY)
    (target_thread, flavor, new_state, new_stateCnt)
	mach_port_t target_thread;
	int flavor;
	thread_state_t new_state;
	mach_msg_type_number_t new_stateCnt;
{ return S_xxx_thread_set_state(target_thread, flavor, new_state, new_stateCnt); }
#else
(
	mach_port_t target_thread,
	int flavor,
	thread_state_t new_state,
	mach_msg_type_number_t new_stateCnt
);
#endif

/* Routine thread_get_special_port */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_thread_get_special_port
#if	defined(LINTLIBRARY)
    (thread, which_port, special_port)
	mach_port_t thread;
	int which_port;
	mach_port_t *special_port;
{ return S_thread_get_special_port(thread, which_port, special_port); }
#else
(
	mach_port_t thread,
	int which_port,
	mach_port_t *special_port
);
#endif

/* Routine thread_set_special_port */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_thread_set_special_port
#if	defined(LINTLIBRARY)
    (thread, which_port, special_port)
	mach_port_t thread;
	int which_port;
	mach_port_t special_port;
{ return S_thread_set_special_port(thread, which_port, special_port); }
#else
(
	mach_port_t thread,
	int which_port,
	mach_port_t special_port
);
#endif

/* Routine xxx_thread_info */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_xxx_thread_info
#if	defined(LINTLIBRARY)
    (target_thread, flavor, thread_info_out, thread_info_outCnt)
	mach_port_t target_thread;
	int flavor;
	thread_info_t thread_info_out;
	mach_msg_type_number_t *thread_info_outCnt;
{ return S_xxx_thread_info(target_thread, flavor, thread_info_out, thread_info_outCnt); }
#else
(
	mach_port_t target_thread,
	int flavor,
	thread_info_t thread_info_out,
	mach_msg_type_number_t *thread_info_outCnt
);
#endif

/* Routine task_set_emulation */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_task_set_emulation
#if	defined(LINTLIBRARY)
    (target_port, routine_entry_pt, routine_number)
	mach_port_t target_port;
	vm_address_t routine_entry_pt;
	int routine_number;
{ return S_task_set_emulation(target_port, routine_entry_pt, routine_number); }
#else
(
	mach_port_t target_port,
	vm_address_t routine_entry_pt,
	int routine_number
);
#endif

/* Routine task_ras_control */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_task_ras_control
#if	defined(LINTLIBRARY)
    (target_task, basepc, boundspc, flavor)
	mach_port_t target_task;
	vm_address_t basepc;
	vm_address_t boundspc;
	int flavor;
{ return S_task_ras_control(target_task, basepc, boundspc, flavor); }
#else
(
	mach_port_t target_task,
	vm_address_t basepc,
	vm_address_t boundspc,
	int flavor
);
#endif

/* Routine vm_map */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_vm_map
#if	defined(LINTLIBRARY)
    (target_task, address, size, mask, anywhere, memory_object, offset, copy, cur_protection, max_protection, inheritance)
	mach_port_t target_task;
	vm_address_t *address;
	vm_size_t size;
	vm_address_t mask;
	boolean_t anywhere;
	mach_port_t memory_object;
	vm_offset_t offset;
	boolean_t copy;
	vm_prot_t cur_protection;
	vm_prot_t max_protection;
	vm_inherit_t inheritance;
{ return S_vm_map(target_task, address, size, mask, anywhere, memory_object, offset, copy, cur_protection, max_protection, inheritance); }
#else
(
	mach_port_t target_task,
	vm_address_t *address,
	vm_size_t size,
	vm_address_t mask,
	boolean_t anywhere,
	mach_port_t memory_object,
	vm_offset_t offset,
	boolean_t copy,
	vm_prot_t cur_protection,
	vm_prot_t max_protection,
	vm_inherit_t inheritance
);
#endif

/* SimpleRoutine memory_object_data_error */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_memory_object_data_error
#if	defined(LINTLIBRARY)
    (memory_control, offset, size, error_value)
	mach_port_t memory_control;
	vm_offset_t offset;
	vm_size_t size;
	kern_return_t error_value;
{ return S_memory_object_data_error(memory_control, offset, size, error_value); }
#else
(
	mach_port_t memory_control,
	vm_offset_t offset,
	vm_size_t size,
	kern_return_t error_value
);
#endif

/* SimpleRoutine memory_object_set_attributes */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_memory_object_set_attributes
#if	defined(LINTLIBRARY)
    (memory_control, object_ready, may_cache, copy_strategy)
	mach_port_t memory_control;
	boolean_t object_ready;
	boolean_t may_cache;
	memory_object_copy_strategy_t copy_strategy;
{ return S_memory_object_set_attributes(memory_control, object_ready, may_cache, copy_strategy); }
#else
(
	mach_port_t memory_control,
	boolean_t object_ready,
	boolean_t may_cache,
	memory_object_copy_strategy_t copy_strategy
);
#endif

/* SimpleRoutine memory_object_destroy */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_memory_object_destroy
#if	defined(LINTLIBRARY)
    (memory_control, reason)
	mach_port_t memory_control;
	kern_return_t reason;
{ return S_memory_object_destroy(memory_control, reason); }
#else
(
	mach_port_t memory_control,
	kern_return_t reason
);
#endif

/* SimpleRoutine memory_object_data_supply */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_memory_object_data_supply
#if	defined(LINTLIBRARY)
    (memory_control, offset, data, dataCnt, lock_value, precious, reply_to, reply_toPoly)
	mach_port_t memory_control;
	vm_offset_t offset;
	vm_offset_t data;
	mach_msg_type_number_t dataCnt;
	vm_prot_t lock_value;
	boolean_t precious;
	mach_port_t reply_to;
	mach_msg_type_name_t reply_toPoly;
{ return S_memory_object_data_supply(memory_control, offset, data, dataCnt, lock_value, precious, reply_to, reply_toPoly); }
#else
(
	mach_port_t memory_control,
	vm_offset_t offset,
	vm_offset_t data,
	mach_msg_type_number_t dataCnt,
	vm_prot_t lock_value,
	boolean_t precious,
	mach_port_t reply_to,
	mach_msg_type_name_t reply_toPoly
);
#endif

/* SimpleRoutine memory_object_ready */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_memory_object_ready
#if	defined(LINTLIBRARY)
    (memory_control, may_cache, copy_strategy)
	mach_port_t memory_control;
	boolean_t may_cache;
	memory_object_copy_strategy_t copy_strategy;
{ return S_memory_object_ready(memory_control, may_cache, copy_strategy); }
#else
(
	mach_port_t memory_control,
	boolean_t may_cache,
	memory_object_copy_strategy_t copy_strategy
);
#endif

/* SimpleRoutine memory_object_change_attributes */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_memory_object_change_attributes
#if	defined(LINTLIBRARY)
    (memory_control, may_cache, copy_strategy, reply_to, reply_toPoly)
	mach_port_t memory_control;
	boolean_t may_cache;
	memory_object_copy_strategy_t copy_strategy;
	mach_port_t reply_to;
	mach_msg_type_name_t reply_toPoly;
{ return S_memory_object_change_attributes(memory_control, may_cache, copy_strategy, reply_to, reply_toPoly); }
#else
(
	mach_port_t memory_control,
	boolean_t may_cache,
	memory_object_copy_strategy_t copy_strategy,
	mach_port_t reply_to,
	mach_msg_type_name_t reply_toPoly
);
#endif

/* Routine vm_machine_attribute */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t S_vm_machine_attribute
#if	defined(LINTLIBRARY)
    (target_task, address, size, attribute, value)
	mach_port_t target_task;
	vm_address_t address;
	vm_size_t size;
	vm_machine_attribute_t attribute;
	vm_machine_attribute_val_t *value;
{ return S_vm_machine_attribute(target_task, address, size, attribute, value); }
#else
(
	mach_port_t target_task,
	vm_address_t address,
	vm_size_t size,
	vm_machine_attribute_t attribute,
	vm_machine_attribute_val_t *value
);
#endif

#endif	/* not defined(_mach_server_) */
