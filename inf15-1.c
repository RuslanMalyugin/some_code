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
  char *program1 = argv[1];
  char *program2 = argv[2];
  int fd[2];
  pipe (fd);
  pid_t pid = fork ();
  if (pid == 0)
    {
      dup2 (fd[1], 1);
      close (fd[1]);
      close (fd[0]);
      execlp (program1, program1, NULL);
    }
  close (fd[1]);
  pid_t pid_2 = fork ();
  if (pid_2 == 0)
    {
      dup2 (fd[0], 0);
      close (fd[0]);
      execlp (program2, program2, NULL);
    }
  close (fd[0]);
  int status;
  waitpid (pid, &status, 0);
  waitpid (pid_2, &status, 0);
  return 0;
}