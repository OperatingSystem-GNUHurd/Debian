#include <ddekit/initcall.h>

#define BEG	{ (crt0_hook) ~1U }
#define END	{ (crt0_hook)   0 }

#if (__GNUC__ == 3 && __GNUC_MINOR__ >= 3) || __GNUC__ >= 4
#define SECTION(x) __attribute__((used, section( x )))
#else
#define SECTION(x) __attribute__((section( x )))
#endif

typedef void (*const crt0_hook)(void);

static crt0_hook __L4DDE_CTOR_BEG__[1] SECTION(".mark_beg_l4dde_ctors")	= BEG;
static crt0_hook __l4DDE_CTOR_END__[1] SECTION(".mark_end_l4dde_ctors")	= END;

void ddekit_do_initcalls() {
    crt0_hook *list = __L4DDE_CTOR_BEG__;

    list++;
    while (*list)
      {
	(**list)();
	list++;
      }
}

