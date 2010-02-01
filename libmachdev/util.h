#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdio.h>

#define panic(format, ...) do				\
{							\
  char buf[1024];                                       \
  snprintf (buf, 1024, "devnode: %s", format);       \
  fprintf (stderr , buf, ## __VA_ARGS__);		\
  fflush (stderr);					\
  abort ();			    \
} while (0)

#define DEBUG

#ifdef DEBUG

#define debug(format, ...) do				\
{							\
  char buf[1024];                                       \
  snprintf (buf, 1024, "pcnet32: %s: %s\n", __func__, format);       \
  fprintf (stderr , buf, ## __VA_ARGS__);		\
  fflush (stderr);					\
} while (0)

#else

#define debug(format, ...) do {} while (0)

#endif

#endif
