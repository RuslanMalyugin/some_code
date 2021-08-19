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
  int size;
  int iterations;
  int index;
  double *buf;
  pthread_mutex_t *mut;
  int *lock;
} thread_task_args_t;

static void *
lock_and_write (const thread_task_args_t *arg)
{
  for (size_t k = 0; k < arg->iterations; ++k)
    {
      int i = arg->index;
      // pthread_mutex_lock (arg->left_mutex);
      // pthread_mutex_lock (arg->middle_mutex);
      // pthread_mutex_lock (arg->right_mutex);
      double val = 0.0;

      if (*(arg->lock + i) == 1)
        {
          pthread_mutex_lock (arg->mut);
          *(arg->lock + i) = 1;
          if (*(arg->lock + (1 + i) % arg->size) == 1)
            {

              pthread_mutex_lock (arg->mut + (1 + i) % arg->size);
              *(arg->lock + (1 + i) % arg->size) = 1;
              pthread_mutex_lock (arg->mut + (i - 1 + arg->size) % arg->size);
              *(arg->lock + (i - 1 + arg->size) % arg->size) = 1;
              *(arg->buf + i) += 1.0;
              *(arg->buf + (i - 1 + arg->size) % arg->size) += 1.01;
              *(arg->buf + (1 + i) % arg->size) += 0.99;
              *(arg->lock + (i - 1 + arg->size) % arg->size) = 0;
              pthread_mutex_unlock (arg->mut
                                    + (i - 1 + arg->size) % arg->size);
              *(arg->lock + (1 + i) % arg->size) = 0;
              pthread_mutex_unlock (arg->mut + (1 + i) % arg->size);
              *(arg->lock + i) = 0;
            }
          else
            {

              pthread_mutex_lock (arg->mut + (i - 1 + arg->size) % arg->size);
              *(arg->lock + (1 + i) % arg->size) = 1;
              pthread_mutex_lock (arg->mut + (1 + i) % arg->size);
              *(arg->lock + (i - 1 + arg->size) % arg->size) = 1;
              *(arg->buf + i) += 1.0;
              *(arg->buf + (i - 1 + arg->size) % arg->size) += 1.01;
              *(arg->buf + (1 + i) % arg->size) += 0.99;
              *(arg->lock + (i - 1 + arg->size) % arg->size) = 0;
              pthread_mutex_unlock (arg->mut + (1 + i) % arg->size);
              *(arg->lock + (1 + i) % arg->size) = 0;
              pthread_mutex_unlock (arg->mut
                                    + (i - 1 + arg->size) % arg->size);
              *(arg->lock + i) = 0;
            }
          pthread_mutex_unlock (arg->mut);
        }
      else
        {
          if (*(arg->lock + (1 + i) % arg->size) == 1)
            {
              pthread_mutex_lock (arg->mut + (1 + i) % arg->size);
              *(arg->lock + (1 + i) % arg->size) = 1;
              pthread_mutex_lock (arg->mut + (i - 1 + arg->size) % arg->size);
              *(arg->lock + (i - 1 + arg->size) % arg->size) = 1;
              pthread_mutex_lock (arg->mut);
              *(arg->lock + i) = 1;
              *(arg->buf + i) += 1.0;
              *(arg->buf + (i - 1 + arg->size) % arg->size) += 1.01;
              *(arg->buf + (1 + i) % arg->size) += 0.99;
              *(arg->lock + i) = 0;
              pthread_mutex_unlock (arg->mut);
              *(arg->lock + (i - 1 + arg->size) % arg->size) = 0;
              pthread_mutex_unlock (arg->mut
                                    + (i - 1 + arg->size) % arg->size);
              *(arg->lock + (1 + i) % arg->size) = 0;
              pthread_mutex_unlock (arg->mut + (1 + i) % arg->size);
            }
          else
            {
              pthread_mutex_lock (arg->mut + (i - 1 + arg->size) % arg->size);
              *(arg->lock + (i - 1 + arg->size) % arg->size) = 1;
              pthread_mutex_lock (arg->mut + (1 + i) % arg->size);
              *(arg->lock + (1 + i) % arg->size) = 1;
              pthread_mutex_lock (arg->mut);
              *(arg->lock + i) = 1;
              *(arg->buf + i) += 1.0;
              *(arg->buf + (i - 1 + arg->size) % arg->size) += 1.01;
              *(arg->buf + (1 + i) % arg->size) += 0.99;
              *(arg->lock + i) = 0;
              pthread_mutex_unlock (arg->mut);
              *(arg->lock + (1 + i) % arg->size) = 0;
              pthread_mutex_unlock (arg->mut + (1 + i) % arg->size);
              *(arg->lock + (i - 1 + arg->size) % arg->size) = 0;
              pthread_mutex_unlock (arg->mut
                                    + (i - 1 + arg->size) % arg->size);
            }
        }


        
      // if (arg->index % 3 == 1)
      //   {
      //     pthread_mutex_lock (arg->middle_mutex);
      //     val = *(arg->middle_value);
      //     *(arg->middle_value) = val + 1;
      //     pthread_mutex_lock (arg->left_mutex);
      //     val = *(arg->left_value);
      //     *(arg->left_value) = val + 0.99;
      //     pthread_mutex_lock (arg->right_mutex);
      //     val = *(arg->right_value);
      //     *(arg->right_value) = val + 1.01;
      //     pthread_mutex_unlock (arg->right_mutex);
      //     pthread_mutex_unlock (arg->left_mutex);
      //     pthread_mutex_unlock (arg->middle_mutex);
      //   }

      // if (arg->index % 3 == 0)
      //   {

      //     pthread_mutex_lock (arg->right_mutex);
      //     val = *(arg->right_value);
      //     *(arg->right_value) = val + 1.01;
      //     pthread_mutex_lock (arg->middle_mutex);
      //     val = *(arg->middle_value);
      //     *(arg->middle_value) = val + 1;
      //     pthread_mutex_lock (arg->left_mutex);
      //     val = *(arg->left_value);
      //     *(arg->left_value) = val + 0.99;
      //     pthread_mutex_unlock (arg->left_mutex);
      //     pthread_mutex_unlock (arg->middle_mutex);
      //     pthread_mutex_unlock (arg->right_mutex);
      //   }

      // if (arg->index % 3 == 2)
      //   {

      //     pthread_mutex_lock (arg->left_mutex);
      //     val = *(arg->left_value);
      //     *(arg->left_value) = val + 0.99;
      //     pthread_mutex_lock (arg->right_mutex);
      //     val = *(arg->right_value);
      //     *(arg->right_value) = val + 1.01;
      //     pthread_mutex_lock (arg->middle_mutex);
      //     val = *(arg->middle_value);
      //     *(arg->middle_value) = val + 1;
      //     pthread_mutex_unlock (arg->middle_mutex);
      //     pthread_mutex_unlock (arg->right_mutex);
      //     pthread_mutex_unlock (arg->left_mutex);
      //   }
      // pthread_mutex_unlock (arg->right_mutex);
      // pthread_mutex_unlock (arg->middle_mutex);
      // pthread_mutex_unlock (arg->left_mutex);
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

  int *locked = (int *)malloc (threads_count * sizeof (int));

  for (size_t i = 0; i < threads_count; ++i)
    {
      locked[i] = 0;
    }

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
      args[i].buf = buffer;
      args[i].mut = mutexes;
      args[i].iterations = iteration_count;
      args[i].index = i;
      args[i].lock = locked;
      args[i].size = threads_count;
      pthread_create (threads + i, NULL, (void *(*)(void *))lock_and_write,
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
  free (locked);
  free (buffer);
  free (threads);
  free (mutexes);
  free (args);
  return 0;
}