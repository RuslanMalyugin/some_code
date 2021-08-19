#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct
{
  int c;
} thread_task_result_t;

static thread_task_result_t *
partional_sum (void *argv)
{
  thread_task_result_t *result
      = (thread_task_result_t *)malloc (sizeof (thread_task_result_t));
  int current = 0;
  int sum = 0;
  int inpt_chk = 0;
  do
    {
      sum += current;
      inpt_chk = scanf ("%d", &current);
    }
  while (inpt_chk != EOF);

  result->c = sum;
  return result;
}

int
main (int argc, char *argv[])
{
  int threads_count = strtol (argv[1], NULL, 10);
  pthread_t *threads
      = (pthread_t *)malloc (threads_count * sizeof (pthread_t));

  for (size_t i = 0; i < threads_count; ++i)
    {
      pthread_t thread;
      threads[i] = thread;
      pthread_attr_t thread_attr;
      pthread_attr_init (&thread_attr);
      pthread_attr_setstacksize (&thread_attr, PTHREAD_STACK_MIN);
      pthread_create (&threads[i], &thread_attr,
                                 (void *(*)(void *))partional_sum, 0);
      pthread_attr_destroy (&thread_attr);
    }
  int full_sum = 0;
  thread_task_result_t *result;
  for (size_t i = 0; i < threads_count; ++i)
    {
      pthread_join (threads[i], (void **)&result);
      full_sum += result->c;
    }
  free (result);
  printf("%d", full_sum);
  return 0;
}