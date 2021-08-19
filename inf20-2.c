#define _GNU_SOURCE
#include <ctype.h>
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

volatile int checker = 1;

typedef struct
{
  sem_t take;
  sem_t give;
  char item;
} data;

static void
handler (int signum)
{
  checker = 0;
}

static void
barman_work (data *info, pid_t pids[])
{
  char c;
  while ((c = getc (stdin)) >= 0)
    {
      if (isalpha (c))
        {
          info->item = toupper(c);
          sem_post (&info->take);
          sem_wait (&info->give);
        }
    }
  for (int i = 0; i < 3; ++i)
    {
      kill (pids[i], SIGTERM);
    }
  for (int i = 0; i < 3; ++i)
    {
      sem_post (&info->take);
      kill (pids[i], SIGTERM);
    }
  return;
}

static void
smoker_work (data *info)
{
  sigaction (
      SIGTERM,
      &(struct sigaction){ .sa_handler = handler, .sa_flags = SA_RESTART },
      NULL);
  while (checker)
    {
      sem_wait (&(info->take));
      if (checker == 0)
        break;

      printf ("%c ", info->item);
      fflush (stdout);
      sem_post (&info->give);
    }
  return;
}

int
main ()
{
  pid_t pid[3];
  data *info = mmap (
      /* desired addr, addr = */ NULL,
      /* length = */ sizeof (data),
      /* access attributes, prot = */ PROT_READ | PROT_WRITE,
      /* flags = */ MAP_SHARED | MAP_ANONYMOUS,
      /* fd = */ -1,
      /* offset in file, offset = */ 0);
  sem_init (&info->give, 1, 0);
  sem_init (&info->take, 1, 0);
  for (int i = 0; i < 3; ++i)
    {
      pid_t current_pid = fork ();
      if (current_pid == 0)
        {
          smoker_work (info);
          return 0;
        }
      pid[i] = current_pid;
    }
  barman_work (info, pid);
  for (int i = 0; i < 3; ++i)
    {
      int status;
      waitpid (pid[i], &status, 0);
    }
  return 0;
}