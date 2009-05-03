#include <stdio.h>

#include <hurd.h>
#include <mach.h>

int main()
{
  pid_t *pp;
  size_t npids = 0;
  error_t err;
  int i;

  err = proc_getallpids (getproc (), &pp, &npids);
  if (err)
    error (1, err, "get all pids");

  for (i = 0; i < npids; i++)
    {
      task_t task = pid2task (pp[i]);
      printf ("get task: %d\n", task);
//      err = task_terminate (task);
//      if (err)
//	error (2, err, "terminate task: %d", task);
    }
  return 0;
}
