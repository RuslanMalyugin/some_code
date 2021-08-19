#define _GNU_SOURCE
#include <errno.h>
#include <pthread.h>
#include <stdatomic.h>
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
  int iterations;
  int index;
  int count;
  double *middle_value;
  double *left_value;
  double *right_value;
  pthread_mutex_t *middle_mutex;
  pthread_mutex_t *left_mutex;
  pthread_mutex_t *right_mutex;

} thread_task_args_t;

static void *
lock_and_write (const thread_task_args_t *arg)
{
  for (size_t i = 0; i < arg->iterations; ++i)
    {
      if (arg->count == 1)
        {
          pthread_mutex_lock (arg->right_mutex);
          *(arg->right_value) += 1.01;
          *(arg->left_value) += 0.99;
          *(arg->middle_value) += 1;
          pthread_mutex_unlock (arg->right_mutex);
        }
      else if (arg->count == 2)
        {
          if (arg->index == 0)
            {
              pthread_mutex_lock (arg->middle_mutex);
              *(arg->middle_value) += 1;
              pthread_mutex_lock (arg->left_mutex);
              *(arg->left_value) += 0.99;
              *(arg->right_value) += 1.01;
              pthread_mutex_unlock (arg->left_mutex);
              pthread_mutex_unlock (arg->middle_mutex);
            }
          if (arg->index == 1)
            {
              pthread_mutex_lock (arg->left_mutex);
              *(arg->middle_value) += 1;
              pthread_mutex_lock (arg->middle_mutex);
              *(arg->left_value) += 0.99;
              *(arg->right_value) += 1.01;
              pthread_mutex_unlock (arg->middle_mutex);
              pthread_mutex_unlock (arg->left_mutex);
            }
        }
      else
        {
          if (arg->index % 3 == 2)
            {

              pthread_mutex_lock (arg->right_mutex);
              *(arg->right_value) += 1.01;
              pthread_mutex_lock (arg->middle_mutex);
              *(arg->middle_value) += 1;
              pthread_mutex_lock (arg->left_mutex);
              *(arg->left_value) += 0.99;
              pthread_mutex_unlock (arg->left_mutex);
              pthread_mutex_unlock (arg->middle_mutex);
              pthread_mutex_unlock (arg->right_mutex);
            }
          if (arg->index % 3 == 0)
            {
              pthread_mutex_lock (arg->middle_mutex);
              *(arg->middle_value) += 1;
              pthread_mutex_lock (arg->left_mutex);
              *(arg->left_value) += 0.99;
              pthread_mutex_lock (arg->right_mutex);
              *(arg->right_value) += 1.01;
              pthread_mutex_unlock (arg->right_mutex);
              pthread_mutex_unlock (arg->left_mutex);
              pthread_mutex_unlock (arg->middle_mutex);
            }
          if (arg->index % 3 == 1)
            {
              pthread_mutex_lock (arg->left_mutex);
              *(arg->left_value) += 0.99;
              pthread_mutex_lock (arg->right_mutex);
              *(arg->right_value) += 1.01;
              pthread_mutex_lock (arg->middle_mutex);
              *(arg->middle_value) += 1;
              pthread_mutex_unlock (arg->middle_mutex);
              pthread_mutex_unlock (arg->right_mutex);
              pthread_mutex_unlock (arg->left_mutex);
            }
          // pthread_mutex_unlock (arg->right_mutex);
          // pthread_mutex_unlock (arg->middle_mutex);
          // pthread_mutex_unlock (arg->left_mutex);
        }
    }
}

int
main (int argc, char *argv[])
{
  int iteration_count = strtol (argv[1], NULL, 10);
  int threads_count = strtol (argv[2], NULL, 10);
  pthread_t *threads
      = (pthread_t *)malloc (threads_count * sizeof (pthread_t));

  pthread_mutex_t *mutexes
      = (pthread_mutex_t *)malloc (threads_count * sizeof (pthread_mutex_t));

  double *buffer = (double *)malloc (threads_count * sizeof (double));

  for (size_t i = 0; i < threads_count; ++i)
    {
      buffer[i] = 0.0;
    }

  for (size_t i = 0; i < threads_count; ++i)
    {
      pthread_mutex_init (mutexes + i, NULL);
    }

  thread_task_args_t *args = (thread_task_args_t *)malloc (
      (threads_count) * sizeof (thread_task_args_t));

  for (size_t i = 0; i < threads_count; ++i)
    {
      pthread_t thread;
      threads[i] = thread;
      args[i].middle_value = buffer + i;
      args[i].middle_mutex = mutexes + i;
      args[i].left_value = buffer + ((i - 1 + threads_count) % threads_count);
      args[i].left_mutex = mutexes + ((i - 1 + threads_count) % threads_count);
      args[i].right_value = buffer + ((i + 1) % threads_count);
      args[i].right_mutex = mutexes + ((i + 1) % threads_count);
      args[i].iterations = iteration_count;
      args[i].index = i;
      args[i].count = threads_count;
      pthread_create (&threads[i], NULL, (void *(*)(void *))lock_and_write,
                      (void *)&args[i]);
    }

  for (size_t i = 0; i < threads_count; ++i)
    {
      pthread_join (threads[i], NULL);
    }

  for (size_t i = 0; i < threads_count; ++i)
    {
      printf ("%.10g\n", buffer[i]);
    }

  free (buffer);
  free (threads);
  free (mutexes);
  return 0;
}