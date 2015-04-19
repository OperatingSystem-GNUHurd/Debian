/* FS helper library definitions
   Copyright (C) 1994,95,96,97,98,99,2000,01,02,13,14
     Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2, or (at
   your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. */

#ifndef _HURD_FSHELP_
#define _HURD_FSHELP_

/* This library implements various things that are generic to
   all or most implementors of the filesystem protocol.  It
   presumes that you are using the iohelp library as well.  It
   is divided into separate facilities which may be used independently.  */

#include <errno.h>
#include <mach.h>
#include <hurd/hurd_types.h>
#include <pthread.h>
#include <hurd/iohelp.h>
#include <sys/stat.h>
#include <maptime.h>


/* Keeping track of active translators */
/* These routines keep a list of active translators.  They do not
   require multi threading but depend on the ports library.  */

struct port_info;

/* Record an active translator being bound to the given file name
   NAME.  ACTIVE is the control port of the translator.  PI references
   a receive port that is used to request dead name notifications,
   typically the port for the underlying node passed to the
   translator.  */
error_t
fshelp_set_active_translator (struct port_info *pi,
			      const char *name,
			      mach_port_t active);

/* Remove the active translator specified by its control port ACTIVE.
   If there is no active translator with the given control port, this
   does nothing.  */
error_t
fshelp_remove_active_translator (mach_port_t active);

/* This kind of function is used by fshelp_get_active_translators to
   filter the list of translators to return.  If a filter returns an
   error for a given PATH, the translator bound to the PATH is not
   included in the list.  */
typedef error_t (*fshelp_filter) (const char *path);

/* Records the list of active translators below PREFIX into the argz
   vector specified by TRANSLATORS filtered by FILTER.  If PREFIX is
   NULL, entries with any prefix are considered.  If FILTER is NULL,
   no filter is applied.  */
error_t
fshelp_get_active_translators (char **translators,
			       size_t *translators_len,
			       fshelp_filter filter,
			       const char *prefix);


/* Passive translator linkage */
/* These routines are self-contained and start passive translators,
   returning the control port.  They do not require multi threading
   or the ports library. */

/* A callback used by the translator starting functions, which should be a
   function that given some open flags, opens the appropriate file, and
   returns the node port.  */
typedef error_t (*fshelp_open_fn_t) (int flags,
				     file_t *node,
				     mach_msg_type_name_t *node_type,
				     task_t, void *cookie);

/* Start a passive translator NAME with arguments ARGZ (length
   ARGZ_LEN).  Initialize the initports to PORTS (length PORTS_LEN),
   the initints to INTS (length INTS_LEN), and the file descriptor
   table to FDS (length FDS_LEN).  Return the control port in
   *CONTROL.  If the translator doesn't respond or die in TIMEOUT
   milliseconds (if TIMEOUT > 0), return an appropriate error.  If the
   translator dies before responding, return EDIED.  Set the new
   task's owner to OWNER_UID (or, if OWNER_UID is -1, then clear the
   new task's owner. */
error_t
fshelp_start_translator_long (fshelp_open_fn_t underlying_open_fn, void *cookie,
			      char *name, char *argz, int argz_len,
			      mach_port_t *fds,
			      mach_msg_type_name_t fds_type, int fds_len,
			      mach_port_t *ports,
			      mach_msg_type_name_t ports_type, int ports_len,
			      int *ints, int ints_len,
			      uid_t owner_uid,
			      int timeout, fsys_t *control);


/* Same as fshelp_start_translator_long, except the initports and ints
   are copied from our own state, fd[2] is copied from our own stderr,
   and the other fds are cleared.  */
error_t
fshelp_start_translator (fshelp_open_fn_t underlying_open_fn, void *cookie,
			 char *name, char *argz, int argz_len,
			 int timeout, fsys_t *control);


/* Active translator linkage */

/* These routines implement the linkage to active translators needed
   by any filesystem which supports them.  They require pthreads and
   use the passive translator routines above, but they don't require
   the ports library at all.  */

