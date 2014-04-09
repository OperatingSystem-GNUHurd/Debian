/* 
   Copyright (C) 2008 Free Software Foundation, Inc.
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

/*
 * This program is a filter translator which sits on the top of the network
 * interface. 
 * It provides two filters: for outgoing packets and for incoming packets.
 * Only one pfinet server are allowed to run on the top of the translator. 
 */

#include <argp.h>
#include <argz.h>
#include <errno.h>
#include <error.h>
#include <stddef.h>

#include <fcntl.h>
#include <hurd.h>
#include <mach.h>
#include <device/device.h>
#include <hurd/trivfs.h>
#include <hurd/ports.h>
#include <hurd/ihash.h>
#include <hurd/fshelp.h>

#include "ourdevice_S.h"
#include "notify_S.h"
#include "bpf_impl.h"
#include "util.h"

struct proxy_user
{
  struct port_info pi;
  struct proxy *proxy;
};

struct proxy_device
{
  struct port_info pi;
  struct proxy *proxy;
};

struct proxy
{
  struct proxy_device *device;
  mach_port_t deliver_port;
  hurd_ihash_locp_t p_deliverport_hashloc;
  mach_port_t device_port;
};

int deliver_msg (struct net_rcv_msg *msg, queue_head_t *port_list, 
		 mach_port_t dest);

static struct hurd_ihash proxy_deliverport_ht
= HURD_IHASH_INITIALIZER (offsetof (struct proxy, p_deliverport_hashloc));

/* The name of the network interface that the filter translator sits on. */
static char *device_file;
const char *argp_program_version = "eth-filter 0.1";
const char *argp_program_bug_address = "<bug-hurd@gnu.org>";
static const char doc[] = "Hurd filter translator.";
static const struct argp_option options[] =
{
    {"interface", 'i', "DEVICE", 0,
      "Network interface to use", 2},
    {"send-filter", 's', "string", 0,
      "The filter rule which applies to the outgoing packet", 4},
    {"receive-filter", 'r', "string", 0,
      "The filter rule which applies to the ingoing packet", 5},
    {"send-ip-range", 'S', "IP range", 0,
      "A range of IP to create the send filter", 6},
    {"receive-ip-range", 'R', "IP range", 0,
      "A range of IP to create the receive filter", 7},
    {0}
};

/* A filter allows every packet to be sent and be received. */
static struct bpf_insn default_snd_filter[] = 
{
    {NETF_OUT|NETF_BPF, 0, 0, 0},
    {6, 0, 0, 1500}
};
static struct bpf_insn default_rcv_filter[] = 
{
    {NETF_IN|NETF_BPF, 0, 0, 0},
    {6, 0, 0, 1500}
};
static char *snd_string = NULL;
static struct bpf_insn *snd_filter = NULL;
static int snd_filter_length;
static char *rcv_string = NULL;
static struct bpf_insn *rcv_filter = NULL;
static int rcv_filter_length;

/* Port bucket we service requests on.  */
struct port_bucket *port_bucket;

struct port_class *user_portclass;
struct port_class *device_portclass;
struct port_class *other_portclass;

/* Trivfs hooks.  */
int trivfs_fstype = FSTYPE_MISC;
int trivfs_fsid = 0;
int trivfs_support_read = 0;
int trivfs_support_write = 0;
int trivfs_support_exec = 0;
int trivfs_allow_open = O_READ | O_WRITE;

struct port_class *trivfs_protid_portclasses[1];
struct port_class *trivfs_cntl_portclasses[1];
int trivfs_protid_nportclasses = 1;
int trivfs_cntl_nportclasses = 1;

/* For getting the notification of ports from the kernel. */
struct port_info *notify_pi;

error_t
create_proxy_user (struct proxy *proxy, mach_port_t *port)
{
  error_t err;
  struct proxy_user *user;

  err = ports_create_port (user_portclass, port_bucket, sizeof (*user), &user);
  if (err)
      return err;
  user->proxy = proxy;

  *port = ports_get_right (user);
  ports_port_deref (user);
  return 0;
}

