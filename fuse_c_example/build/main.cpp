#include <cerrno>
#include <cinttypes>
#include <climits>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <deque>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>
#include <unordered_set>
#include <vector>

#define FUSE_USE_VERSION 30
#include <fuse.h>

using namespace std::filesystem;

vector<path> in_dirs;

typedef struct
{
  char *string_of_dirs;
} my_options_t;
my_options_t my_options;

path
time_from_absolute_path (const path &relative_path)
{
  // нужен тип переменной
  decltype (last_write_time (relative_path)) last_time;
  bool is_exist = false;
  path max_path = relative_path;
  // нужно какое-то вхождение

  // перебор всех абсолютных путей входящих папок

  for (path &dir : in_dirs)
    {
      //  и внутреннего каталога
      path absolute_path = dir / relative_path;
      if (relative_path == "")
        {
          absolute_path = dir;
        }
      // если существует такая папка

      if (exists (absolute_path) && last_write_time (absolute_path) > last_time 
          || !is_exist)
        {
          is_exist = true;
          last_time = last_write_time (absolute_path);
          max_path = absolute_path;
        }
    }
  return max_path;
}

// Самый важный колбэк. Вызывается первым при любом другом колбэке.
// Заполняет структуру stbuf.
int
GetAttrCallback (const char *relative_path, struct stat *stbuf,
                 struct fuse_file_info *fi)
{
  (void)fi;

  stat (absolute (time_from_absolute_path (relative_path + 1)).c_str (),
        stbuf);

  if (S_ISREG (stbuf->st_mode))
    {
      stbuf->st_mode &= S_IFREG | 0444;
    }

  if (S_ISDIR (stbuf->st_mode))
    {
      stbuf->st_mode &= S_IFDIR | 0555;
    }

  return 0; // При ошибке, вместо errno возвращаем (-errno).
}
//
// filler(buf, filename, stat, flags) -- заполняет информацию о файле и
// вставляет её в buf.

int
ReadDirCallback (const char *relative_path, void *buf, fuse_fill_dir_t filler,
                 off_t offset, struct fuse_file_info *fi,
                 enum fuse_readdir_flags flags)
{
  (void)offset;
  (void)fi;
  (void)flags; // unused variables
  filler (buf, ".", nullptr, 0, (enum fuse_fill_dir_flags)0);
  filler (buf, "..", nullptr, 0, (enum fuse_fill_dir_flags)0);

  ++relative_path;
  std::unordered_set<std::string> one_names;
  vector<path> paths;
  for (path &dir : in_dirs)
    {
      // перебор всех абсолютных путей входящих папок и внутреннего каталога
      path absolute_path = dir / relative_path;
      // если она существует и она директория
      if (exists (absolute_path)
          && is_directory (directory_entry (absolute_path).status ()))
        {
          // перебираем свойства директорий
          for (const directory_entry &properties :
               directory_iterator (absolute_path))
            {
              // кастуем к обычной си строке и добавляем в список файлов
              // количестве 1
              string file_name (path (properties).filename ().c_str ());

              if (one_names.count (file_name) == 0)
                {
                  one_names.insert (file_name);
                  filler (buf, file_name.data (), nullptr, 0,
                          (enum fuse_fill_dir_flags)0);
                }
            }
        }
    }

  return 0;
}

//// Вызывается после успешной обработки open.
int
ReadCallback (const char *relative_path, char *buf, size_t size, off_t offset,
              struct fuse_file_info *fi)
{

  // print_cwd();
  // "/my_file"

  path real_path = time_from_absolute_path (relative_path + 1);
  if (!is_regular_file (real_path))
    {
      return 0;
    }

  uint64_t len = file_size (real_path);
  if (offset >= len)
    {
      return 0;
    }

  size = (offset + size <= len) ? size : (len - offset);
  // fseek как в прошлой задаче
  ifstream file (real_path);
  file.seekg (offset, std::ios_base::beg);
  // memcpy(buf, BUFFER + offset, size);
  file.read (buf, size);

  return size;
}

// Структура с колбэками.
struct fuse_operations fuse_example_operations = {
  .getattr = GetAttrCallback,
  .read = ReadCallback,
  .readdir = ReadDirCallback,
};

struct fuse_opt opt_specs[] = {
  { "--src %s", offsetof (my_options_t, string_of_dirs), 0 },
  FUSE_OPT_END // Структурка заполненная нулями. В общем такой типичный
               // zero-terminated массив
};

int
main (int argc, char **argv)
{
  struct fuse_args args = FUSE_ARGS_INIT (argc, argv);
  /*
   * ВАЖНО: заполняемые поля должны быть инициализированы нулями.
   * (В противном случае fuse3 может делать что-то очень плохое. TODO)
   */
  fuse_opt_parse (&args, &my_options, opt_specs, nullptr);
  //// init
  std::istringstream string_stream (my_options.string_of_dirs);
  string dir;

  //// allocate string_of_dirs
  while (getline (string_stream, dir, ':'))
    {
      in_dirs.push_back (absolute (std::filesystem::path (dir)));
    }

  //// perform
  int ret
      = fuse_main (args.argc, args.argv, &fuse_example_operations, nullptr);

  //// free
  fuse_opt_free_args (&args);
  return ret;
}
