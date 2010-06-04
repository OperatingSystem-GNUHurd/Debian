#include <linux/netdevice.h>
#include <linux/if.h>

/* List of sk_buffs waiting to be freed.  */
static struct sk_buff_head skb_done_list;

struct net_device *search_netdev (char *name)
{
	struct net_device *dev;
	struct net_device *found = NULL;
	struct net *net;

	printk("search device %s\n", name);
	read_lock(&dev_base_lock);
	for_each_net(net) {
		for_each_netdev(net, dev) {
			printk("there is device %s, base addr: %x\n",
			       dev->name, dev->base_addr);
			if (!strcmp (name, dev->name))
			{
				found = dev;
				goto end;
			}
		}
	}
end:
	read_unlock(&dev_base_lock);
	return found;
}

int linux_pkg_xmit (char *pkg_data, int len, void *del_data,
		    int (*del_func) (struct sk_buff *, void *),
		    struct net_device *dev)
{
  struct sk_buff *skb;

  if (len == 0 || len > dev->mtu + dev->hard_header_len)
    return EINVAL;

  /* Allocate a sk_buff.  */
  skb = dev_alloc_skb (len);
  if (!skb)
    return ENOMEM;

  skb->del_data = del_data;
  skb->pre_del_func = del_func;

  /* Copy user data.  This is only required if it spans multiple pages.  */
  skb->len = len;
  skb->tail = skb->data + len;

  memcpy (skb->data, pkg_data, len);

  skb->dev = dev;

  return dev_queue_xmit(skb);
}

char *netdev_addr(struct net_device *dev)
{
  return dev->dev_addr;
}

int netdev_flags(struct net_device *dev)
{
  return dev->flags;
}

void *skb_reply(struct sk_buff *skb)
{
  return skb->del_data;
}

void skb_done_head_init()
{
  skb_queue_head_init (&skb_done_list);
}

struct sk_buff *skb_done_dequeue()
{
  return skb_dequeue (&skb_done_list);
}

void skb_done_queue(struct sk_buff *skb)
{
  skb_queue_tail (&skb_done_list, skb);
}
