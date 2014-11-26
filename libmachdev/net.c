/*
 * Linux network driver support.
 *
 * Copyright (C) 1996 The University of Utah and the Computer Systems
 * Laboratory at the University of Utah (CSL)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *      Author: Shantanu Goel, University of Utah CSL
 */

/*
 * INET               An implementation of the TCP/IP protocol suite for the LINUX
 *              operating system.  INET is implemented using the  BSD Socket
 *              interface as the means of communication with the user level.
 *
 *              Ethernet-type device handling.
 *
 * Version:     @(#)eth.c       1.0.7   05/25/93
 *
 * Authors:     Ross Biro, <bir7@leland.Stanford.Edu>
 *              Fred N. van Kempen, <waltje@uWalt.NL.Mugnet.ORG>
 *              Mark Evans, <evansmp@uhura.aston.ac.uk>
 *              Florian  La Roche, <rzsfl@rz.uni-sb.de>
 *              Alan Cox, <gw4pts@gw4pts.ampr.org>
 * 
 * Fixes:
 *              Mr Linux        : Arp problems
 *              Alan Cox        : Generic queue tidyup (very tiny here)
 *              Alan Cox        : eth_header ntohs should be htons
 *              Alan Cox        : eth_rebuild_header missing an htons and
 *                                minor other things.
 *              Tegge           : Arp bug fixes. 
 *              Florian         : Removed many unnecessary functions, code cleanup
 *                                and changes for new arp and skbuff.
 *              Alan Cox        : Redid header building to reflect new format.
 *              Alan Cox        : ARP only when compiled with CONFIG_INET
 *              Greg Page       : 802.2 and SNAP stuff.
 *              Alan Cox        : MAC layer pointers/new format.
 *              Paul Gortmaker  : eth_copy_and_sum shouldn't csum padding.
 *              Alan Cox        : Protect against forwarding explosions with
 *                                older network drivers and IFF_ALLMULTI
 *
 *              This program is free software; you can redistribute it and/or
 *              modify it under the terms of the GNU General Public License
 *              as published by the Free Software Foundation; either version
 *              2 of the License, or (at your option) any later version.
 */
#include <assert.h>
#include <string.h>
#include <arpa/inet.h>
#include <error.h>

#include "mach_U.h"

#include <mach.h>
#include <hurd.h>

#define MACH_INCLUDE

#include "ds_routines.h"
#include "vm_param.h"
#include "device_reply_U.h"
#include "dev_hdr.h"
#include "if_ether.h"
#include "util.h"
#include "mach_glue.h"
#include "if_hdr.h"

#define ether_header ethhdr

/* One of these is associated with each instance of a device.  */
struct net_data
{
  struct port_info port;	/* device port */
  struct emul_device device;	/* generic device structure */
  struct ifnet ifnet;		/* Mach ifnet structure (needed for filters) */
  struct net_device *dev;	/* Linux network device structure */
  struct net_data *next;
};

struct skb_reply
{
  mach_port_t reply;
  mach_msg_type_name_t reply_type;
  int pkglen;
};

static struct net_data *nd_head;

/* Forward declarations.  */

extern struct device_emulation_ops linux_net_emulation_ops;

static mach_msg_type_t header_type = 
{
  MACH_MSG_TYPE_BYTE,
  8,
  NET_HDW_HDR_MAX,
  TRUE,
  FALSE,
  FALSE,
  0
};

static mach_msg_type_t packet_type = 
{
  MACH_MSG_TYPE_BYTE,	/* name */
  8,			/* size */
  0,			/* number */
  TRUE,			/* inline */
  FALSE,			/* longform */
  FALSE			/* deallocate */
};

static struct net_data *search_nd (struct net_device *dev)
{
  struct net_data *nd = nd_head;

  //TODO protected by locks.
  while (nd)
    {
      if (nd->dev == dev)
	return nd;
      nd = nd->next;
    }
  return NULL;
}

/* Linux kernel network support routines.  */

/* actions before freeing the sk_buff SKB.
 * If it returns 1, the packet will be deallocated later. */
