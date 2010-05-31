/**
 * \file   windhoek/server/src/components.c
 * \brief  Windhoek IDL component implementation
 *
 * \date   2008-02-15
 * \author Bjoern Doebel <doebel@tudos.org>
 *
 * (c) 2008 Technische Universitaet Dresden
 * This file is part of DROPS, which is distributed under the terms of the
 * GNU General Public License 2. Please see the COPYING file for details.
 */

#include <l4/log/l4log.h>
#include <l4/l4rm/l4rm.h>
#include <l4/dm_mem/dm_mem.h>

#include <l4/dde/linux26/dde26_mem.h> // page cache functions
#include <l4/dde/ddekit/pgtab.h> // set_region_with_size

#include <linux/list.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/bio.h>
#include <linux/hdreg.h>
#include <linux/mm.h>

#include "windhoek_local.h"
#include <l4/windhoek/server-internal-server.h>
#include <l4/windhoek/server-internal-client.h>

#define DEBUG 0

struct client_state clients[MAX_CLIENTS];


int get_free_client_state(void)
{
	int i;
	int ret = -1;

	for (i=0; i < MAX_CLIENTS; ++i) {
		if (!client_state_valid(i)) {
			ret = i;
			break;
		}
	}

	return ret;
}


void free_client_state(int idx)
{
	/*
	 * clean up page struct mappings
	 *
	 * XXX: If this was time-critical, we could of course implement a
	 *      page_cache_lookup_and_del() function, but freeing a client
	 *      object won't happen that often.
	 */
	int i;
	for (i=0; i < clients[idx].data_size; i += PAGE_SIZE) {
		struct page *p = dde_page_lookup((unsigned long)clients[idx].data_address + i);
		if (p)
			dde_page_cache_remove(p);
	}

	/*
	 * detach dataspace
	 */
	l4rm_detach(clients[idx].data_address);
	l4rm_detach(clients[idx].ctrl_address);

	/*
	 * XXX: does l4rm_detach() also remove the user ptr???
	 */

	/*
	 * remove cached address and gendisk
	 */
	clients[idx].data_ds      = L4DM_INVALID_DATASPACE;
	clients[idx].data_address = NULL;
	clients[idx].data_size    = 0;
	clients[idx].ctrl_ds      = L4DM_INVALID_DATASPACE;
	clients[idx].ctrl_address = NULL;
	clients[idx].ctrl_size    = 0;
	clients[idx].gendisk      = NULL;
	clients[idx].client_id    = L4_INVALID_ID;
}


static int init_clientstate(int idx, const l4dm_dataspace_t *ctrl, l4_size_t ctrl_size,
                               const l4dm_dataspace_t *data, l4_size_t data_size,
                               struct gendisk *disk, l4_threadid_t client)
{
	windhoek_device *dev = NULL;
	struct hd_geometry geom;
	struct block_device *bd = NULL;
	int err, dummy;
	ddekit_addr_t pa;

	/*
	 * attach data and ctrl dataspaces
	 */
	err = l4rm_attach(data, data_size, 0, L4DM_RW | L4RM_LOG2_ALIGNED | L4RM_MAP,
	                  &clients[idx].data_address);
	LOGd(DEBUG, "attach data to %p (size %d), err %d", clients[idx].data_address, data_size, err);
	if (err) {
		LOG_Error("cannot attach data area.");
		free_client_state(idx);
		return -ENOMEM;
	}
	memset(clients[idx].data_address, 0, data_size);

	err = l4dm_mem_ds_phys_addr(data, 0, data_size, &pa, &dummy);
	LOGd(DEBUG, "data phys addr: %p (err %d)", (void*)pa, err);
	if (err) {
		LOG_Error("cannot find phys addr for %p", clients[idx].data_address);
		free_client_state(idx);
		return -ENOMEM;
	}

	/*
	 * For the data area, we add a page struct for each page in the ds, so
	 * that Linux code may later on use it as it was normal Linux kernel
	 * memory.
	 */
	for (dummy = 0; dummy < data_size; dummy += PAGE_SIZE) {
		unsigned long va = (unsigned long)(clients[idx].data_address + dummy);
		struct page *p = kmalloc(sizeof(struct page), GFP_KERNEL);
		p->virtual = (void *)va;
		//LOGd(DEBUG, "page %p -> %p", p, page_address(p));
		dde_page_cache_add(p);
	}

	/*
	 * add virt->phys mapping for __pa()
	 */
	ddekit_pgtab_set_region_with_size(clients[idx].data_address,
	                                  pa, data_size, PTE_TYPE_LARGE);


