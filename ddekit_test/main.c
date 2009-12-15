#include <mach.h>

#include "dde.h"
#include "ddekit/condvar.h"
#include "ddekit/semaphore.h"
#include "ddekit/thread.h"
#include "ddekit/printf.h"
#include "ddekit/assert.h"

#define VERBOSE 1

boolean_t using_std = 1;

/****************
 ** Timer test **
 ****************/

#include "ddekit/timer.h"

static int twenty_timer = -1;

static void __attribute__((unused)) dummy(void *arg) { }

static void timer_func(void *arg)
{
	ddekit_printf("Timed func called with arg %d\n", (int)arg);

	if ((int)arg == 15) {
		int del = ddekit_del_timer(twenty_timer);
		ddekit_printf("\tdropped 20s timer: %d\n", del);
	}

	if ((int)arg == 30)
		ddekit_printf("\033[32;1mEND TIMER TEST\033[0m\n");
}


/* One tick each second. */
static void tick(void *arg)
{
	static int cnt=0;
	int r=0;

	ddekit_printf("tick %d\n", cnt++);
	
	r = ddekit_add_timer(tick, 0, jiffies + HZ);
}


static void timer_test(void)
{
	int i;

	ddekit_printf("\033[32;1mBEGIN TIMER TEST\033[0m\n");

	ddekit_printf("HZ: %d\n", HZ);
	ddekit_printf("JIFFIES: %lld\n", jiffies);
	
	ddekit_init_timers();
	ddekit_thread_msleep(1000);

	i = ddekit_add_timer(tick, 0, jiffies+HZ);
	ddekit_printf("added timer: %d\n", i);

	i = ddekit_add_timer(timer_func, (void *)5, jiffies + 5*HZ);
	ddekit_printf("added timer: %d\n", i);
	i = ddekit_add_timer(timer_func, (void *)10, jiffies + 10*HZ);
	ddekit_printf("added timer: %d\n", i);
	i = ddekit_add_timer(timer_func, (void *)15, jiffies + 15*HZ);
	ddekit_printf("added timer: %d\n", i);
	twenty_timer = ddekit_add_timer(timer_func, (void *)20, jiffies + 20*HZ);
	ddekit_printf("added timer: %d\n", twenty_timer);
	i = ddekit_add_timer(timer_func, (void *)30, jiffies + 30*HZ);
	ddekit_printf("added timer: %d\n", i);

	ddekit_thread_msleep (10 * 1000);
}

/********************************
 ** Memory and page-table test **
 ********************************/

#include "ddekit/memory.h"
#include "ddekit/pgtab.h"