static int 
pre_kfree_skb (struct sk_buff *skb, void *data)
{
  struct skb_reply *reply = data;
  extern void wakeup_io_done_thread ();

  /* Queue sk_buff on done list if there is a
     page list attached or we need to send a reply.
     Wakeup the iodone thread to process the list.  */
  if (reply && MACH_PORT_VALID (reply->reply))
    {
      if (MACH_PORT_VALID (reply->reply))
	{
	  ds_device_write_reply (reply->reply, reply->reply_type,
				 0, reply->pkglen);
	  reply->reply = MACH_PORT_NULL;
	}
    }
  /* deallocate skb_reply before freeing the packet. */
  free (data);
  return 0;
}

/*
 * Deliver the message to all right pfinet servers that
 * connects to the virtual network interface.
 */
int
deliver_msg(struct net_rcv_msg *msg, if_filter_list_t *ifp)
{
  mach_msg_return_t err;
  queue_head_t *if_port_list;
  net_rcv_port_t infp, nextfp;

  msg->msg_hdr.msgh_bits = MACH_MSGH_BITS (MACH_MSG_TYPE_COPY_SEND, 0);
  /* remember message sizes must be rounded up */
  msg->msg_hdr.msgh_local_port = MACH_PORT_NULL;
  msg->msg_hdr.msgh_kind = MACH_MSGH_KIND_NORMAL;
  msg->msg_hdr.msgh_id = NET_RCV_MSG_ID;

  if_port_list = &ifp->if_rcv_port_list;
  FILTER_ITERATE (if_port_list, infp, nextfp, &infp->input) 
    {
      mach_port_t dest;
      net_hash_entry_t entp, *hash_headp;
      int ret_count;

      entp = (net_hash_entry_t) 0;
      ret_count = bpf_do_filter (infp,
				 msg->packet + sizeof (struct packet_header),
				 msg->net_rcv_msg_packet_count, msg->header,
				 sizeof (struct ethhdr), &hash_headp, &entp);
      if (entp == (net_hash_entry_t) 0)
	dest = infp->rcv_port;
      else
	dest = entp->rcv_port;

      if (ret_count) 
	{
	  msg->msg_hdr.msgh_remote_port = dest;
	  err = mach_msg ((mach_msg_header_t *)msg,
			  MACH_SEND_MSG|MACH_SEND_TIMEOUT,
			  msg->msg_hdr.msgh_size, 0, MACH_PORT_NULL,
			  0, MACH_PORT_NULL);
	  if (err != MACH_MSG_SUCCESS)
	    {
	      /* TODO: remove from filter */
	    }
	}
    }
  FILTER_ITERATE_END

    return 0;
}

/* Accept packet SKB received on an interface.  */
static void
netif_rx_handle (char *data, int len, struct net_device *dev)
{
  int pack_size;
  struct net_rcv_msg net_msg;
  struct ether_header *eh;
  struct packet_header *ph;
  struct net_data *nd;

  nd = search_nd(dev);
  assert (nd);

  pack_size = len - sizeof (struct ethhdr);
  /* remember message sizes must be rounded up */
  net_msg.msg_hdr.msgh_size =
    (((mach_msg_size_t) (sizeof (struct net_rcv_msg)
			 - sizeof net_msg.sent
			 + sizeof (struct packet_header)
			 - NET_RCV_MAX + pack_size)) + 3) & ~3;

  /* Copy packet into message buffer.  */
  eh = (struct ether_header *) (net_msg.header);
  ph = (struct packet_header *) (net_msg.packet);
  memcpy (eh, data, sizeof (struct ether_header));
  /* packet is prefixed with a struct packet_header,
     see include/device/net_status.h.  */
  memcpy (ph + 1, data + sizeof (struct ether_header), pack_size);
  ph->type = eh->h_proto;
  ph->length = pack_size + sizeof (struct packet_header);

  net_msg.sent = FALSE; /* Mark packet as received.  */

  net_msg.header_type = header_type;
  net_msg.packet_type = packet_type;
  net_msg.net_rcv_msg_packet_count = ph->length;
  deliver_msg (&net_msg, &nd->ifnet.port_list);
}

/* Mach device interface routines.  */

