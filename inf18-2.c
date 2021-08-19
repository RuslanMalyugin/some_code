#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

typedef enum
{
  DECREASE = -3,
  INCREASE = 5
} mode;

typedef struct
{
  int c;
} thread_task_result_t;

typedef struct
{
  mode mode_;
  int fd;
  pthread_t another_thread;
  int start_num;
  int position;
} thread_task_args_t;

static void *
read_and_write (const thread_task_args_t *arg)
{
  if (arg->position == 1)
    {
      int current = arg->start_num + arg->mode_;
      printf ("%d\n", current);
      write (arg->fd, &current, sizeof (int));
    }
  int current = 0;
  int inpt_chk = 0;
  do
    {
      read (arg->fd, &current, sizeof (int));
      if (current > 100 || current == 0)
        {
          int stop = 101;
          write (arg->fd, &stop, sizeof (int));
          break;
        }
      current += arg->mode_;
      printf ("%d\n", current);
      write (arg->fd, &current, sizeof (int));
    }
  while (inpt_chk != EOF && current <= 100 && current != 0);
  return NULL;
}
int
main (int argc, char *argv[])
{
  int fd[2];
  socketpair (PF_LOCAL, SOCK_STREAM, 0, fd);
  int start_num = strtol (argv[1], NULL, 10);
  pthread_t thread_1;
  pthread_t thread_2;
  thread_task_args_t args1 = { .mode_ = DECREASE,
                               .fd = fd[0],
                               .another_thread = thread_2,
                               .position = 1,
                               .start_num = start_num };
  thread_task_args_t args2 = { .mode_ = INCREASE,
                               .fd = fd[1],
                               .another_thread = thread_1,
                               .position = 2,
                               .start_num = 0 };
  pthread_create (&thread_1, NULL, (void *(*)(void *))read_and_write,
                  (void *)&args1);
  pthread_create (&thread_2, NULL, (void *(*)(void *))read_and_write,
                  (void *)&args2);
  pthread_join (thread_1, NULL);
  pthread_join (thread_2, NULL);
  close (fd[0]);
  close (fd[1]);
  return 0;
}