static void memory_test(void)
{
	{
		/* simple malloc */
		ddekit_printf("\033[32;1mBEGIN SIMPLE MEMORY TEST\033[0m\n");

		int i, j;
		unsigned size = 508;
		unsigned *p[10];

		for (i = 0; i < 10; ++i) {
			p[i] = ddekit_simple_malloc(size);
			for (j = 0; j < size/sizeof(unsigned); ++j)
				(p[i])[j] = i;
			ddekit_printf("malloc(%d) => %p\n", size, p[i]);
		}

		i = 4;
		ddekit_simple_free(p[i]);
		ddekit_printf("free(%p)\n", p[i]);

		p[i] = ddekit_simple_malloc(size);
		for (j = 0; j < size/sizeof(unsigned); ++j)
			(p[i])[j] = 0xeeeeeeee;
		ddekit_printf("malloc(%d) => %p\n", size, p[i]);

		for (i = 9; i >= 0; --i) {
			ddekit_simple_free(p[i]);
			ddekit_printf("free(%p)\n", p[i]);
		}

		i = 0;
		p[i] = ddekit_simple_malloc(size);
		for (j = 0; j < size/sizeof(unsigned); ++j)
			(p[i])[j] = 0xaaaaaaaa;
		ddekit_printf("malloc(%d) => %p\n", size, p[i]);

		ddekit_printf("\033[32;1mEND SIMPLE MEMORY TEST\033[0m\n");
	}
	{
		/* large malloc */
		ddekit_printf("\033[32;1mBEGIN LARGE MEMORY TEST\033[0m\n");

		const size_t malloc_size = 1024 * 4096;

		char *p = ddekit_large_malloc(malloc_size);

		ddekit_printf("virt->phys mappings:\n");
		char *tmp = p;
		ddekit_printf("  %p -> %p\n", tmp, ddekit_pgtab_get_physaddr(tmp));
		tmp = p + malloc_size / 2;
		ddekit_printf("  %p -> %p\n", tmp, ddekit_pgtab_get_physaddr(tmp));
		tmp = p + malloc_size;
		ddekit_printf("Expected error follows...\n");
		ddekit_printf("  %p -> %p\n", tmp, ddekit_pgtab_get_physaddr(tmp));

		ddekit_large_free(p);
		tmp = p + malloc_size;
		ddekit_printf("Expected error follows...\n");
		ddekit_printf("  %p -> %p\n", tmp, ddekit_pgtab_get_physaddr(tmp));

		p = ddekit_large_malloc(malloc_size);

		ddekit_printf("virt->phys mappings:\n");
		tmp = p;
		ddekit_printf("  %p -> %p\n", tmp, ddekit_pgtab_get_physaddr(tmp));
		tmp = p + malloc_size / 2;
		ddekit_printf("  %p -> %p\n", tmp, ddekit_pgtab_get_physaddr(tmp));
		tmp = p + malloc_size;
		ddekit_printf("Expected error follows...\n");
		ddekit_printf("  %p -> %p\n", tmp, ddekit_pgtab_get_physaddr(tmp));

		ddekit_large_free(p);

		ddekit_printf("\033[32;1mEND LARGE MEMORY TEST\033[0m\n");
	}
	{
		int x = 0;
		/* slabs */
		ddekit_printf("\033[32;1mBEGIN SLAB MEMORY TEST\033[0m\n");

		/* Test slabs for contiguous and non-contiguous case. */
		for ( ; x < 2; ++x) {
			ddekit_printf("Contiguous mode set to %d\n", x);

			struct ddekit_slab *slab = ddekit_slab_init(256, x);
			if (!slab) ddekit_debug("slab cache init failed");

			unsigned const magic = 0xdeadbeef;
			ddekit_slab_set_data(slab, (void *)magic);
			if (ddekit_slab_get_data(slab) != (void *)magic)
				ddekit_debug("user pointer differs");

			unsigned i;
			unsigned *p[64];

			for (i = 0; i < sizeof(p)/sizeof(*p); ++i) {
				p[i]  = ddekit_slab_alloc(slab);
				*p[i] = i | 0xaffe0000;
				if (i % 8 == 0) {
					if (x)
						ddekit_printf("slab_alloc[%d] => %p (%p phys)\n",
						              i, p[i], ddekit_pgtab_get_physaddr(p[i]));
					else
						ddekit_printf("slab_alloc[%d] => %p (no phys)\n", i, p[i]);
				}
			}

			for (i = 0; i < sizeof(p)/sizeof(*p); ++i) {
				ddekit_slab_free(slab, p[i]);
			}
		}

		ddekit_printf("\033[32;1mEND SLAB MEMORY TEST\033[0m\n");
	}

	{
		ddekit_printf("\033[32;1mBEGIN PGTAB TEST\033[0m\n");
		ddekit_addr_t phys = 0x12345000;
		void *virt = (void *)0xABC00000;
		void *virt2 = (void *)0;
		void *virt3 = (void *)0;

		ddekit_printf("4 invalid resolutions...\n");
		ddekit_printf("virt_to_phys(0) = %p\n", ddekit_pgtab_get_physaddr(0));
		ddekit_printf("phys_to_virt(0) = %p\n", ddekit_pgtab_get_virtaddr(0));
		ddekit_printf("virt_to_phys(%p) = %p\n", virt, ddekit_pgtab_get_physaddr(virt));
		ddekit_printf("phys_to_virt(%p) = %p\n", phys, ddekit_pgtab_get_virtaddr(phys));

		virt = ddekit_large_malloc(16387);
		virt2 = ddekit_large_malloc(32769);
		virt3 = ddekit_large_malloc(8193);

		ddekit_printf("6 correct lookups.\n");

		phys = ddekit_pgtab_get_physaddr(virt);
		ddekit_printf("virt_to_phys(%p) = %p\n", virt, phys);
		ddekit_printf("phys_to_virt(%p) = %p\n", phys, ddekit_pgtab_get_virtaddr(phys));

		phys = ddekit_pgtab_get_physaddr(virt2);
		ddekit_printf("virt_to_phys(%p) = %p\n", virt2, phys);
		ddekit_printf("phys_to_virt(%p) = %p\n", phys, ddekit_pgtab_get_virtaddr(phys));

		phys = ddekit_pgtab_get_physaddr(virt3);
		ddekit_printf("virt_to_phys(%p) = %p\n", virt3, phys);
		ddekit_printf("phys_to_virt(%p) = %p\n", phys, ddekit_pgtab_get_virtaddr(phys));

		ddekit_printf("freeing 2nd area.\n");
		ddekit_large_free(virt2);


		ddekit_printf("2 errors.\n");

		phys = ddekit_pgtab_get_physaddr(virt2);
		ddekit_printf("virt_to_phys(%p) = %p\n", virt2, phys);
		ddekit_printf("phys_to_virt(%p) = %p\n", phys, ddekit_pgtab_get_virtaddr(phys));

		ddekit_printf("4 correct lookups.\n");

		phys = ddekit_pgtab_get_physaddr(virt);
		ddekit_printf("virt_to_phys(%p) = %p\n", virt, phys);
		ddekit_printf("phys_to_virt(%p) = %p\n", phys, ddekit_pgtab_get_virtaddr(phys));

		phys = ddekit_pgtab_get_physaddr(virt3);
		ddekit_printf("virt_to_phys(%p) = %p\n", virt3, phys);
		ddekit_printf("phys_to_virt(%p) = %p\n", phys, ddekit_pgtab_get_virtaddr(phys));


		ddekit_printf("\033[32;1mEND PGTAB TEST\033[0m\n");
	}
}