/* Return a send right associated with network device ND.  */
static mach_port_t
dev_to_port (void *nd)
{
  return (nd
	  ? ports_get_send_right (nd)
	  : MACH_PORT_NULL);
}

/*    
 * Initialize send and receive queues on an interface.
 */   
void if_init_queues(ifp)
     register struct ifnet *ifp;
{     
//  IFQ_INIT(&ifp->if_snd);
  queue_init(&ifp->port_list.if_rcv_port_list);
  queue_init(&ifp->port_list.if_snd_port_list);
  pthread_mutex_init(&ifp->if_rcv_port_list_lock, NULL);
  pthread_mutex_init(&ifp->if_snd_port_list_lock, NULL);
}

static io_return_t
device_open (mach_port_t reply_port, mach_msg_type_name_t reply_port_type,
	     dev_mode_t mode, char *name, device_t *devp,
	     mach_msg_type_name_t *devicePoly)
{
  io_return_t err = D_SUCCESS;
  struct net_device *dev;
  struct net_data *nd;
  struct ifnet *ifp;

  /* Search for the device.  */
  dev = search_netdev (name);
  if (!dev)
    {
      fprintf (stderr, "after search_netdev: cannot find %s\n", name);
      return D_NO_SUCH_DEVICE;
    }

  /* Allocate and initialize device data if this is the first open.  */
  nd = search_nd (dev);
  if (!nd)
    {
      char *name;

      err = create_device_port (sizeof (*nd), &nd);
      if (err)
	{
	  fprintf (stderr, "after create_device_port: cannot create a port\n");
	  goto out;
	}
	
      nd->dev = dev;
      nd->device.emul_data = nd;
      nd->device.emul_ops = &linux_net_emulation_ops;
      nd->next = nd_head;
      nd_head = nd;

      ifp = &nd->ifnet;
      name = netdev_name (dev);
      ifp->if_unit = name[strlen (name) - 1] - '0';
      ifp->if_flags = IFF_UP | IFF_RUNNING;
      ifp->if_mtu = netdev_mtu (dev);
      ifp->if_header_size = netdev_header_len (dev);
      ifp->if_header_format = netdev_type (dev);
      ifp->if_address_size = netdev_addr_len (dev);
      ifp->if_address = netdev_addr (dev);
      if_init_queues (ifp);

      if ((err = dev_open(dev)) < 0)
	{
	  fprintf (stderr, "after dev_open: cannot open the device\n");
	  err = linux_to_mach_error (err);
	}

    out:
      if (err)
	{
	  if (nd)
	    {
	      ports_destroy_right (nd);
	      nd = NULL;
	    }
	}
      else
	{
#if 0
	  /* IPv6 heavily relies on multicasting (especially router and
	     neighbor solicits and advertisements), so enable reception of
	     those multicast packets by setting `LINUX_IFF_ALLMULTI'.  */
	  dev->flags |= LINUX_IFF_UP | LINUX_IFF_RUNNING | LINUX_IFF_ALLMULTI;
	  skb_queue_head_init (&dev->buffs[0]);

	  if (dev->set_multicast_list)
	    dev->set_multicast_list (dev);
#endif
	}
    }

  if (nd)
    {
      *devp = ports_get_right (nd);
      *devicePoly = MACH_MSG_TYPE_MAKE_SEND;
    }
  return err;
}

