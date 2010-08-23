#include <mach/vm_param.h>
#include <mach/vm_statistics.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <ps.h>
#include "procfs.h"
#include "procfs_dir.h"
#include "main.h"

/* This implements a directory node with the static files in /proc.
   NB: the libps functions for host information return static storage;
   using them would require locking and as a consequence it would be
   more complicated, not simpler.  */

#define INIT_PID 1

static error_t
get_boottime (struct ps_context *pc, struct timeval *tv)
{
  struct proc_stat *ps;
  error_t err;

  err = _proc_stat_create (INIT_PID, pc, &ps);
  if (err)
    return err;

  err = proc_stat_set_flags (ps, PSTAT_TASK_BASIC);
  if (err || !(proc_stat_flags (ps) & PSTAT_TASK_BASIC))
    err = EIO;

  if (! err)
    {
      task_basic_info_t tbi = proc_stat_task_basic_info (ps);
      tv->tv_sec = tbi->creation_time.seconds;
      tv->tv_usec = tbi->creation_time.microseconds;
    }

  _proc_stat_free (ps);
  return err;
}

static error_t
rootdir_gc_version (void *hook, void **contents, size_t *contents_len)
{
  struct utsname uts;
  int r;

  r = uname (&uts);
  if (r < 0)
    return errno;

  *contents_len = asprintf ((char **) contents,
      "Linux version 2.6.1 (%s %s %s %s)\n",
      uts.sysname, uts.release, uts.version, uts.machine);

  return *contents_len >= 0 ? 0 : ENOMEM;
}

/* Uptime -- we use the start time of init to deduce it. This is probably a bit
   fragile, as any clock update will make the result inaccurate. */
static error_t
rootdir_gc_uptime (void *hook, void **contents, size_t *contents_len)
{
  struct timeval time, boottime;
  double up_secs;
  error_t err;

  err = gettimeofday (&time, NULL);
  if (err < 0)
    return errno;

  err = get_boottime (hook, &boottime);
  if (err)
    return err;

  timersub (&time, &boottime, &time);
  up_secs = time.tv_sec + time.tv_usec / 1000000.;

  /* The second field is the total idle time. As far as I know we don't
     keep track of it.  However, procps uses it to compute "USER_HZ", and
     proc(5) specifies that it should be equal to USER_HZ times the idle value
     in ticks from /proc/stat.  So we assume a completely idle system both here
     and there to make that work.  */
  *contents_len = asprintf ((char **) contents, "%.2lf %.2lf\n", up_secs, up_secs);

  return *contents_len >= 0 ? 0 : ENOMEM;
}

static error_t
rootdir_gc_stat (void *hook, void **contents, size_t *contents_len)
{
  struct timeval boottime, time;
  struct vm_statistics vmstats;
  unsigned long up_ticks;
  error_t err;

  err = gettimeofday (&time, NULL);
  if (err < 0)
    return errno;

  err = get_boottime (hook, &boottime);
  if (err)
    return err;

  err = vm_statistics (mach_task_self (), &vmstats);
  if (err)
    return EIO;

  timersub (&time, &boottime, &time);
  up_ticks = opt_clk_tck * (time.tv_sec + time.tv_usec / 1000000.);

  *contents_len = asprintf ((char **) contents,
      /* Does Mach keeps track of any of this? */
      "cpu  0 0 0 %lu 0 0 0 0 0\n"
      "cpu0 0 0 0 %lu 0 0 0 0 0\n"
      "intr 0\n"
      /* This we know. */
      "page %d %d\n"
      "btime %lu\n",
      up_ticks,
      up_ticks,
      vmstats.pageins, vmstats.pageouts,
      boottime.tv_sec);

  return *contents_len >= 0 ? 0 : ENOMEM;
}

static error_t
rootdir_gc_loadavg (void *hook, void **contents, size_t *contents_len)
{
  host_load_info_data_t hli;
  mach_msg_type_number_t cnt;
  error_t err;

  cnt = HOST_LOAD_INFO_COUNT;
  err = host_info (mach_host_self (), HOST_LOAD_INFO, (host_info_t) &hli, &cnt);
  if (err)
    return err;

  assert (cnt == HOST_LOAD_INFO_COUNT);
  *contents_len = asprintf ((char **) contents,
      "%.2f %.2f %.2f 1/0 0\n",
      hli.avenrun[0] / (double) LOAD_SCALE,
      hli.avenrun[1] / (double) LOAD_SCALE,
      hli.avenrun[2] / (double) LOAD_SCALE);

  return *contents_len >= 0 ? 0 : ENOMEM;
}

static error_t
rootdir_gc_meminfo (void *hook, void **contents, size_t *contents_len)
{
  host_basic_info_data_t hbi;
  mach_msg_type_number_t cnt;
  struct vm_statistics vmstats;
  error_t err;

  err = vm_statistics (mach_task_self (), &vmstats);
  if (err)
    return EIO;

  cnt = HOST_BASIC_INFO_COUNT;
  err = host_info (mach_host_self (), HOST_BASIC_INFO, (host_info_t) &hbi, &cnt);
  if (err)
    return err;

  assert (cnt == HOST_BASIC_INFO_COUNT);
  *contents_len = asprintf ((char **) contents,
      "MemTotal: %14lu kB\n"
      "MemFree:  %14lu kB\n"
      "Active:   %14lu kB\n"
      "Inactive: %14lu kB\n"
      "Mlocked:  %14lu kB\n"
      ,
      /* TODO: check that these are really 1024-bytes kBs. */
      (long unsigned) hbi.memory_size / 1024,
      (long unsigned) vmstats.free_count * PAGE_SIZE / 1024,
      (long unsigned) vmstats.active_count * PAGE_SIZE / 1024,
      (long unsigned) vmstats.inactive_count * PAGE_SIZE / 1024,
      (long unsigned) vmstats.wire_count * PAGE_SIZE / 1024);

  return *contents_len >= 0 ? 0 : ENOMEM;
}