struct transbox
{
  fsys_t active;
  pthread_mutex_t *lock;
  int flags;
  pthread_cond_t wakeup;
  void *cookie;
};
#define TRANSBOX_STARTING 1
#define TRANSBOX_WANTED 2

/* This interface is complex, because creating the ports and state
   necessary for start_translator_long is expensive.  The caller to
   fshelp_fetch_root should not need to create them on every call, since
   usually there will be an existing active translator. */

/* This routine is called by fshelp_fetch_root to fetch more information.
   Return the owner and group of the underlying translated file in *UID and
   *GID; point *ARGZ at the entire passive translator spec for the file
   (setting *ARGZ_LEN to the length.)  If there is no passive translator,
   then return ENOENT.  COOKIE1 is the cookie passed in fshelp_transbox_init.
   COOKIE2 is the cookie passed in the call to fshelp_fetch_root.  */
typedef error_t (*fshelp_fetch_root_callback1_t) (void *cookie1, void *cookie2,
						  uid_t *uid, gid_t *gid,
						  char **argz, size_t *argz_len);

/* This routine is called by fshelp_fetch_root to fetch more information.
   Return an unauthenticated node for the file itself in *UNDERLYING and
   *UNDERLYING_TYPE (opened with FLAGS).  COOKIE1 is the cookie passed in
   fshelp_transbox_init.  COOKIE2 is the cookie passed in the call to
   fshelp_fetch_root.  */
typedef error_t (*fshelp_fetch_root_callback2_t) (void *cookie1, void *cookie2,
						  int flags,
						  mach_port_t *underlying,
						  mach_msg_type_name_t
						  *underlying_type);

/* Fetch the root from TRANSBOX.  DOTDOT is an unauthenticated port
   for the directory in which we are looking; USER specifies the ids
   of the user responsible for the call.  FLAGS are as for dir_lookup
   (but O_CREAT and O_EXCL are not meaningful and are ignored).  The
   transbox lock (as set by fshelp_transbox_init) must be held before
   the call, and will be held upon return, but may be released during
   the operation of the call.  */
error_t
fshelp_fetch_root (struct transbox *transbox, void *cookie,
		   file_t dotdot,
		   struct iouser *user,
		   int flags,
		   fshelp_fetch_root_callback1_t callback1,
		   fshelp_fetch_root_callback2_t callback2,
		   retry_type *retry, char *retryname, mach_port_t *root);

void
fshelp_transbox_init (struct transbox *transbox,
		      pthread_mutex_t *lock,
		      void *cookie);

/* Return true iff there is an active translator on this box */
int fshelp_translated (struct transbox *box);

/* Atomically replace the existing active translator port for this box
   with NEWACTIVE.  If EXCL is non-zero then don't frob an existing
   active; return EBUSY instead.  */
error_t fshelp_set_active (struct transbox *box,
			   fsys_t newactive, int excl);

/* Fetch the control port to make a request on it.  It's a bad idea
   to do fsys_getroot with the result; use fetch_root instead. */
error_t fshelp_fetch_control (struct transbox *box,
			      mach_port_t *control);

/* A transbox is being deallocated, clean associated state. */
void fshelp_drop_transbox (struct transbox *box);



/* Flock handling. */
struct lock_box
{
  int type;
  pthread_cond_t wait;
  int waiting;
  int shcount;
};

/* Call when a user makes a request to acquire an lock via file_lock.
   There should be one lock box per object and one int per open; these
   are passed as arguments BOX and USER respectively.  FLAGS are as
   per file_lock.  MUT is a mutex which will be held whenever this
   routine is called, to lock BOX->wait.  */
error_t fshelp_acquire_lock (struct lock_box *box, int *user,
			     pthread_mutex_t *mut, int flags);


/* Initialize lock_box BOX.  (The user int passed to fshelp_acquire_lock
   should be initialized with LOCK_UN.).  */
