#include <mach.h>

#include <ddekit/printf.h>

#include "linux-errno.h"

int
linux_to_mach_error (int err);
{
  switch (err)
    {
    case 0:
      return D_SUCCESS;

    case -EPERM:
      return D_INVALID_OPERATION;

    case -EIO:
      return D_IO_ERROR;

    case -ENXIO:
      return D_NO_SUCH_DEVICE;

    case -EACCES:
      return D_INVALID_OPERATION;

    case -EFAULT:
      return D_INVALID_SIZE;

    case -EBUSY:
      return D_ALREADY_OPEN;

    case -EINVAL:
      return D_INVALID_SIZE;

    case -EROFS:
      return D_READ_ONLY;

    case -EWOULDBLOCK:
      return D_WOULD_BLOCK;

    case -ENOMEM:
      return D_NO_MEMORY;

    default:
      ddekit_printf ("linux_to_mach_error: unknown code %d\n", err);
      return D_IO_ERROR;
    }
}
