/*
 * \brief   Logging facility with printf()-like interface
 * \author  Thomas Friebel <yaron@yaron.de>
 * \date    2006-03-01
 */

#include <error.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ddekit/printf.h"

static FILE *output;

/**
 * Log constant string message w/o arguments
 *
 * \param msg  message to be logged
 */
int ddekit_print(const char *msg)
{
	int ret;

	/* If LOG hasn't been initialized or failed its initialization,
	 * return the error. */
	if (output == NULL)
		return -1;

	ret = fprintf (output, "%s", msg);
	if (!ret)
		fflush (output);
	return ret;
}

/**
 * Log message with print()-like arguments
 *
 * \param fmt  format string followed by optional arguments
 */
int ddekit_printf(const char *fmt, ...)
{
	int res;
	va_list va;

	va_start(va, fmt);
	res = ddekit_vprintf(fmt, va);
	va_end(va);

	return res;
}

/* Log message with vprintf()-like arguments
 *
 * \param fmt  format string
 * \param va   variable argument list
 */
int ddekit_vprintf(const char *fmt, va_list va)
{
	char *tmp = NULL;
	int ret;

	ret = vasprintf (&tmp, fmt, va);
	if (!ret) {
		ret = ddekit_print (tmp);
		free (tmp);
	}
	return ret;
}

int log_init ()
{
	char *log_file_name = mktemp ("/tmp/dde_log.XXXXXX");
	output = fopen (log_file_name, "a+");
	if (!output) {
		error (0, errno, "open %s", log_file_name);
		return -1;
	}
	return 0;
}
