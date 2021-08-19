#define _GNU_SOURCE
#include <assert.h>
#include <dlfcn.h>
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
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

const char SH_MEM_NAME[26] = "/ami-ypech-191";

typedef struct
{
  sem_t request_ready;  // начальное значение 0
  sem_t response_ready; // начальное значение 0
  char func_name[20];
  double value;
  double result;
} shared_data_t;

int
main (int argc, char *argv[])
{
  char *lib_name = argv[1];
  void *lib_handle = dlopen (lib_name, RTLD_NOW);
  int fd = shm_open (SH_MEM_NAME, O_RDWR, 0644);
  ftruncate (fd, sizeof (shared_data_t));
  shared_data_t *data = mmap (
      /* desired addr, addr = */ NULL,
      /* length = */ sizeof (shared_data_t),
      /* access attributes, prot = */ PROT_READ | PROT_WRITE,
      /* flags = */ MAP_SHARED,
      /* fd = */ fd,
      /* offset in file, offset = */ 0);
  close (fd);
  char name[4096];
  while (scanf ("%s", data->func_name) >= 0)
    {
      printf ("%s", "In cycle...\n");
      fflush (stdout);
      double val;
      scanf ("%lf", &data->value);
      sem_post (&data->request_ready);
      printf ("%s", "After post\n");
      fflush (stdout);
      sem_wait (&data->response_ready);
      printf ("Sended data %lf\n", data->result);
      fflush (stdout);
      printf ("%s", "After wait\n");
      fflush (stdout);
    }
  for (int i = 0; i < 20; ++i)
    data->func_name[i] = '\0';
  sem_post (&data->request_ready);
  printf ("%s", "After post\n");
  return 0;
}