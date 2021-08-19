#include <stdint.h>
#include <stdio.h>
#include <windows.h>

struct Item
{
  int value;
  uint32_t next_pointer;
};

int
main (int argc, char *argv[])
{

  struct Item test;
  size_t struct_size = sizeof (test);

  HANDLE fileHandle = CreateFile (argv[1], GENERIC_READ, FILE_SHARE_READ, NULL,
                                  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  DWORD bytes_read;
  LARGE_INTEGER u_winFsz;
  GetFileSizeEx (fileHandle, &u_winFsz);
  int64_t _file_size = u_winFsz.QuadPart;
  if (_file_size / struct_size == 0)
    return 0;
  char *buffer = (char *)malloc (_file_size);
  ReadFile (fileHandle, buffer, _file_size, &bytes_read, NULL);
  size_t ptr = 0;
  struct Item *current = (struct Item *)buffer;
  do
    {
      printf ("%d ", current[ptr / 8].value);
      ptr = current[ptr / 8].next_pointer;
    }
  while (ptr != 0);
  CloseHandle (fileHandle);
  return 0;
}

/*
Run: i686-w64-mingw32-gcc winapi_example.c -o winapi_example.exe

Run: echo "Hello students!" > winapi_example_input_001.txt

Run: WINEDEBUG=-all wine winapi_example.exe winapi_example_input_001.txt
*/