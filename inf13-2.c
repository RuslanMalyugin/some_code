#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
volatile int chk_stop = 0;
volatile int counter = 0;

int
find_size (int data_written_len)
{
  int power = 10;
  int num = 1;
  int count = 0;
  while (data_written_len > 0)
    {
      for (int i = 0; (i < power) && (data_written_len > 0); i++)
        {
          data_written_len -= (num + 1);
          count++;
        }
      power *= 10;
      num++;
    }
  return count;
}
// static void
// handler (int signum)
// {
//   if (signum == SIGPIPE)
//     {
//       return;
//     }
// }

int
main (int argc, char *argv[])
{
  sigset_t mask;
  sigaddset (&mask, SIGPIPE);
  sigprocmask (SIG_BLOCK, &mask, NULL);
  // {
  //   sigaction (
  //       SIGPIPE,
  //       &(struct sigaction){ .sa_handler = handler, .sa_flags = SA_RESTART
  //       }, NULL);
  // }f

  char *FIFO_name = argv[1];
  int N = strtoll (argv[2], NULL, 10);
  char data[4096] = "";
  int cn = 0;
  for (int i = 0; i <= N; ++i)
    {
      cn += snprintf (data + cn, 4096 - cn, "%d ", i);
    }
  // printf ("%s", data);
  int PID;
  scanf ("%d", &PID);
  kill (PID, SIGHUP);
  mkfifo (FIFO_name, 0644);
  int fd = open (FIFO_name, O_WRONLY);
  counter = write (fd, &data, strlen (data));
  close (fd);
  // printf ("%d \n", counter);
  counter = find_size (counter);
  printf ("%d", counter);
  return 0;
}