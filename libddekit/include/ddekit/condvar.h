#ifndef _ddekit_condvar_h
#define _ddekit_condvar_h

/** \file ddekit/condvar.h */
#include "ddekit/lock.h"

struct ddekit_condvar;
typedef struct ddekit_condvar ddekit_condvar_t;

/** Initialize conditional variable.
 *
 * \ingroup DDEKit_synchronization
 */
ddekit_condvar_t * ddekit_condvar_init(void);

/** Uninitialize conditional variable. 
 *
 * \ingroup DDEKit_synchronization
 */
void ddekit_condvar_deinit(ddekit_condvar_t *cvp);

/** Wait on a conditional variable.
 *
 * \ingroup DDEKit_synchronization
 */
void ddekit_condvar_wait(ddekit_condvar_t *cvp, ddekit_lock_t *mp);

/** Wait on a conditional variable at most until a timeout expires.
 *
 * \ingroup DDEKit_synchronization
 *
 * \param cvp    pointer to condvar
 * \param mp     lock
 * \param timo   timeout in ms
 *
 * \return 0     success
 * \return !=0   timeout
 */
int  ddekit_condvar_wait_timed(ddekit_condvar_t *cvp, ddekit_lock_t *mp, int timo);

/** Send signal to the next one waiting for condvar.
 *
 * \ingroup DDEKit_synchronization
 */
void ddekit_condvar_signal(ddekit_condvar_t *cvp);

/** Send signal to all threads waiting for condvar.
 *
 * \ingroup DDEKit_synchronization
 */
void ddekit_condvar_broadcast(ddekit_condvar_t *cvp);

#endif