void fshelp_lock_init (struct lock_box *box);



struct port_bucket;		/* shut up C compiler */
/* Return an identity port in *PT for the node numbered FILENO,
   suitable for returning from io_identity; exactly one send right
   must be created from the returned value.  FILENO should be the same
   value returned as the `fileno' out-parameter in io_identity, and in
   the enclosing directory (except for mount points), and in the
   st_ino stat field.  BUCKET should be a ports port bucket; fshelp
   requires the caller to make sure port operations (for no-senders
   notifications) are used.
   */
error_t fshelp_get_identity (struct port_bucket *bucket,
			     ino64_t fileno, mach_port_t *pt);



/* Try to hand off responsibility from a translator to the server located on
   the node SERVER_NAME.  REQUESTOR is the translator's bootstrap port, and
   ARGV is the command line.  If SERVER_NAME is NULL, then a name is
   concocted by appending ARGV[0] to _SERVERS.  */
error_t fshelp_delegate_translation (const char *server_name,
				     mach_port_t requestor, char **argv);

struct idvec;			/* Include <idvec.h> to get the real thing. */

/* If SUID or SGID is true, adds UID and/or GID respectively to the
   authentication in PORTS[INIT_PORT_AUTH], and replaces it with the result.
   All the other ports in PORTS and FDS are then reauthenticated, using any
   privileges available through AUTH.  If GET_FILE_IDS is non-NULL, and the
   auth port in PORTS[INIT_PORT_AUTH] is bogus, it is called to get a list of
   uids and gids from the file to use as a replacement.  If SECURE is
   non-NULL, whether not the added ids are new is returned in it.  If either
   the uid or gid case fails, then the other may still be applied.  */
error_t
fshelp_exec_reauth (int suid, uid_t uid, int sgid, gid_t gid,
		    auth_t auth,
		    error_t
		      (*get_file_ids)(struct idvec *uids, struct idvec *gids),
		    mach_port_t *ports, mach_msg_type_number_t num_ports,
		    mach_port_t *fds, mach_msg_type_number_t num_fds,
		    int *secure);

struct argp;			/* Include <argp.h> to get the real thing.  */

/* Invoke ARGP with data from DATA & LEN, in the standard way.  */
error_t fshelp_set_options (const struct argp *argp, int flags,
			    const char *argz, size_t argz_len, void *input);


/*  Standardized filesystem permission checking */

/* Check to see whether USER should be considered the owner of the
   file identified by ST.  If so, return zero; otherwise return an
   appropriate error code. */
error_t fshelp_isowner (io_statbuf_t *st, struct iouser *user);

/* Check to see whether USER should be considered a controller of the
   filesystem.  Which is to say, check to see if we should give USER the
   control port.  ST is the stat of the root node.  USER is the user
   asking for a send right to the control port.  */
error_t
fshelp_iscontroller (io_statbuf_t *st, struct iouser *user);

/* Check to see whether the user USER can operate on a file identified
   by ST.  OP is one of S_IREAD, S_IWRITE, and S_IEXEC.  If the access
   is permitted, return zero; otherwise return an appropriate error
   code.  */
error_t fshelp_access (io_statbuf_t *st, int op, struct iouser *user);

/* Check to see whether USER is allowed to modify DIR with respect to
   existing file ST.  (If there is no existing file, pass 0 for ST.)
   If the access is permissible return 0; otherwise return an
   appropriate error code.  */
error_t fshelp_checkdirmod (io_statbuf_t *dir, io_statbuf_t *st,
			    struct iouser *user);


/* Timestamps to change.  */
#define TOUCH_ATIME 0x1
#define TOUCH_MTIME 0x2
#define TOUCH_CTIME 0x4

/* Change the stat times of NODE as indicated by WHAT (from the set TOUCH_*)
   to the current time.  */
void fshelp_touch (io_statbuf_t *st, unsigned what,
		   volatile struct mapped_time_value *maptime);
#endif