error_t 
create_proxy_device (struct proxy *proxy, mach_port_t *port)
{
  error_t err;
  struct proxy_device *device;

  err = ports_create_port (device_portclass, port_bucket, sizeof (*device), &device);
  if (err)
      return err;
  device->proxy = proxy;
  proxy->device = device;

  *port = ports_get_right (device);
  ports_port_deref (device);
  return 0;
}

void
clean_proxy_user (void *p)
{
  struct proxy_user *user = p;
  struct proxy *proxy = user->proxy;

  if (proxy->p_deliverport_hashloc)
    hurd_ihash_locp_remove (&proxy_deliverport_ht, proxy->p_deliverport_hashloc);

  if (proxy->deliver_port != MACH_PORT_NULL)
    mach_port_deallocate (mach_task_self (), proxy->deliver_port);
  if (proxy->device_port != MACH_PORT_NULL)
    mach_port_deallocate (mach_task_self (), proxy->device_port);

  if (proxy->device)
    ports_destroy_right (proxy->device);

  free (proxy);
}

void
clean_proxy_device (void *p)
{
  struct proxy_device *device = p;
  if (device->proxy)
    device->proxy->device = NULL;
}

static int
filter_demuxer (mach_msg_header_t *inp,
		mach_msg_header_t *outp)
{
  extern int device_server (mach_msg_header_t *, mach_msg_header_t *);
  extern int notify_server (mach_msg_header_t *, mach_msg_header_t *);
  extern int ethernet_demuxer (mach_msg_header_t *, mach_msg_header_t *);
  return device_server (inp, outp) || notify_server (inp, outp)
    || ethernet_demuxer (inp, outp) || trivfs_demuxer (inp, outp);
}

int
ethernet_demuxer (mach_msg_header_t *inp,
		  mach_msg_header_t *outp)
{
  struct net_rcv_msg *msg = (struct net_rcv_msg *) inp;
  struct proxy_device *device;
  struct proxy *proxy;
  mach_msg_header_t header;

  if (inp->msgh_id != NET_RCV_MSG_ID)
    return 0;

  device = ports_lookup_port (port_bucket, inp->msgh_local_port, device_portclass);
  if (device == NULL)
    return 0;
  proxy = device->proxy;
  ports_port_deref (device);

  if (proxy && proxy->deliver_port != MACH_PORT_NULL)
    {
      header = msg->msg_hdr;
      deliver_msg (msg, &rcv_port_list, proxy->deliver_port);
      msg->msg_hdr = header;
    }

  return 1;
}

/* Implementation of notify interface */
kern_return_t
do_mach_notify_port_deleted (struct port_info *pi,
			     mach_port_t name)
{
  return EOPNOTSUPP;
}

kern_return_t
do_mach_notify_msg_accepted (struct port_info *pi,
			     mach_port_t name)
{
  return EOPNOTSUPP;
}

kern_return_t
do_mach_notify_port_destroyed (struct port_info *pi,
			       mach_port_t port)
{
  return EOPNOTSUPP;
}

kern_return_t
do_mach_notify_no_senders (struct port_info *pi,
			   mach_port_mscount_t mscount)
{
  debug ("do_mach_notify_no_senders is called\n");
  return ports_do_mach_notify_no_senders (pi, mscount);
}

kern_return_t
do_mach_notify_send_once (struct port_info *pi)
{
  return EOPNOTSUPP;
}

kern_return_t
do_mach_notify_dead_name (struct port_info *pi,
			  mach_port_t name)
{
  struct proxy *proxy;

  debug ("do_mach_notify_dead_name is called\n");
  mach_port_deallocate (mach_task_self (), name);
  proxy = hurd_ihash_find (&proxy_deliverport_ht, name);
  if (proxy)
    {
      proxy->deliver_port = MACH_PORT_NULL;
      return 0;
    }
  return EINVAL;
}

