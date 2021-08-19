
#include <assert.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int
main (int argc, char *argv[])
{
  char *lib_name = argv[1];
  char *lib_func = argv[2];
  void *lib_handle = dlopen (lib_name, RTLD_NOW);
  double (*func) (double) = dlsym (lib_handle, lib_func);
  double num;
  while (scanf ("%lf", &num) >= 0)
    {
      printf ("%.3f ", func (num));
    }
  dlclose (lib_handle);
  return 0;
}