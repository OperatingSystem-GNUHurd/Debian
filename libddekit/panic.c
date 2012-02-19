#include "ddekit/panic.h"
#include "ddekit/printf.h"

#include <error.h>
#include <stdarg.h>

void ddekit_panic(char *fmt, ...) {
	va_list va;

	va_start(va, fmt);
	ddekit_vprintf(fmt, va);
	va_end(va);
	ddekit_printf("\n");

	error (1, 0, "ddekit_panic()");
}

void ddekit_debug(char *fmt, ...) {
	va_list va;

	va_start(va, fmt);
	ddekit_vprintf(fmt, va);
	va_end(va);
	ddekit_printf("\n");

	error (0, 0, "ddekit_debug()");
}

