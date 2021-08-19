#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/sysinfo.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

typedef double (*function_t) (double);

double *
pmap_process (function_t func, const double *in, size_t count)
{
  size_t proc_cnt = get_nprocs ();
  size_t work = count / proc_cnt;
  size_t remainder = count % proc_cnt;
  char *raw_mem
      = mmap (NULL,
              (proc_cnt + 1) * sizeof (int) + (count + 1) * sizeof (double)
                  + sizeof (sem_t),
              PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  double *buffer = (double *)raw_mem;
  int *pids = (int *)(raw_mem + (count + 1) * sizeof (double));
  sem_t *semaphore = (sem_t *)(buffer + (count + 1) * sizeof (double));
  sem_init (semaphore, 1, 0);
  for (size_t i = 0; i < proc_cnt; ++i)
    {
      pids[i] = fork ();
      if (pids[i] == 0)
        {
          for (size_t j = 0; j < work; ++j)
            {
              buffer[i * work + j] = func (in[i * work + j]);
            }
          if (i < remainder)
            {
              buffer[work * proc_cnt + i] = func (in[work * proc_cnt + i]);
            }
          sem_post (semaphore);
          int sem_val;
          sem_getvalue (semaphore, &sem_val);
          exit (0);
        }
    }
  for (size_t i = 0; i < proc_cnt; ++i)
    {
      sem_wait (semaphore);
    }
  for (size_t i = 0; i < proc_cnt; ++i)
    {
      int state = 0;
      waitpid (pids[i], &state, 0);
    }
  sem_close (semaphore);
  return buffer;
}

void
pmap_free (double *ptr, size_t count)
{
  size_t proc_cnt = get_nprocs ();
  munmap ((char *)ptr, (count + 1) * sizeof (double) + sizeof (sem_t)
                           + sizeof (int) * proc_cnt);
}