/* Implementation of device interface */
kern_return_t 
ds_xxx_device_set_status (device_t device, dev_flavor_t flavor,
			  dev_status_t status, size_t statu_cnt)
{
  return D_INVALID_OPERATION;
}

kern_return_t
ds_xxx_device_get_status (device_t device, dev_flavor_t flavor,
			  dev_status_t status, size_t *statuscnt)
{
  return D_INVALID_OPERATION;
}

kern_return_t
ds_xxx_device_set_filter (device_t device, mach_port_t rec,
			  int pri, filter_array_t filt, size_t len)
{
  return D_INVALID_OPERATION;
}

kern_return_t
ds_device_open (mach_port_t master_port, mach_port_t reply_port,
		mach_msg_type_name_t reply_portPoly,
		dev_mode_t mode, dev_name_t name, mach_port_t *device, 
		mach_msg_type_name_t *devicetype)
{
  kern_return_t err;
  mach_port_t master_device;
  mach_port_t user_port;
  int file_mode = 0;
  struct proxy *proxy;

  if (device_file == NULL)
    return D_NO_SUCH_DEVICE;

  if (mode | D_WRITE)
    file_mode |= O_WRITE;
  if (mode | D_READ)
    file_mode |= O_READ;
  master_device = file_name_lookup (device_file, file_mode, 0);
  if (master_device == MACH_PORT_NULL)
    return errno;

  proxy = (struct proxy *)calloc (1, sizeof (*proxy));
  if (proxy == NULL)
    {
      mach_port_deallocate (mach_task_self (), master_device);
      return D_NO_MEMORY;
    }

  err = device_open (master_device, mode, name, &proxy->device_port);
  mach_port_deallocate (mach_task_self (), master_device);
  if (err != KERN_SUCCESS)
    {
      free (proxy);
      return err;
    }

  err = create_proxy_user (proxy, &user_port);
  if (err)
    {
      free (proxy);
      return err;
    }

  *device = user_port;
  *devicetype = MACH_MSG_TYPE_MAKE_SEND;

  return 0;
}

kern_return_t
ds_device_close (device_t device)
{
  return 0;
}

kern_return_t
ds_device_write (device_t device, mach_port_t reply_port,
		 mach_msg_type_name_t reply_type, dev_mode_t mode,
		 recnum_t recnum, io_buf_ptr_t data, size_t datalen,
		 int *bytes_written)
{
  int ret_count = 0;
  int has_filter = 0;
  net_hash_entry_t entp, *hash_headp;
  net_rcv_port_t infp, nextfp;
  struct proxy_user *user;
  struct proxy *proxy;

  user = ports_lookup_port (port_bucket, device, user_portclass);
  if (user == NULL)
    {
      vm_deallocate (mach_task_self (), (vm_address_t) data, datalen);
      return D_INVALID_OPERATION;
    }
  proxy = user->proxy;
  ports_port_deref (user);

  /* The packet can be sent as long as it passes one filter,
   * even thought there is usually only one filter in the list. */
  FILTER_ITERATE (&snd_port_list, infp, nextfp, &infp->chain) 
    {
      has_filter = 1;
      ret_count = mach_bpf_do_filter (infp,
				      data + sizeof (struct ethhdr),
				      datalen - sizeof (struct ethhdr),
				      data, sizeof (struct ethhdr),
				      &hash_headp, &entp);
      if (ret_count)
	break;
    }
  FILTER_ITERATE_END

  error_t err;
  if (ret_count || !has_filter) 
    {
      print_pack (data, datalen);
      debug ("before writing a packet from the device.\n");
      err = device_write (proxy->device_port, mode , recnum ,
			  data, datalen, bytes_written);
      debug ("after writing a packet from the device.\n");
    }
  else 
    {
      *bytes_written = datalen;
      err = 0;
    }
  vm_deallocate (mach_task_self (), (vm_address_t) data, datalen);
  return err;
}

