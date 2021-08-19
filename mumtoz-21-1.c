#define _GNU_SOURCE
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct
{
  sem_t request_ready;
  sem_t response_ready;
  char func_name[20];
  double value;
  double result;
} shared_data_t;

enum Constants
{
  MAX_STRLEN = 4096
};

typedef double (*func_t) (double);

int
main (int argc, char *argv[])
{
  char *lib_name = argv[1];

  char shm_name[MAX_STRLEN] = "/ami-ypech-191";

  int fd = shm_open (shm_name, O_RDWR | O_CREAT, 0644);
  ftruncate (fd, sizeof (shared_data_t));
  shared_data_t *shdata = mmap (NULL, sizeof (shared_data_t),
                                PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  sem_init (&shdata->request_ready, 1, 0);
  sem_init (&shdata->response_ready, 1, 0);
  void *lib_handle = dlopen (lib_name, RTLD_NOW);
  printf ("%s", shm_name);
  fflush (stdout);

  while (true)
    {
      sem_wait (&shdata->request_ready);
      int sem_val;
      sem_getvalue (&shdata->request_ready, &sem_val);
      printf ("Semaphore value, after wait %d\n", sem_val);
      fflush (stdout);
      if (strlen (shdata->func_name) == 0)
        {
          break;
        }
      printf ("\nFunction name  %s\n", shdata->func_name);
      fflush (stdout);
      double (*func) (double) = dlsym (lib_handle, shdata->func_name);
      printf ("Function arg  %lf\n", shdata->value);
      fflush (stdout);
      shdata->result = func (shdata->value);
      printf ("Function ret  %lf\n", shdata->result);
      fflush (stdout);
      sem_post (&shdata->response_ready);
      sem_getvalue (&shdata->response_ready, &sem_val);
      printf ("Semaphore value, after post %d\n", sem_val);
      fflush (stdout);
    }
  dlclose (lib_handle);

  sem_destroy (&shdata->request_ready);
  sem_destroy (&shdata->response_ready);
  munmap (shdata, sizeof (shared_data_t));
  close (fd);
  return 0;
}
