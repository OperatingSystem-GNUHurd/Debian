#ifndef __MACH_GLUE_H__
#define __MACH_GLUE_H__

/* network */
#include <arpa/inet.h>

#include <ddekit/thread.h>

struct sk_buff;
struct net_device;
void skb_done_queue(struct sk_buff *skb);
struct sk_buff *skb_done_dequeue();
void *skb_reply(struct sk_buff *skb);

int netdev_flags(struct net_device *dev);
char *netdev_addr(struct net_device *dev);
char *netdev_name (struct net_device *dev);
unsigned int netdev_mtu (struct net_device *dev);
unsigned short netdev_header_len (struct net_device *dev);
unsigned short netdev_type (struct net_device *dev);
unsigned char netdev_addr_len (struct net_device *dev);

int dev_change_flags (struct net_device *dev, short flags);
int linux_pkg_xmit (char *pkg_data, int len, void *del_data,
		    int (*del_func) (struct sk_buff *, void *),
		    struct net_device *dev);
struct net_device *search_netdev (char *name);
void kfree_skb (struct sk_buff *skb);
int dev_open(struct net_device *dev);
void *l4dde26_register_rx_callback(void *cb);
void skb_done_head_init();

/* block device */
struct block_device;
struct block_device *open_block_dev (char *name, int part, dev_mode_t mode);
int block_dev_write (struct block_device *dev, int sectornr,
		     char *data, int count, void (*write_done) (int err));

int l4dde26_process_from_ddekit(ddekit_thread_t *t);

#endif
