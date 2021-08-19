#include <unistd.h>
#include <wait.h>

int exec_error = 0;

int
main (int argc, char *argv[])
{

  if (argc == 2)
    {
      execlp (argv[1], argv[1], NULL);
      _exit (1);
    }
  else if (argc == 1)
    {
      return 0;
    }

  int pipes_[2][2];
  pipe (pipes_[0]);

  int in = 0;
  int out = 0;
  for (int i = 1; i < argc - 1; ++i)
    {
      if (exec_error == 1)
        {
          close (pipes_[in][1]);
          close (pipes_[in][0]);
          close (pipes_[out][0]);
          close (pipes_[out][1]);
          _exit (1);
        }
      if (i == 1)
        {
          out = 0;
          in = 1;

          pid_t child_pid = fork ();
          if (child_pid == 0)
            {
              dup2 (pipes_[out][1], 1);
              close (pipes_[out][0]);
              execlp (argv[i], argv[i], NULL);
              exec_error = 1;
              _exit (1);
            }

          pipes_[1][0] = -1;

          int status = 0;
          waitpid (child_pid, &status, 0);
        }
      else
        {
          if ((i - 1) % 2 == 0)
            {
              in = 1;
              out = 0;
            }
          else
            {
              in = 0;
              out = 1;
            }

          close (pipes_[in][1]);
          close (pipes_[out][0]);
          pipe (pipes_[out]);

          pid_t child_pid = fork ();
          if (child_pid == 0)
            {
              dup2 (pipes_[out][1], 1);
              close (pipes_[out][0]);
              dup2 (pipes_[in][0], 0);
              execlp (argv[i], argv[i], NULL);
              exec_error = 1;
              close (pipes_[in][1]);
              close (pipes_[in][0]);
              close (pipes_[out][0]);
              close (pipes_[out][1]);
              _exit (1);
            }

          int status = 0;
          waitpid (child_pid, &status, 0);
        }
    }
  if (exec_error == 1)
    {
      close (pipes_[in][1]);
      close (pipes_[in][0]);
      close (pipes_[out][0]);
      close (pipes_[out][1]);
      _exit (1);
    }

  close (pipes_[in][0]);
  close (pipes_[out][1]);

  in = out;
  pid_t child_pid = fork ();
  if (child_pid == 0)
    {
      dup2 (pipes_[in][0], 0);
      execlp (argv[argc - 1], argv[argc - 1], NULL);
      exec_error = 1;
      close (pipes_[in][1]);
      _exit (1);
    }

  if (exec_error == 1)
    {
      close (pipes_[in][1]);
      close (pipes_[out][0]);
      _exit (1);
    }

  close (pipes_[in][0]);

  int status = 0;
  waitpid (child_pid, &status, 0);

  return 0;
}
