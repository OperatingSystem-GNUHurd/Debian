#include <stdio.h>
#include <fcntl.h>
#include <pciaccess.h>
#include <error.h>
#include <hurd/ports.h>
#include <hurd/trivfs.h>
#include <hurd.h>

#include "device_S.h"
#include "notify_S.h"

static struct port_bucket *port_bucket;

/* Trivfs hooks.  */
int trivfs_fstype = FSTYPE_MISC;
int trivfs_fsid = 0;
int trivfs_support_read = 0;
int trivfs_support_write = 0;
int trivfs_support_exec = 0;
int trivfs_allow_open = O_READ | O_WRITE;

/* Our port classes.  */
struct port_class *trivfs_protid_class;
struct port_class *trivfs_cntl_class;

/* Implementation of notify interface */
kern_return_t
do_mach_notify_port_deleted (struct port_info *pi,
			     mach_port_t name)
{
  return EOPNOTSUPP;
}

kern_return_t
do_mach_notify_msg_accepted (struct port_info *pi,
			     mach_port_t name)
{
  return EOPNOTSUPP;
}

kern_return_t
do_mach_notify_port_destroyed (struct port_info *pi,
			       mach_port_t port)
{
  return EOPNOTSUPP;
}

kern_return_t
do_mach_notify_no_senders (struct port_info *pi,
			   mach_port_mscount_t mscount)
{
  return ports_do_mach_notify_no_senders (pi, mscount);
}

kern_return_t
do_mach_notify_send_once (struct port_info *pi)
{
  return EOPNOTSUPP;
}

kern_return_t
do_mach_notify_dead_name (struct port_info *pi,
			  mach_port_t name)
{
  return EOPNOTSUPP;
}

boolean_t
is_master_device (mach_port_t port)
{
  struct port_info *pi = ports_lookup_port (port_bucket, port,
					    trivfs_protid_class);
  if (pi == NULL)
    return FALSE;
  
  ports_port_deref (pi);
  return TRUE;
}

error_t
trivfs_append_args (struct trivfs_control *fsys, char **argz, size_t *argz_len)
{
  error_t err = 0;

#define ADD_OPT(fmt, args...)						\
  do { char buf[100];							\
       if (! err) {							\
         snprintf (buf, sizeof buf, fmt , ##args);			\
         err = argz_add (argz, argz_len, buf); } } while (0)

#undef ADD_OPT
  return err;
}

int trivfs_init()
{
  port_bucket = ports_create_bucket ();
  trivfs_cntl_class = ports_create_class (trivfs_clean_cntl, 0);
  trivfs_protid_class = ports_create_class (trivfs_clean_protid, 0);
  return 0;
}

error_t
trivfs_goaway (struct trivfs_control *fsys, int flags)
{
  int count;

  /* Stop new requests.  */
  ports_inhibit_class_rpcs (trivfs_cntl_class);
  ports_inhibit_class_rpcs (trivfs_protid_class);

  count = ports_count_class (trivfs_protid_class);

  if (count && !(flags & FSYS_GOAWAY_FORCE))
    {
      /* We won't go away, so start things going again...  */
      ports_enable_class (trivfs_protid_class);
      ports_resume_class_rpcs (trivfs_cntl_class);
      ports_resume_class_rpcs (trivfs_protid_class);
      return EBUSY;
    }

  pci_system_cleanup ();
  exit (0);
}

static int
demuxer (mach_msg_header_t *inp, mach_msg_header_t *outp)
{
  mig_routine_t routine;
  if ((routine = device_server_routine (inp)) ||
      (routine = notify_server_routine (inp)) ||
      (routine = NULL, trivfs_demuxer (inp, outp)))
    {
      if (routine)
        (*routine) (inp, outp);
      return TRUE;
    }
  else
    return FALSE;
}

void
trivfs_modify_stat (struct trivfs_protid *cred, io_statbuf_t *stat)
{
}

void trivfs_server()
{
  mach_port_t bootstrap;
  struct trivfs_control *fsys;
  int err;

  task_get_bootstrap_port (mach_task_self (), &bootstrap);
  if (bootstrap == MACH_PORT_NULL)
    error (1, 0, "must be started as a translator");

  /* Reply to our parent.  */
  err = trivfs_startup (bootstrap, 0,
			trivfs_cntl_class, port_bucket,
			trivfs_protid_class, port_bucket, &fsys);
  mach_port_deallocate (mach_task_self (), bootstrap);
  if (err)
    error (1, err, "Contacting parent");

  /* Launch.  */
  do
    {
      ports_manage_port_operations_one_thread (port_bucket, demuxer, 0);
    } while (trivfs_goaway (fsys, 0));
}
