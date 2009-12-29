#ifndef _ddekit_panic_h
#define _ddekit_panic_h

/** \defgroup DDEKit_util */

#include "c_headers.h"

/** Panic - print error message and enter the kernel debugger.
 * \ingroup DDEKit_util
 */
#define ddekit_panic(format, ...) do				\
{							\
  char buf[1024];                                       \
  snprintf (buf, 1024, "%s", format);       \
  fprintf (stderr , buf, ## __VA_ARGS__);		\
  fflush (stderr);					\
  abort ();			    \
} while (0)

/** Print a debug message.
 * \ingroup DDEKit_util
 */
#define ddekit_debug(format, ...) do				\
{							\
  char buf[1024];                                       \
  snprintf (buf, 1024, "%s: %s\n", __func__, format);       \
  fprintf (stderr , buf, ## __VA_ARGS__);		\
  fflush (stderr);					\
} while (0)

#endif
