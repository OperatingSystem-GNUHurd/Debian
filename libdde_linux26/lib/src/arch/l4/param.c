#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/slab.h>

/* Lazy bastard, eh? */
#define STANDARD_PARAM_DEF(name, type, format, tmptype, strtolfn)      	\
	int param_set_##name(const char *val, struct kernel_param *kp)	\
	{								\
		return 0;						\
	}								\
	int param_get_##name(char *buffer, struct kernel_param *kp)	\
	{ \
		return 0;\
	}

STANDARD_PARAM_DEF(byte, unsigned char, "%c", unsigned long, simple_strtoul);
STANDARD_PARAM_DEF(short, short, "%hi", long, simple_strtol);
STANDARD_PARAM_DEF(ushort, unsigned short, "%hu", unsigned long, simple_strtoul);
STANDARD_PARAM_DEF(int, int, "%i", long, simple_strtol);
STANDARD_PARAM_DEF(uint, unsigned int, "%u", unsigned long, simple_strtoul);
STANDARD_PARAM_DEF(long, long, "%li", long, simple_strtol);
STANDARD_PARAM_DEF(ulong, unsigned long, "%lu", unsigned long, simple_strtoul);

int param_set_charp(const char *val, struct kernel_param *kp)
{
	if (!val) {
		printk(KERN_ERR "%s: string parameter expected\n",
		       kp->name);
		return -EINVAL;
	}

	if (strlen(val) > 1024) {
		printk(KERN_ERR "%s: string parameter too long\n",
		       kp->name);
		return -ENOSPC;
	}

	*(char **)kp->arg = (char *)val;
	return 0;
}

int param_get_charp(char *buffer, struct kernel_param *kp)
{
	return sprintf(buffer, "%s", *((char **)kp->arg));
}

int param_set_bool(const char *val, struct kernel_param *kp)
{
	/* No equals means "set"... */
	if (!val) val = "1";

	/* One of =[yYnN01] */
	switch (val[0]) {
	case 'y': case 'Y': case '1':
		*(int *)kp->arg = 1;
		return 0;
	case 'n': case 'N': case '0':
		*(int *)kp->arg = 0;
		return 0;
	}
	return -EINVAL;
}

int param_get_bool(char *buffer, struct kernel_param *kp)
{
	/* Y and N chosen as being relatively non-coder friendly */
	return sprintf(buffer, "%c", (*(int *)kp->arg) ? 'Y' : 'N');
}

int param_set_invbool(const char *val, struct kernel_param *kp)
{
	int boolval, ret;
	struct kernel_param dummy;

	dummy.arg = &boolval;
	ret = param_set_bool(val, &dummy);
	if (ret == 0)
		*(int *)kp->arg = !boolval;
	return ret;
}

int param_get_invbool(char *buffer, struct kernel_param *kp)
{
	return sprintf(buffer, "%c", (*(int *)kp->arg) ? 'N' : 'Y');
}

int printk_ratelimit(void)
{
	return 0;
}
