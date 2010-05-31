/**
 * \file   windhoek/server/src/windhoek_local.h
 * \brief  Windhoek internal server header
 *
 * \date   2008-01-29
 * \author Bjoern Doebel <doebel@tudos.org>
 *
 * (c) 2008 Technische Universitaet Dresden
 * This file is part of DROPS, which is distributed under the terms of the
 * GNU General Public License 2. Please see the COPYING file for details.
 */

#ifndef WINDHOEK_LOCAL
#define WINDHOEK_LOCAL

#include <l4/windhoek/types.h>
#include <l4/util/macros.h>

#include <linux/genhd.h>
#include <linux/hdreg.h>

#define MAX_GENDISKS       10
#define MAX_CLIENTS        16

#undef DEBUG_MSG
#undef WARN_UNIMPL

#define DEBUG_MSG(msg, ...) printk("%s: \033[34m"msg"\033[0m\n", __FUNCTION__, ##__VA_ARGS__)
#define WARN_UNIMPL         DEBUG_MSG("unimplemented")

extern struct gendisk* known_disks[MAX_GENDISKS];
void gendisk_init(void);
struct gendisk* find_disk_by_name(const char *name);

/* Main thread, so that IRQ threads can send notifications */
extern l4_threadid_t main_thread;

struct windhoek_data_ds
{
	struct list_head next;
	l4dm_dataspace_t ds;
	l4_size_t        size;
};


struct windhoek_bio_entry
{
	struct list_head    next;
	const windhoek_bio *wbio;
};


struct client_state
{
	l4dm_dataspace_t   ctrl_ds;
	l4_size_t          ctrl_size;
	void              *ctrl_address;  /**< address where ctrl ds is mapped */
	l4dm_dataspace_t   data_ds;
	l4_size_t          data_size;
	void              *data_address;  /**< address where data ds is mapped */
	struct gendisk    *gendisk;       /**< the corresponding disk object */
	l4_threadid_t      client_id;
	l4_threadid_t      notify_waiter; /**< thread waiting for notification */
	struct list_head   finished_bios; /**< list of bios that have been serviced
										   - used to store until notifications
										   are retrieved by the client */
	spinlock_t         lock;          /**< lock XXX needed? */
};


extern struct client_state clients[MAX_CLIENTS];

void client_state_init(void);


/** Check whether there is a valid client with this handle */
L4_INLINE int client_state_valid(int idx)
{
	return clients[idx].data_address != NULL;
}


/** Check whether the handle itself is valid */
L4_INLINE int handle_valid(int handle)
{
	return ((handle >= 0) && (handle < MAX_CLIENTS));
}


/** sanity check handle and caller */
L4_INLINE int sanity_check(int handle, l4_threadid_t t)
{
	if (!handle_valid(handle)  ||
	    !client_state_valid(handle)) {
		LOG_Error("invalid handle %d", handle);
		return -EACCES;
	}

	if (!l4_task_equal(clients[handle].client_id, t)) {
		LOG_Error("Close by "l4util_idfmt", but owner is "l4util_idfmt,
		          l4util_idstr(t), l4util_idstr(clients[handle].client_id));
		return -EACCES;
	}

	return 0;
}

#define SANITY_CHECK(varname, handle, thread) \
	int varname = sanity_check(handle, thread); \
	if (varname) return varname;

#endif
