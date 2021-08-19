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
  const size_t BUFFER_SIZE = 4096;
  char *program = argv[1];
  int fd[2];
  pipe (fd);
  pid_t pid = fork ();
  if (pid == 0)
    {
      dup2 (fd[1], 2);
      close (fd[1]);
      close (fd[0]);
      execlp ("gcc", "gcc", program, NULL);
    }
  int status;
  waitpid (pid, &status, 0);
  close (fd[1]);
  dup2 (fd[0], 0);
  close (fd[0]);
  char current_string[BUFFER_SIZE];
  int error_counter = 0;
  int warning_counter = 0;
  int prev_error = -1;
  int prev_warning = -1;
  int curr_error = -1;
  int curr_warning = -1;
  while (fgets (current_string, BUFFER_SIZE, stdin))
    {
      if (strstr (current_string, "error"))
        {
          scanf("%d", &curr_error);
          if (curr_error != prev_error)
            ++error_counter;
          prev_error = curr_error;
        }
      if (strstr (current_string, "warning"))
        {
          scanf ("%d", &curr_warning);
          if (curr_warning != prev_warning)
            ++warning_counter;
          prev_warning = curr_warning;
        }
    }
  printf ("%d ", error_counter);
  printf ("%d", warning_counter);
  return 0;
}