/* Linux */
#include <linux/gfp.h>
#include <linux/string.h>
#include <asm/page.h>

#include "local.h"

int ioprio_best(unsigned short aprio, unsigned short bprio)
{
	WARN_UNIMPL;
	return 0;
}

void *__alloc_bootmem(unsigned long size, unsigned long align,
                      unsigned long goal)
{
	WARN_UNIMPL;
	return 0;
}

/*
 * Stolen from linux-2.6.29/fs/libfs.c
 */
ssize_t memory_read_from_buffer(void *to, size_t count, loff_t *ppos,
                                const void *from, size_t available)
{
	loff_t pos = *ppos;
	if (pos < 0)
		return -EINVAL;
	if (pos > available)
		return 0;
	if (count > available - pos)
		count = available - pos;
	memcpy(to, from + pos, count);
	*ppos = pos + count;

	return count;
}

int capable(int f) { return 1; }
