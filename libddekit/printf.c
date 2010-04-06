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
#include <mach.h>
#include <execinfo.h>

#include "ddekit/printf.h"

extern boolean_t using_std;
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
	if (ret > 0)
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
	if (ret > 0) {
		ret = ddekit_print (tmp);
		free (tmp);
	}
	return ret;
}

int log_init ()
{
	if (using_std) {
		output = stderr;
	}
	else {
		char template[] = "/var/log/dde_log.XXXXXX";
		int ret = mkstemp (template);
		if (ret < 0) {
			error (0, errno, "mkstemp");
			return -1;
		}

		output = fopen (template, "a+");
		if (!output) {
			error (0, errno, "open %s", template);
			return -1;
		}
	}

	return 0;
}

void dump_stack()
{
#define NUM_TRACES 16
	void *trace[NUM_TRACES];
	int trace_size = 0;

	fprintf (stderr, "dump the stack\n");
	trace_size = backtrace(trace, NUM_TRACES);
	backtrace_symbols_fd(trace, trace_size, 2);
}
