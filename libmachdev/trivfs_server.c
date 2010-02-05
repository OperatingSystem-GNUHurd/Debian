#include <stdio.h>
#include <fcntl.h>
#include <pciaccess.h>
#include <error.h>
#include <hurd/ports.h>
#include <hurd/trivfs.h>
#include <hurd.h>

static struct port_bucket *port_bucket;

/* Trivfs hooks.  */
int trivfs_fstype = FSTYPE_MISC;
int trivfs_fsid = 0;
int trivfs_support_read = 0;
int trivfs_support_write = 0;
int trivfs_support_exec = 0;
int trivfs_allow_open = O_READ | O_WRITE;

struct port_class *trivfs_protid_portclasses[1];
struct port_class *trivfs_cntl_portclasses[1];
int trivfs_protid_nportclasses = 1;
int trivfs_cntl_nportclasses = 1;

/* Implementation of notify interface */
kern_return_t
do_mach_notify_port_deleted (mach_port_t notify,
			     mach_port_t name)
{
  return EOPNOTSUPP;
}

kern_return_t
do_mach_notify_msg_accepted (mach_port_t notify,
			     mach_port_t name)
{
  return EOPNOTSUPP;
}

kern_return_t
do_mach_notify_port_destroyed (mach_port_t notify,
			       mach_port_t port)
{
  return EOPNOTSUPP;
}

kern_return_t
do_mach_notify_no_senders (mach_port_t notify,
			   mach_port_mscount_t mscount)
{
  return ports_do_mach_notify_no_senders (notify, mscount);
}

kern_return_t
do_mach_notify_send_once (mach_port_t notify)
{
  return EOPNOTSUPP;
}

kern_return_t
do_mach_notify_dead_name (mach_port_t notify,
			  mach_port_t name)
{
  return EOPNOTSUPP;
}

boolean_t
is_master_device (mach_port_t port)
{
  struct port_info *pi = ports_lookup_port (port_bucket, port,
					    trivfs_protid_portclasses[0]);
  if (pi == NULL)
    return FALSE;
  
  ports_port_deref (pi);
  return TRUE;
}

int trivfs_init()
{
  port_bucket = ports_create_bucket ();
  trivfs_cntl_portclasses[0] = ports_create_class (trivfs_clean_cntl, 0);
  trivfs_protid_portclasses[0] = ports_create_class (trivfs_clean_protid, 0);
  return 0;
}

error_t
trivfs_goaway (struct trivfs_control *fsys, int flags)
{
  int count;

  fprintf (stderr, "check point 1\n");
  /* Stop new requests.  */
  ports_inhibit_class_rpcs (trivfs_cntl_portclasses[0]);
  ports_inhibit_class_rpcs (trivfs_protid_portclasses[0]);

  count = ports_count_class (trivfs_protid_portclasses[0]);

  fprintf (stderr, "check point 2\n");
  if (count && !(flags & FSYS_GOAWAY_FORCE))
    {
      fprintf (stderr, "check point 4\n");
      /* We won't go away, so start things going again...  */
      ports_enable_class (trivfs_protid_portclasses[0]);
      ports_resume_class_rpcs (trivfs_cntl_portclasses[0]);
      ports_resume_class_rpcs (trivfs_protid_portclasses[0]);
      fprintf (stderr, "check point 5\n");
      return EBUSY;
    }

  fprintf (stderr, "check point 3\n");
  pci_system_cleanup ();
  fprintf (stderr, "trivfs goes away\n");
  exit (0);
}

static int
demuxer (mach_msg_header_t *inp, mach_msg_header_t *outp)
{
  int ret;
  extern int device_server (mach_msg_header_t *, mach_msg_header_t *);
  extern int notify_server (mach_msg_header_t *, mach_msg_header_t *);
  fprintf (stderr, "before trivfs demuxer\n");
  ret = device_server (inp, outp) || notify_server (inp, outp)
    || trivfs_demuxer (inp, outp);
  fprintf (stderr, "after trivfs demuxer\n");
  return ret;
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

  fprintf (stderr, "after get bootstrap port\n");
  /* Reply to our parent.  */
  err = trivfs_startup (bootstrap, 0,
			trivfs_cntl_portclasses[0], port_bucket,
			trivfs_protid_portclasses[0], port_bucket, &fsys);
  mach_port_deallocate (mach_task_self (), bootstrap);
  if (err)
    error (1, err, "Contacting parent");
  fprintf (stderr, "start trivfs\n");

  /* Launch.  */
  do
    {
      ports_manage_port_operations_one_thread (port_bucket, demuxer, 0);
    } while (trivfs_goaway (fsys, 0));
}
