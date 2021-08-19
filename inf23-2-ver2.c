#include <curl/curl.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct
{
  char *data;
  size_t size;
  size_t capacity;
} simple_vector_t;

void
realocate_vector (simple_vector_t *simple_vector, size_t plus_size)
{
  if (simple_vector->size + plus_size > simple_vector->capacity)
    {
      simple_vector->data
          = realloc (simple_vector->data, simple_vector->size + plus_size);
      simple_vector->capacity += plus_size;
    }
  return;
}

static size_t
get_page (char *ptr, size_t size, size_t amount, void *user_data)
{
  simple_vector_t *simple_vector = user_data;
  size_t full_size = size * amount;
  realocate_vector (simple_vector, simple_vector->size + full_size);
  memcpy (simple_vector->data + simple_vector->size, ptr, full_size);
  simple_vector->size += full_size;
  return full_size;
}

int
main (int argc, char *argv[])
{
  CURL *curl = curl_easy_init ();
  if (curl)
    {
      CURLcode result;
      curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, get_page);
      simple_vector_t simple_vector;
      simple_vector.data = calloc (4096, 1);
      simple_vector.size = 0;
      simple_vector.capacity = 4096;
      curl_easy_setopt (curl, CURLOPT_WRITEDATA, &simple_vector);
      curl_easy_setopt (curl, CURLOPT_URL, argv[1]);
      result = curl_easy_perform (curl);
      char *begin, *end;
      begin = strstr (simple_vector.data, "<title>");
      end = strstr (simple_vector.data, "</title>");
      begin += sizeof ("<title>") - 1;
      size_t size = end - begin;
      write (1, begin, size);
      free (simple_vector.data);
      curl_easy_cleanup (curl);
    }
  return 0;
}