kern_return_t
ds_device_write_inband (device_t device, mach_port_t reply_port,
			mach_msg_type_name_t reply_type, dev_mode_t mode,
			recnum_t recnum, io_buf_ptr_inband_t data,
			size_t datalen, int *bytes_written)
{
  kern_return_t ret;
  struct proxy_user *user;
  struct proxy *proxy;

  user = ports_lookup_port (port_bucket, device, user_portclass);
  if (user == NULL)
    return D_INVALID_OPERATION;
  proxy = user->proxy;
  ports_port_deref (user);

  ret = device_write_inband (proxy->device_port, mode, recnum, data,
			     datalen, bytes_written);
  return ret;
}

kern_return_t
ds_device_read (device_t device, mach_port_t reply_port,
		mach_msg_type_name_t reply_type, dev_mode_t mode,
		recnum_t recnum, int bytes_wanted,
		io_buf_ptr_t *data, size_t *datalen)
{
  kern_return_t ret;
  struct proxy_user *user;
  struct proxy *proxy;

  user = ports_lookup_port (port_bucket, device, user_portclass);
  if (user == NULL)
    return D_INVALID_OPERATION;
  proxy = user->proxy;
  ports_port_deref (user);

  ret = device_read (proxy->device_port, mode, recnum,
		     bytes_wanted, data, datalen);
  return ret;
}

kern_return_t
ds_device_read_inband (device_t device, mach_port_t reply_port,
		       mach_msg_type_name_t reply_type, dev_mode_t mode,
		       recnum_t recnum, int bytes_wanted,
		       io_buf_ptr_inband_t data, size_t *datalen)
{
  kern_return_t ret;
  struct proxy_user *user;
  struct proxy *proxy;

  user = ports_lookup_port (port_bucket, device, user_portclass);
  if (user == NULL)
    return D_INVALID_OPERATION;
  proxy = user->proxy;
  ports_port_deref (user);

  ret = device_read_inband (proxy->device_port, mode, recnum, 
			    bytes_wanted, data, datalen);
  return ret;
}

kern_return_t
ds_device_map (device_t device, vm_prot_t prot, vm_offset_t offset,
	       vm_size_t size, memory_object_t *pager, int unmap)
{
  kern_return_t ret;
  struct proxy_user *user;
  struct proxy *proxy;

  user = ports_lookup_port (port_bucket, device, user_portclass);
  if (user == NULL)
    return D_INVALID_OPERATION;
  proxy = user->proxy;
  ports_port_deref (user);

  ret = device_map (proxy->device_port, prot, offset,
		    size, pager, unmap);
  return ret;
}

kern_return_t
ds_device_set_status (device_t device, dev_flavor_t flavor,
		      dev_status_t status, size_t statuslen)
{
  kern_return_t ret;
  struct proxy_user *user;
  struct proxy *proxy;

  user = ports_lookup_port (port_bucket, device, user_portclass);
  if (user == NULL)
    return D_INVALID_OPERATION;
  proxy = user->proxy;
  ports_port_deref (user);

  ret = device_set_status (proxy->device_port, flavor,
			   status, statuslen);
  return ret;
}

kern_return_t
ds_device_get_status (device_t device, dev_flavor_t flavor,
		      dev_status_t status, size_t *statuslen)
{
  kern_return_t ret;
  struct proxy_user *user;
  struct proxy *proxy;

  user = ports_lookup_port (port_bucket, device, user_portclass);
  if (user == NULL)
    return D_INVALID_OPERATION;
  proxy = user->proxy;
  ports_port_deref (user);

  ret = device_get_status (proxy->device_port, flavor, status, statuslen);
  return ret;
}

