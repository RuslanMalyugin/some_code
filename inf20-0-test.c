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
main (int argc, char *argv[])
{
  char *sh_mem_nm = argv[1];
  char *sem_nm = argv[2];
  int size = strtol (argv[3], NULL, 10);
  sem_t *sem = sem_open (sem_nm, O_CREAT, 0644, 0);
  int fd = shm_open (sh_mem_nm, O_RDWR | O_CREAT, 0644);
  int buf_size = size * sizeof (int);
  //int *buffer = (int *)malloc (buf_size);
  ftruncate(fd, buf_size);
  
  int *sh_buf = mmap (
      /* desired addr, addr = */ NULL,
      /* length = */ buf_size,
      /* access attributes, prot = */ PROT_READ | PROT_WRITE,
      /* flags = */ MAP_SHARED,
      /* fd = */ fd,
      /* offset in file, offset = */ 0);
  for (int i = 0; i < size; ++i)
    {
      sh_buf[i] = i;
    }
  for (int i = 0; i < size; ++i)
    {
      printf("%d\n",sh_buf[i]);
    }
  //write (fd, buffer, sizeof (int) * size);
  //printf ("%s", "after write");
  sem_post (sem);
  sem_close (sem);
  return 0;
}