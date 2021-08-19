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
  if (argc == 1)
    {
      return 0;
    }
  else if (argc == 2)
    {
      pid_t pid = fork ();
      if (pid == 0)
        {
          execlp (argv[1], argv[1], NULL);
        }
      int status = 0;
      waitpid (pid, &status, 0);
      return 0;
    }

  int fd[2][2];
  pipe (fd[0]);
  pid_t pid_ = 0;
  pid_ = fork ();
  if (pid_ == 0)
    {
      dup2 (fd[0][1], 1);
      close (fd[0][0]);
      execlp (argv[1], argv[1], NULL);
    }
  int status = 0;
  waitpid (pid_, &status, 0);
  close (fd[0][1]);
  for (int i = 2; i < argc - 1; ++i)
    {
      pipe (fd[1]);
      pid_ = fork ();
      if (pid_ == 0)
        {
          dup2 (fd[0][0], 0);
          dup2 (fd[1][1], 1);
          close (fd[1][1]);
          execlp (argv[i], argv[i], NULL);
        }
      int status = 0;
      waitpid (pid_, &status, 0);
      close (fd[0][0]);
      close (fd[1][1]);
      fd[0][0] = fd[1][0];
    }
  pid_ = fork ();
  if (pid_ == 0)
    {
      dup2 (fd[0][0], 0);
      close (fd[0][0]);
      execlp (argv[argc - 1], argv[argc - 1], NULL);
    }
  waitpid (pid_, &status, 0);
  close (fd[0][0]);
  return 0;
}