kern_return_t
ds_device_set_filter (device_t device, mach_port_t receive_port,
		      int priority, filter_array_t filter, size_t filterlen)
{
  mach_port_t tmp;
  kern_return_t err;
  mach_port_t device_receive_port;
  struct proxy_user *user;
  struct proxy *proxy;

  user = ports_lookup_port (port_bucket, device, user_portclass);
  if (user == NULL)
    return D_INVALID_OPERATION;
  proxy = user->proxy;
  ports_port_deref (user);

  if (proxy->device == NULL)
    {
      error_t err;
      err = create_proxy_device (proxy, &device_receive_port);
      if (err)
	return err;
    }
  else
    device_receive_port = ports_get_right (proxy->device);

  /* Set the filter from pfinet into the interface,
   * but the packet will be delivered to the translator,
   * so the translator has the chance to filter some packets. */
  err = device_set_filter (proxy->device_port,
			   device_receive_port,
			   MACH_MSG_TYPE_MAKE_SEND, priority,
			   filter, filterlen);
  if (err)
    return err;

  proxy->deliver_port = receive_port;
  hurd_ihash_add (&proxy_deliverport_ht, receive_port, proxy);

  err = mach_port_request_notification (mach_task_self (), receive_port, 
					MACH_NOTIFY_DEAD_NAME, 0,
					ports_get_right (notify_pi), 
					MACH_MSG_TYPE_MAKE_SEND_ONCE, &tmp);
  if (tmp != MACH_PORT_NULL)
    mach_port_deallocate (mach_task_self (), tmp);

  return err;
}

void
trivfs_modify_stat (struct trivfs_protid *cred, io_statbuf_t *stat)
{
}

error_t
trivfs_goaway (struct trivfs_control *fsys, int flags)
{
  int count;

  /* Stop new requests.  */
  ports_inhibit_class_rpcs (trivfs_cntl_portclasses[0]);
  ports_inhibit_class_rpcs (trivfs_protid_portclasses[0]);

  count = ports_count_class (user_portclass);
  debug ("the number of ports alive: %d\n", count);

  if (count && !(flags & FSYS_GOAWAY_FORCE)) 
    {
      /* We won't go away, so start things going again...  */
      ports_enable_class (trivfs_protid_portclasses[0]);
      ports_resume_class_rpcs (trivfs_cntl_portclasses[0]);
      ports_resume_class_rpcs (trivfs_protid_portclasses[0]); 
      return EBUSY;
    } 

  debug ("the translator is gone away\n");
  exit (0);
}

/* Convert the network address input by the user into
 * a form that is accepted by libpcap. */
int
correct_net_addr (char *orig, char *result, int result_len)
{
  char *ptr;
  int netmask_len;
  int remain_bits;
  int remain_bytes;
  char netmask;
  char addr[4];
  char buf[INET_ADDRSTRLEN];
  int i;

  ptr = strstr (orig, "/");
  if (ptr == NULL)
    {
      strncpy (result, orig, result_len);
      return 0;
    }

  *ptr = 0;
  ptr++;
  netmask_len = atoi (ptr);
  if (inet_pton (AF_INET, orig, addr) < 0)
    {
      perror ("inet_pton");
      return -1;
    }
  remain_bits = netmask_len % 8;
  netmask = ~0;
  netmask >>= 8 - remain_bits;
  netmask <<= 8 - remain_bits;
  remain_bytes = netmask_len / 8;
  addr[remain_bytes] &= netmask;
  for (i=remain_bytes+1 ; i < 4 ; i++)
    addr[i] = 0;

  snprintf (result, result_len, "%s/%s",
	    inet_ntop (AF_INET, addr, buf, INET_ADDRSTRLEN), ptr);
  return 0;
}