	err = l4rm_attach(ctrl, ctrl_size, 0, L4DM_RW | L4RM_MAP,
	                  &clients[idx].ctrl_address);
	LOGd(DEBUG, "attach ctrl to %p, err %d", clients[idx].ctrl_address, err);
	if (err) {
		LOG_Error("cannot attach ctrl area.");
		free_client_state(idx);
		return -ENOMEM;
	}

	err = l4dm_mem_ds_phys_addr(ctrl, 0, ctrl_size, &pa, &dummy);
	LOGd(DEBUG, "ctrl phys addr: %p (err %d)", (void*)pa, err);
	if (err) {
		LOG_Error("cannot find phys addr for %p", clients[idx].ctrl_address);
		free_client_state(idx);
		return -ENOMEM;
	}

	/*
	 * add virt->phys mapping for __pa()
	 */
	ddekit_pgtab_set_region_with_size(clients[idx].ctrl_address, (ddekit_addr_t)pa, ctrl_size, PTE_TYPE_LARGE);

	clients[idx].data_ds   = *data;
	clients[idx].data_size = data_size;
	clients[idx].ctrl_ds   = *ctrl;
	clients[idx].ctrl_size = ctrl_size;
	clients[idx].gendisk   = disk;
	clients[idx].client_id = client;

	INIT_LIST_HEAD(&clients[idx].finished_bios);
	clients[idx].notify_waiter = L4_INVALID_ID;
	spin_lock_init(&clients[idx].lock);

	/*
	 * Fill in the windhoek device at beginning of CTRL ds.
	 */
	bd = bdget_disk(clients[idx].gendisk, 0);
	err = clients[idx].gendisk->fops->getgeo(bd, &geom);

	dev = (windhoek_device *)clients[idx].ctrl_address;
	dev->sectors      = geom.sectors;
	dev->start_sector = 0;
	dev->block_size   = 512;
	dev->heads        = geom.heads;
	dev->cylinders    = geom.cylinders;
	dev->flags        = 0;
	dev->capacity     = get_capacity(clients[idx].gendisk);
	strncpy(dev->name, disk->disk_name, MAX_NAME_LEN);
	dev->handle       = idx;

	return 0;
}


int
windhoek_client_open_component (CORBA_Object _dice_corba_obj,
                                const char* name /* in */,
                                const l4dm_dataspace_t *ctrl_ds /* in */,
                                l4_size_t ctrl_size /* in */,
                                const l4dm_dataspace_t *data_ds /* in */,
                                l4_size_t data_size /* in */,
                                CORBA_Server_Environment *_dice_corba_env)
{
	struct gendisk *disk;
	int idx, err;
	l4_size_t size;

	LOGd(DEBUG, "name '%s'", name);
	LOGd(DEBUG, "data ds: id 0x%03x, tid "l4util_idfmt, data_ds->id, l4util_idstr(data_ds->manager));
	LOGd(DEBUG, "ctrl ds: id 0x%03x, tid "l4util_idfmt, ctrl_ds->id, l4util_idstr(ctrl_ds->manager));

	/*
	 * find gendisk object
	 */
	disk = find_disk_by_name(name);
	LOGd(DEBUG, "disk @ %p", disk);

	if (!disk) {
		LOG_Error("No such disk '%s'", name);
		return -EACCES;
	}

	idx = get_free_client_state();
	if (idx < 0) {
		LOG_Error("Out of client states...");
		return -ENOMEM;
	}

	if ((err = init_clientstate(idx, ctrl_ds, ctrl_size, data_ds, data_size,
	                            disk, *_dice_corba_obj)))
		return err;

	return idx;
}


int
windhoek_client_close_component (CORBA_Object _dice_corba_obj,
                                 int handle /* in */,
                                 CORBA_Server_Environment *_dice_corba_env)
{
	SANITY_CHECK(error, handle, *_dice_corba_obj);

	free_client_state(handle);

	return 0;
}


static void bio_end_component(struct bio *bio, int err)
{
	windhoek_bio *wb = bio->bi_private;
	int error;
	DICE_DECLARE_ENV(env);

	if (DEBUG)
		printk("bio @ %p (handle %d) done.\n", bio, wb->handle);

	// send notification
	error = windhoek_server_notify_call(&main_thread, wb, &env);

	/* XXX: bio cleanup ?*/
}



