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
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int
main ()
{
  char sem_nm[4096];
  char sh_mem_nm[4096];
  int num_cnt;
  scanf ("%s %s %d", sem_nm, sh_mem_nm, &num_cnt);
  sem_t *sem = sem_open (sem_nm, 0);
  int fd = shm_open (sh_mem_nm, O_RDWR, 0644);
  int *buffer;
  int buf_size = num_cnt * sizeof (int);
  ftruncate (fd, buf_size);
  sem_wait (sem);
  int *sh_buf = mmap (
      /* desired addr, addr = */ NULL,
      /* length = */ buf_size,
      /* access attributes, prot = */ PROT_READ,
      /* flags = */ MAP_SHARED,
      /* fd = */ fd,
      /* offset in file, offset = */ 0);
  for (int i = 0; i < num_cnt; ++i)
    {
      printf ("%d\n", sh_buf[i]);
    }
  sem_close (sem);
  close (fd);
  return 0;
}