static io_return_t
device_write (void *d, mach_port_t reply_port,
	      mach_msg_type_name_t reply_port_type, dev_mode_t mode,
	      recnum_t bn, io_buf_ptr_t data, unsigned int count,
	      int *bytes_written)
{
  struct net_data *nd = d;
  struct net_device *dev = nd->dev;
  struct skb_reply *skb_reply = malloc (sizeof (*skb_reply));
  error_t err;

  if (skb_reply == NULL)
    return D_NO_MEMORY;

  skb_reply->pkglen = count;
  skb_reply->reply = reply_port;
  skb_reply->reply_type = reply_port_type;

  err = linux_pkg_xmit (data, count, skb_reply, pre_kfree_skb, dev);
  vm_deallocate (mach_task_self (), (vm_address_t) data, count);
  if (err)
    return linux_to_mach_error (err);

  /* Send packet to filters.  */
  // TODO should I deliver the packet to other network stacks?
#if 0
  {
    struct packet_header *packet;
    struct ether_header *header;
    ipc_kmsg_t kmsg;

    kmsg = net_kmsg_get ();

    if (kmsg != IKM_NULL)
      {
        /* Suitable for Ethernet only.  */
        header = (struct ether_header *) (net_kmsg (kmsg)->header);
        packet = (struct packet_header *) (net_kmsg (kmsg)->packet);
        memcpy (header, skb->data, sizeof (struct ether_header));

        /* packet is prefixed with a struct packet_header,
           see include/device/net_status.h.  */
        memcpy (packet + 1, skb->data + sizeof (struct ether_header),
                skb->len - sizeof (struct ether_header));
        packet->length = skb->len - sizeof (struct ether_header)
                         + sizeof (struct packet_header);
        packet->type = header->ether_type;
        net_kmsg (kmsg)->sent = TRUE; /* Mark packet as sent.  */
        s = splimp ();
        net_packet (&dev->net_data->ifnet, kmsg, packet->length,
                    ethernet_priority (kmsg));
        splx (s);
      }
  }
#endif

  return MIG_NO_REPLY;
}

/*
 * Other network operations
 */
static io_return_t
net_getstat(ifp, flavor, status, count)
	struct ifnet	*ifp;
	dev_flavor_t	flavor;
	dev_status_t	status;		/* pointer to OUT array */
	natural_t	*count;		/* OUT */
{
#define ETHERMTU 1500
	switch (flavor) {
	    case NET_STATUS:
	    {
		register struct net_status *ns = (struct net_status *)status;

		if (*count < NET_STATUS_COUNT)
		    return (D_INVALID_OPERATION);
		
		ns->min_packet_size = ifp->if_header_size;
		ns->max_packet_size = ifp->if_header_size + ifp->if_mtu;
		ns->header_format   = ifp->if_header_format;
		ns->header_size	    = ifp->if_header_size;
		ns->address_size    = ifp->if_address_size;
		ns->flags	    = ifp->if_flags;
		ns->mapped_size	    = 0;

		*count = NET_STATUS_COUNT;
		break;
	    }
	    case NET_ADDRESS:
	    {
		register int	addr_byte_count;
		register int	addr_int_count;
		register int	i;

		addr_byte_count = ETH_ALEN;
		addr_int_count = (addr_byte_count + (sizeof(int)-1))
					 / sizeof(int);

		if (*count < addr_int_count)
		{
		  /* XXX debug hack. */
		  printf ("net_getstat: count: %d, addr_int_count: %d\n",
			  *count, addr_int_count);
		  return (D_INVALID_OPERATION);
		}

		memcpy(status, ifp->if_address, addr_byte_count);
		if (addr_byte_count < addr_int_count * sizeof(int))
		    memset((char *)status + addr_byte_count, 0, 
			  (addr_int_count * sizeof(int)
				      - addr_byte_count));

		for (i = 0; i < addr_int_count; i++) {
		    register int word;

		    word = status[i];
		    status[i] = htonl(word);
		}
		*count = addr_int_count;
		break;
	    }
	    default:
		return (D_INVALID_OPERATION);
	}
	return (D_SUCCESS);
}