ddekit_sem_t *sem;
ddekit_condvar_t *cond;
ddekit_lock_t cond_lock;
int status;

static void thread_func (void *arg)
{
  ddekit_lock_t lock = arg;
  int ret;

  ddekit_printf ("%s thread starts at %d\n",
		 ddekit_thread_get_name (ddekit_thread_myself ()), time (NULL));
  ddekit_lock_lock (&lock);
  ddekit_thread_sleep (&lock);
  ddekit_lock_unlock (&lock);
  ddekit_printf ("%s thread wakes up at %d\n",
		 ddekit_thread_get_name (ddekit_thread_myself ()), time (NULL));

  ret = ddekit_sem_down_timed (sem, 1000);
  ddekit_printf ("%s thread enter a semaphore at %d, timeout: %d\n",
		 ddekit_thread_get_name (ddekit_thread_myself ()),
		 time (NULL), ret != 0);

  ddekit_printf ("%s thread waits for signal at %d\n",
		 ddekit_thread_get_name (ddekit_thread_myself ()), time (NULL));
  ddekit_lock_lock (&cond_lock);
  while (!status)
    ddekit_condvar_wait (cond, &cond_lock);
  ddekit_lock_unlock (&cond_lock);
  ddekit_printf ("%s thread wakes up at %d\n",
		 ddekit_thread_get_name (ddekit_thread_myself ()), time (NULL));
}

static void thread_test ()
{
  ddekit_thread_t *thread1 = NULL;
  ddekit_thread_t *thread2 = NULL;
  ddekit_lock_t lock1;
  ddekit_lock_t lock2;
  
  sem = ddekit_sem_init (0);
  cond = ddekit_condvar_init ();
  status = 0;
  ddekit_lock_init (&cond_lock);
  ddekit_lock_init (&lock1);
  ddekit_lock_init (&lock2);

  thread1 = ddekit_thread_create (thread_func, lock1, "test1");
  thread2 = ddekit_thread_create (thread_func, lock2, "test2");

  sleep (3);
  ddekit_lock_lock (&lock1);
  ddekit_thread_wakeup (thread1);
  ddekit_lock_unlock (&lock1);

  ddekit_lock_lock (&lock2);
  ddekit_thread_wakeup (thread2);
  ddekit_lock_unlock (&lock2);
  sleep (2);

  ddekit_sem_up (sem);
  ddekit_sem_up (sem);
  sleep (1);

  ddekit_printf ("main thread wakes up the other two at %d\n", time (NULL));
  ddekit_lock_lock (&cond_lock);
  status = 1;
  ddekit_condvar_broadcast (cond);
  ddekit_lock_unlock (&cond_lock);
  sleep (1);
}


int main(int argc, char **argv)
{
	ddekit_log(1, "Hey there, %s running...", argv[0]);

	ddekit_init();

	if (0) memory_test();
	if (0) timer_test();
	if (1) thread_test();

	return 0;
}
