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

int
main (int argc, char *argv[])
{
  char *program = argv[1];
  int fd[2];
  pipe (fd);
  pid_t pid1 = fork ();
  if (pid1 == 0)
    {
      close (fd[0]);
      dup2 (fd[1], 1);
      execvp (program, argv + 1);
    }
  pid_t pid2 = fork ();
  if (pid2 == 0)
    {
      close (fd[1]);
      char buffer[1024];
      int size_of_out = read (fd[0], buffer, sizeof (buffer));
      printf ("%d\n", size_of_out);
      close (fd[0]);
    }

  close (fd[1]);
  close (fd[0]);
  waitpid (pid1, NULL, 0);
  waitpid (pid2, NULL, 0);
  return 0;
}