static error_t
parse_opt (int opt, char *arg, struct argp_state *state)
{
  struct bpf_insn *trans_filter_program (char *str, int send,
					 int *filter_len);
  char buf[1024];
  char addr[INET_ADDRSTRLEN+4];

  switch (opt)
    {
    case 'i':
      device_file = arg;
      break;
    case 's':
      if (snd_filter && snd_filter != default_snd_filter)
	free (snd_filter);
      if (snd_string)
	free (snd_string);
      snd_string = strdup (arg);
      snd_filter = trans_filter_program (arg, 1, &snd_filter_length);
      if (!snd_filter)
	return ARGP_ERR_UNKNOWN;
      break;
    case 'r':
      if (rcv_filter && rcv_filter != default_rcv_filter)
	free (rcv_filter);
      if (rcv_string)
	free (rcv_string);
      rcv_string = strdup (arg);
      rcv_filter = trans_filter_program (arg, 0, &rcv_filter_length);
      if (!rcv_filter)
	return ARGP_ERR_UNKNOWN;
      break;
    case 'S':
      if (correct_net_addr (arg, addr, INET_ADDRSTRLEN+4) < 0)
	return 0;
      snprintf (buf, sizeof (buf), "arp or (ip and src net %s)", addr);
      if (snd_filter && snd_filter != default_snd_filter) 
	free (snd_filter);
      if (snd_string)
	free (snd_string);
      snd_string = strdup (buf);
      snd_filter = trans_filter_program (buf, 1, &snd_filter_length);
      if (!snd_filter)
	return ARGP_ERR_UNKNOWN;
      break;
    case 'R':
      if (correct_net_addr (arg, addr, INET_ADDRSTRLEN+4) < 0)
	return 0;
      snprintf (buf, sizeof (buf), "arp or (ip and dst net %s)", addr);
      if (rcv_filter && rcv_filter != default_rcv_filter) 
	free (rcv_filter);
      if (rcv_string)
	free (rcv_string);
      rcv_string = strdup (buf);
      rcv_filter = trans_filter_program (buf, 0, &rcv_filter_length);
      if (!rcv_filter)
	return ARGP_ERR_UNKNOWN;
      break;
    case ARGP_KEY_ERROR:
    case ARGP_KEY_SUCCESS:
    case ARGP_KEY_INIT:
      break;
    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}

static const struct argp argp = { options, parse_opt, 0, doc };

error_t
trivfs_append_args (struct trivfs_control *fsys,
		    char **argz, size_t *argz_len)
{
  char *opt;

  if (device_file)
    {
      asprintf(&opt, "--interface=\"%s\"", device_file);
      argz_add (argz, argz_len, opt);
      free (opt);
    }

  if (snd_string)
    {
      asprintf(&opt, "--send-filter=\"%s\"", snd_string);
      argz_add (argz, argz_len, opt);
      free (opt);
    }

  if (rcv_string)
    {
      asprintf(&opt, "--receive-filter=\"%s\"", rcv_string);
      argz_add (argz, argz_len, opt);
      free (opt);
    }

  return 0;
}

error_t
trivfs_set_options (struct trivfs_control *fsys, char *argz, size_t argz_len)
{
  error_t err;

  err = fshelp_set_options (&argp, 0, argz, argz_len, fsys);
  if (err)
    return err;

  err = net_set_filter (MACH_PORT_DEAD, 0, (filter_t *) snd_filter,
			snd_filter_length);
  if (err)
    return err;

  err = net_set_filter (MACH_PORT_DEAD, 0, (filter_t *) rcv_filter,
			rcv_filter_length);
  if (err)
    return err;

  return 0;
}

int
main (int argc, char *argv[])
{
  error_t err;
  mach_port_t bootstrap;
  struct trivfs_control *fsys;

  port_bucket = ports_create_bucket ();
  user_portclass = ports_create_class (clean_proxy_user, 0);
  device_portclass = ports_create_class (clean_proxy_device, 0);
  other_portclass = ports_create_class (0, 0);
  trivfs_cntl_portclasses[0] = ports_create_class (trivfs_clean_cntl, 0);
  trivfs_protid_portclasses[0] = ports_create_class (trivfs_clean_protid, 0);

  argp_parse (&argp, argc, argv, 0, 0, 0);

  /* Prepare the filter. */
  if (snd_filter == NULL) 
    {
      snd_filter = default_snd_filter;
      snd_filter_length = sizeof (default_snd_filter) / sizeof (short);
    }
  if (rcv_filter == NULL) 
    {
      rcv_filter = default_rcv_filter;
      rcv_filter_length = sizeof (default_rcv_filter) / sizeof (short);
    }

  task_get_bootstrap_port (mach_task_self (), &bootstrap);
  if (bootstrap == MACH_PORT_NULL)
    error (1, 0, "must be started as a translator");

  err = ports_create_port (other_portclass, port_bucket, 
			   sizeof (struct port_info), &notify_pi);
  if (err)
    error (1, err, "ports_create_port for notification");

  /* Reply to our parent.  */
  err = trivfs_startup (bootstrap, 0,
			trivfs_cntl_portclasses[0], port_bucket,
			trivfs_protid_portclasses[0], port_bucket, &fsys);
  mach_port_deallocate (mach_task_self (), bootstrap);
  if (err)
    error (1, err, "Contacting parent");

  /* Initialize the bpf, and set the filter for outgoing packets.
   * MACH_PORT_DEAD is used because we don't need a receiving port. */
  bpf_init ();
  err = net_set_filter (MACH_PORT_DEAD, 0, (filter_t *) snd_filter,
			snd_filter_length);
  if (err)
    error (1, err, "set the sending filter");
  /* Set the filter translator's own rule. */
  err = net_set_filter (MACH_PORT_DEAD, 0, (filter_t *) rcv_filter,
			rcv_filter_length);
  if (err)
    error (1, err, "set the receiving filter");

  /* Launch.  */
  do 
    {
      ports_manage_port_operations_one_thread (port_bucket, 
					       filter_demuxer, 0);
    } while (trivfs_goaway (fsys, 0)); 
  return 0;
}

int
deliver_msg (struct net_rcv_msg *msg, queue_head_t *port_list, 
	     mach_port_t dest)
{
  int ret_count = 0;
  int has_filter = 0;
  mach_msg_return_t err;
  net_rcv_port_t infp, nextfp;

  msg->msg_hdr.msgh_bits = MACH_MSGH_BITS (MACH_MSG_TYPE_COPY_SEND, 0);
  msg->msg_hdr.msgh_local_port = MACH_PORT_NULL;
  msg->msg_hdr.msgh_kind = MACH_MSGH_KIND_NORMAL;

  /* Deliver the packet to the right pfinet,
   * actually there should be only one filter in the list. */
  FILTER_ITERATE (port_list, infp, nextfp, &infp->chain) 
    {
      net_hash_entry_t entp, *hash_headp;

      entp = (net_hash_entry_t) 0;
      ret_count = mach_bpf_do_filter (infp,
				      msg->packet + sizeof (struct packet_header),
				      msg->net_rcv_msg_packet_count,
				      msg->header,
				      sizeof (struct ethhdr),
				      &hash_headp, &entp);

      if (ret_count) 
	break;
    }
  FILTER_ITERATE_END
    
    if (ret_count || !has_filter)
      {
	msg->msg_hdr.msgh_remote_port = dest;
	debug ("before delivering the message\n");
	err = mach_msg ((mach_msg_header_t *)msg,
			MACH_SEND_MSG|MACH_SEND_TIMEOUT,
			msg->msg_hdr.msgh_size, 0, MACH_PORT_NULL,
			0, MACH_PORT_NULL);
	if (err != MACH_MSG_SUCCESS)
	  {
	    fprintf (stderr, "deliver msg: mach_msg: %s\n",
		     strerror (err));
	    return -1;
	  }
	debug ("after delivering the message\n");
      }

    return 0;
}
