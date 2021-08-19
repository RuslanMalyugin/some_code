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

typedef struct Item
{
  struct Item *next;
  int value;
} item_t;

typedef struct
{
  size_t K;
  size_t I;
  _Atomic(item_t *) * current;
} thread_task_args_t;

static void *
insert_nodes (thread_task_args_t *args)
{
  for (size_t i = 0; i < args->K; ++i)
    {
      item_t *new_item = (item_t *)malloc (sizeof (item_t));
      new_item->value = args->I * args->K + i;
      item_t *next;
      do
        {
          next = *(args->current);
          new_item->next = next;
        }
      while (!atomic_compare_exchange_weak ((args->current), &next, new_item));
    }
  sched_yield ();
  return NULL;
}

void
clear_list (item_t *head)
{
  if (head->next)
    {
      clear_list (head->next);
      free (head);
    }
}

int
main (int argc, char *argv[])
{
  size_t N = strtoll (argv[1], NULL, 10);
  size_t k = strtoll (argv[2], NULL, 10);
  _Atomic(item_t *) root
      = (_Atomic(item_t *))malloc (sizeof (_Atomic(item_t)));
  pthread_t *threads = (pthread_t *)malloc ((N) * sizeof (pthread_t));
  thread_task_args_t *args
      = (thread_task_args_t *)malloc ((N) * sizeof (thread_task_args_t));
  for (size_t i = 0; i < N; ++i)
    {
      pthread_t thread;
      threads[i] = thread;
      args[i].I = i;
      args[i].K = k;
      args[i].current = &root;
      pthread_create (threads + i, NULL, (void *(*)(void *))insert_nodes,
                      (void *)&args[i]);
    }
  for (size_t i = 0; i < N; ++i)
    {
      pthread_join (threads[i], NULL);
    }
  for (item_t *start = root; start->next != NULL;)
    {
      printf ("%d\n", start->value);
      start = start->next;
    }
  clear_list (root);
  free (args);
  free (threads);
  return 0;
}