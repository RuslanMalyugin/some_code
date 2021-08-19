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

int main(int argc, char* argv[]) {
  char *file_name = argv[1];
  FILE *fp;
  fp = fopen (file_name, "r");
  char buffer[1024];
  fread(buffer, sizeof(buffer), sizeof(buffer), fp);
  printf("%s", buffer);
  return 0;
}