static io_return_t
device_get_status (void *d, dev_flavor_t flavor, dev_status_t status,
		   mach_msg_type_number_t *count)
{
  struct net_data *net = (struct net_data *) d;

  if (flavor == NET_FLAGS)
    {
      if (*count != 1)
	return D_INVALID_SIZE;

      *(int *) status = netdev_flags (net->dev);
      return D_SUCCESS;
    }

#if 0
  if(flavor >= SIOCIWFIRST && flavor <= SIOCIWLAST)
    {
      /* handle wireless ioctl */
      if(! IW_IS_GET(flavor))
	return D_INVALID_OPERATION;

      if(*count * sizeof(int) < sizeof(struct ifreq))
	return D_INVALID_OPERATION;

      struct net_data *nd = d;
      struct linux_device *dev = nd->dev;

      if(! dev->do_ioctl)
	return D_INVALID_OPERATION;

      int result;

      if (flavor == SIOCGIWRANGE || flavor == SIOCGIWENCODE
	  || flavor == SIOCGIWESSID || flavor == SIOCGIWNICKN
	  || flavor == SIOCGIWSPY)
	{
	  /*
	   * These ioctls require an `iw_point' as their argument (i.e.
	   * they want to return some data to userspace. 
	   * Therefore supply some sane values and carry the data back
	   * to userspace right behind the `struct iwreq'.
	   */
	  struct iw_point *iwp = &((struct iwreq *) status)->u.data;
	  iwp->length = *count * sizeof (dev_status_t) - sizeof (struct ifreq);
	  iwp->pointer = (void *) status + sizeof (struct ifreq);

	  result = dev->do_ioctl (dev, (struct ifreq *) status, flavor);

	  *count = ((sizeof (struct ifreq) + iwp->length)
		    / sizeof (dev_status_t));
	  if (iwp->length % sizeof (dev_status_t))
	    (*count) ++;
	}
      else
	{
	  *count = sizeof(struct ifreq) / sizeof(int);
	  result = dev->do_ioctl(dev, (struct ifreq *) status, flavor);
	}

      return result ? D_IO_ERROR : D_SUCCESS;
    }
  else
#endif
    {
      /* common get_status request */
      return net_getstat (&net->ifnet, flavor, status, count);
    }
}

static io_return_t
device_set_status(void *d, dev_flavor_t flavor, dev_status_t status,
		  mach_msg_type_number_t count)
{
  if (flavor == NET_FLAGS)
    {
      if (count != 1)
        return D_INVALID_SIZE;

      int flags = *(int *) status;
      struct net_data *net = (struct net_data *) d;

      dev_change_flags (net->dev, flags);

      return D_SUCCESS;
    }
    return D_INVALID_OPERATION;

#if 0
  if(flavor < SIOCIWFIRST || flavor > SIOCIWLAST)
    return D_INVALID_OPERATION;

  if(! IW_IS_SET(flavor))
    return D_INVALID_OPERATION;
  
  if(count * sizeof(int) < sizeof(struct ifreq))
    return D_INVALID_OPERATION;

  struct net_data *nd = d;
  struct linux_device *dev = nd->dev;

  if(! dev->do_ioctl)
    return D_INVALID_OPERATION;

  if((flavor == SIOCSIWENCODE || flavor == SIOCSIWESSID
      || flavor == SIOCSIWNICKN || flavor == SIOCSIWSPY)
     && ((struct iwreq *) status)->u.data.pointer)
    {
      struct iw_point *iwp = &((struct iwreq *) status)->u.data;

      /* safety check whether the status array is long enough ... */
      if(count * sizeof(int) < sizeof(struct ifreq) + iwp->length)
	return D_INVALID_OPERATION;

      /* make sure, iwp->pointer points to the correct address */
      if(iwp->pointer) iwp->pointer = (void *) status + sizeof(struct ifreq);
    }
  
  int result = dev->do_ioctl(dev, (struct ifreq *) status, flavor);
  return result ? D_IO_ERROR : D_SUCCESS;
#endif
}


static io_return_t
device_set_filter (void *d, mach_port_t port, int priority,
		   filter_t * filter, unsigned filter_count)
{
  return net_set_filter (&((struct net_data *) d)->ifnet.port_list,
			 port, priority, filter, filter_count);
}

/* Do any initialization required for network devices.  */
static void linux_net_emulation_init ()
{
  skb_done_head_init();
  l4dde26_register_rx_callback(netif_rx_handle);
}

struct device_emulation_ops linux_net_emulation_ops =
{
  linux_net_emulation_init,
  NULL,
  NULL,
  dev_to_port,
  device_open,
  NULL,
  device_write,
  NULL,
  NULL,
  NULL,
  device_set_status,
  device_get_status,
  device_set_filter,
  NULL,
  NULL,
  NULL,
  NULL
};

void register_net()
{
	extern void reg_dev_emul (struct device_emulation_ops *ops);
	reg_dev_emul (&linux_net_emulation_ops);
}
