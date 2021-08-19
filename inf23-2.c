#include <arpa/inet.h>
#include <assert.h>
#include <ctype.h>
#include <curl/curl.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

typedef struct
{
  char *data;
  size_t length;
  size_t capacity;
} buffer_t;

static size_t
callback_function (char *ptr, size_t chunk_size, size_t nmemb, void *user_data)
{
  buffer_t *buffer = user_data;
  size_t total_size = chunk_size * nmemb;
  size_t req_size = buffer->length + total_size;
  if (req_size > buffer->capacity)
    {
      buffer->data = realloc (buffer->data, req_size * 2);
      buffer->capacity = req_size * 2;
    }
  memcpy (buffer->data + buffer->length, ptr, total_size);
  buffer->length += total_size;
  return total_size;
}

int
main (int argc, char *argv[])
{
  char *URL = argv[1];
  CURL *curl = curl_easy_init ();
  if (curl)
    {
      CURLcode res;
      curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, callback_function);
      buffer_t buffer;
      buffer.data = calloc (1024 * 1024, 1);
      buffer.length = 0;
      buffer.capacity = 0;
      curl_easy_setopt (curl, CURLOPT_WRITEDATA, &buffer);
      curl_easy_setopt (curl, CURLOPT_URL, URL);
      res = curl_easy_perform (curl);
      char *title = strstr (buffer.data, "<title>");
      char *title2 = strstr (buffer.data, "</title>");
      write (1, title + 7, (int)(title2 - title) - 7);
      free (buffer.data);
      curl_easy_cleanup (curl);
    }
  return 0;
}
