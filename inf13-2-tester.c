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

static void
SigHandler (int signum)
{
  static char buffer[200] = "signal SIGHUP received!";
  write (1, buffer, strlen (buffer));
}

int
main ()
{

  sigaction (SIGHUP,
             &(struct sigaction){
                 .sa_handler = SigHandler,
                 .sa_flags = SA_RESTART,
             },
             NULL);
  
  printf ("%d", getpid ());
  fflush (stdout);
  char FIFO_name[128];
  scanf ("%s", FIFO_name);
  int fd = open (FIFO_name, O_RDONLY);
  int size = 0;
  char buf[100];
  // читаем из fifo и пишем прочитанное в stdout
  size = read (fd, buf, sizeof (buf));
  close (fd);
  return 0;
}