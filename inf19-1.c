#define _GNU_SOURCE
#include <errno.h>
#include <math.h>
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
  int64_t low;          // A
  int64_t high;         // B
  int32_t size;         // N
  int64_t *buf;         // buffer for primes
  pthread_mutex_t *mut; // mutex
  pthread_cond_t *cv;   // condvar
} thread_task_args_t;

int
is_prime (int64_t number)
{
  if (number == 1)
    return 0;
  if (number == 2)
    return 1;
  int64_t max = (int64_t)sqrt ((double)number) + 1;
  for (size_t i = 2; i < max; ++i)
    {
      if (number % i == 0)
        {
          return 0;
        }
    }
  return 1;
}

static void *
find_primes (thread_task_args_t *args)
{
  int counter = 0;
  for (int64_t i = args->low; i <= args->high; ++i)
    {
      if (is_prime (i) == 1)
        {
          if (counter >= args->size)
            {
              break;
            }
          pthread_mutex_lock (args->mut);
          args->buf[counter] = i;
          ++counter;
          pthread_mutex_unlock (args->mut);
          pthread_cond_signal (args->cv);
        }
    }
  args->buf[counter] = -1;
  pthread_mutex_lock (args->mut);
  pthread_mutex_unlock (args->mut);
  pthread_cond_signal (args->cv);
  return NULL;
}

int
main (int argc, char *argv[])
{
  int64_t A = strtoll (argv[1], NULL, 10);
  int64_t B = strtoll (argv[2], NULL, 10);
  int32_t N = strtoll (argv[3], NULL, 10);
  int64_t *buffer = (int64_t *)malloc ((N + 1) * sizeof (int64_t));
  for (size_t i = 0; i < N + 1; ++i)
    {
      buffer[i] = 0;
    }
  pthread_mutex_t mutex;
  pthread_cond_t condvar;
  pthread_mutex_init (&mutex, NULL);
  pthread_cond_init (&condvar, NULL);
  pthread_t thread;
  int val;
  val = 0;
  int counter = 0;
  thread_task_args_t args = { .buf = buffer,
                              .mut = &mutex,
                              .cv = &condvar,
                              .high = B,
                              .low = A,
                              .size = N };
  pthread_create (&thread, NULL, (void *(*)(void *))find_primes,
                  (void *)&args);
  while (counter < N)
    {
      pthread_mutex_lock (&mutex);
      while (buffer[counter] == 0)
        {
          pthread_cond_wait (&condvar, &mutex);
        }
      val = 0;
      if (buffer[counter] == -1)
        {
          pthread_mutex_unlock (&mutex);
          break;
        }
      ++counter;
      pthread_mutex_unlock (&mutex);
      printf ("%ld\n", buffer[counter - 1]);
    }
  pthread_join (thread, NULL);
  free (buffer);
  return 0;
}