static error_t
rootdir_gc_vmstat (void *hook, void **contents, size_t *contents_len)
{
  host_basic_info_data_t hbi;
  mach_msg_type_number_t cnt;
  struct vm_statistics vmstats;
  error_t err;

  err = vm_statistics (mach_task_self (), &vmstats);
  if (err)
    return EIO;

  cnt = HOST_BASIC_INFO_COUNT;
  err = host_info (mach_host_self (), HOST_BASIC_INFO, (host_info_t) &hbi, &cnt);
  if (err)
    return err;

  assert (cnt == HOST_BASIC_INFO_COUNT);
  *contents_len = asprintf ((char **) contents,
      "nr_free_pages %lu\n"
      "nr_inactive_anon %lu\n"
      "nr_active_anon %lu\n"
      "nr_inactive_file %lu\n"
      "nr_active_file %lu\n"
      "nr_unevictable %lu\n"
      "nr_mlock %lu\n"
      "pgpgin %lu\n"
      "pgpgout %lu\n"
      "pgfault %lu\n",
      (long unsigned) vmstats.free_count,
      /* FIXME: how can we distinguish the anon/file pages? Maybe we can
         ask the default pager how many it manages? */
      (long unsigned) vmstats.inactive_count,
      (long unsigned) vmstats.active_count,
      (long unsigned) 0,
      (long unsigned) 0,
      (long unsigned) vmstats.wire_count,
      (long unsigned) vmstats.wire_count,
      (long unsigned) vmstats.pageins,
      (long unsigned) vmstats.pageouts,
      (long unsigned) vmstats.faults);

  return *contents_len >= 0 ? 0 : ENOMEM;
}

static error_t
rootdir_gc_fakeself (void *hook, void **contents, size_t *contents_len)
{
  *contents_len = asprintf ((char **) contents, "%d", opt_fake_self);
  return *contents_len >= 0 ? 0 : ENOMEM;
}


static struct node *
rootdir_file_make_node (void *dir_hook, void *entry_hook)
{
  return procfs_make_node (entry_hook, dir_hook);
}

static struct node *
rootdir_symlink_make_node (void *dir_hook, void *entry_hook)
{
  struct node *np = procfs_make_node (entry_hook, dir_hook);
  if (np)
    procfs_node_chtype (np, S_IFLNK);
  return np;
}

static const struct procfs_dir_entry rootdir_entries[] = {
  {
    .name = "self",
    .make_node = rootdir_symlink_make_node,
    .hook = & (struct procfs_node_ops) {
      .get_contents = rootdir_gc_fakeself,
      .cleanup_contents = procfs_cleanup_contents_with_free,
    },
  },
  {
    .name = "version",
    .make_node = rootdir_file_make_node,
    .hook = & (struct procfs_node_ops) {
      .get_contents = rootdir_gc_version,
      .cleanup_contents = procfs_cleanup_contents_with_free,
    },
  },
  {
    .name = "uptime",
    .make_node = rootdir_file_make_node,
    .hook = & (struct procfs_node_ops) {
      .get_contents = rootdir_gc_uptime,
      .cleanup_contents = procfs_cleanup_contents_with_free,
    },
  },
  {
    .name = "stat",
    .make_node = rootdir_file_make_node,
    .hook = & (struct procfs_node_ops) {
      .get_contents = rootdir_gc_stat,
      .cleanup_contents = procfs_cleanup_contents_with_free,
    },
  },
  {
    .name = "loadavg",
    .make_node = rootdir_file_make_node,
    .hook = & (struct procfs_node_ops) {
      .get_contents = rootdir_gc_loadavg,
      .cleanup_contents = procfs_cleanup_contents_with_free,
    },
  },
  {
    .name = "meminfo",
    .make_node = rootdir_file_make_node,
    .hook = & (struct procfs_node_ops) {
      .get_contents = rootdir_gc_meminfo,
      .cleanup_contents = procfs_cleanup_contents_with_free,
    },
  },
  {
    .name = "vmstat",
    .make_node = rootdir_file_make_node,
    .hook = & (struct procfs_node_ops) {
      .get_contents = rootdir_gc_vmstat,
      .cleanup_contents = procfs_cleanup_contents_with_free,
    },
  },
  {}
};

error_t
rootdir_create_node (struct node **np)
{
  struct ps_context *pc;
  const struct procfs_dir_entry *entries;
  error_t err;

  err = ps_context_create (getproc (), &pc);
  if (err)
    return err;

  entries = rootdir_entries;
  if (opt_fake_self < 0)
    entries++;

  *np = procfs_dir_make_node (entries, pc, (void (*)(void *)) ps_context_free);
  return 0;
}
