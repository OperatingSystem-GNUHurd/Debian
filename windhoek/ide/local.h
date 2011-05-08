#ifndef LOCAL_H
#define LOCAL_H

#define DEBUG_MSG(msg, ...) {}

//	printk("%s: \033[31m"msg"\033[0m\n", __FUNCTION__, ##__VA_ARGS__)

#define CONFIG_IDE_GD 1
#define CONFIG_IDE_GD_ATA 1

#endif