int windhoek_client_start_request_component(CORBA_Object _dice_corba_obj,
                                            int handle,
                                            unsigned long offset,
                                            CORBA_Server_Environment *_dice_corba_env)
{
	struct client_state *c  = &clients[handle];
	struct block_device *bd = NULL;
	unsigned long addr      = 0;
	struct bio *bio         = NULL;
	windhoek_bio *wbio      = NULL;
	int i                   = -1;

	LOGd(DEBUG, "handle %d, offset %lx", handle, offset);
	SANITY_CHECK(error, handle, *_dice_corba_obj);

	bio = bio_alloc(GFP_NOIO, 1);
	LOGd(DEBUG, "alloced bio: %p", bio);

	bd = bdget_disk(clients[handle].gendisk, 0);

	addr = (unsigned long)clients[handle].ctrl_address;
	addr += offset;
	
	wbio = (windhoek_bio *)addr;
	LOGd(DEBUG, "wbio @ %p", wbio);

	bio->bi_sector      = wbio->start_sector;
	bio->bi_bdev        = bd;
	bio->bi_vcnt        = wbio->sg_count;
	bio->bi_size        = 0;
	LOGd(DEBUG, "sector: %lx, dev %p, sg count %d", bio->bi_sector, bio->bi_bdev, bio->bi_vcnt);

	for (i = 0; i < wbio->sg_count; ++i) {
		unsigned long va      = 0;
		ds_list_element *dsle = &wbio->sg_data[i];
		struct page *page     = NULL;

#if DEBUG
		dump_ds_element(dsle);
#endif

		va = (unsigned long)clients[handle].data_address;
		va += dsle->offset;
		va &= PAGE_MASK;
		LOGd(DEBUG, "va @ %lx", va);

		page = dde_page_lookup(va);
		LOGd(DEBUG, "page @ %p", page);
		BUG_ON(!page);

		bio->bi_io_vec[i].bv_page = page;
		LOGd(DEBUG, "bi_io_vec[%d].bv_page = %p", i, page);
		bio->bi_io_vec[i].bv_len  = dsle->size;
		bio->bi_io_vec[i].bv_offset = (unsigned long)clients[handle].data_address + dsle->offset - va;

		bio->bi_size += dsle->size;
	}

	bio->bi_end_io = bio_end_component;
	bio->bi_private = wbio;

	bio_get(bio);
	if (wbio->type == BIO_READ)
		submit_bio(READ, bio);
	else
		submit_bio(WRITE, bio);
	bio_put(bio);
	
	return 0;
}


int
windhoek_client_next_notification_component (CORBA_Object _dice_corba_obj,
                                             int handle /* in */,
                                             short *_dice_reply,
                                             CORBA_Server_Environment *_dice_corba_env)
{
	int ret = 0;

	SANITY_CHECK(error, handle, *_dice_corba_obj);

	struct client_state *client = &clients[handle];

	spin_lock(&client->lock);

	/* No finished BIOs -> wait */
	if (list_empty(&client->finished_bios)) {
		client->notify_waiter = *_dice_corba_obj;
		*_dice_reply = DICE_NO_REPLY;
	}
	/* have pending BIOs -> reply immediately */
	else {
		struct list_head *h = &client->finished_bios;
		struct windhoek_bio_entry *be = list_entry(h, struct windhoek_bio_entry, next);
		list_del(h);
		ret = be->wbio->uid;
		kfree(be);
	}

	spin_unlock(&client->lock);

	return ret;
}


int
windhoek_server_notify_component (CORBA_Object _dice_corba_obj,
                                  const windhoek_bio *wbio /* in */,
                                  CORBA_Server_Environment *_dice_corba_env)
{
	struct client_state *client = &clients[wbio->handle];

	BUG_ON(!handle_valid(wbio->handle));
	BUG_ON(!l4_task_equal(*_dice_corba_obj, l4_myself()));

	spin_lock(&client->lock);

	if (l4_is_invalid_id(client->notify_waiter)) {
		struct windhoek_bio_entry *be = kmalloc(sizeof(*be), GFP_KERNEL);
		be->wbio = wbio;
		list_add_tail(&be->next, &client->finished_bios);
	}
	else {
		DICE_DECLARE_SERVER_ENV(env);
		windhoek_client_next_notification_reply(&client->notify_waiter, wbio->uid, &env);
		client->notify_waiter = L4_INVALID_ID;
	}

	spin_unlock(&client->lock);

	return 0;
}


void client_state_init(void)
{
	memset(clients, 0, sizeof(clients));
}
