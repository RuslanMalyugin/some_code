#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

static void*
read_and_write (void * argv)
{
  int current;
  if (scanf ("%d", &current) != EOF)
    {
      pthread_t thread_chld;
      pthread_create (&thread_chld, NULL, read_and_write, 0);
      pthread_join (thread_chld, NULL);
      printf ("%d\n", current);
    }
    return NULL;
}

int
main ()
{
  pthread_t thread_chld;
  pthread_create (&thread_chld, NULL, read_and_write, 0);
  pthread_join (thread_chld, 0);
  return 0;
}