#include <errno.h>
#include <linux/limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define FUSE_USE_VERSION 30
#include <fuse.h>

typedef struct
{
  char *file_name;
  size_t file_size;
  char *log_file_name;
} fuse_options_t;
fuse_options_t options;

typedef struct
{
  char *name;
  size_t size;
} file_info_t;

typedef struct
{
  size_t config_file_size;
  size_t files_count;
  file_info_t *files;
  char *data;
  size_t data_size;
  size_t offset;
} configs_t;
configs_t configs;

char working_directory[PATH_MAX];

int
getattr_callback (const char *path, struct stat *stbuf,
                  struct fuse_file_info *fi)
{
  (void)fi;
  if (strcmp (path, "/") == 0)
    {
      *stbuf = (struct stat){ .st_nlink = 2, .st_mode = S_IFDIR | 0755 };
      return 0;
    }
  for (int i = 0; i < configs.files_count; ++i)
    {
      if (!strncmp (path + 1, configs.files[i].name, PATH_MAX))
        {
          *stbuf = (struct stat){ .st_nlink = 2,
                                  .st_mode = S_IFREG | 0444,
                                  .st_size = (__off_t)configs.files[i].size };
          return 0;
        }
    }
  return -ENOENT;
}

int
readdir_callback (const char *path, void *buf, fuse_fill_dir_t filler,
                  off_t offset, struct fuse_file_info *fi,
                  enum fuse_readdir_flags flags)
{
  (void)offset;
  (void)fi;
  (void)flags; // unused variables
  filler (buf, ".", NULL, 0, (enum fuse_fill_dir_flags)0);
  filler (buf, "..", NULL, 0, (enum fuse_fill_dir_flags)0);

  for (int i = 0; i < configs.files_count; ++i)
    {
      filler (buf, configs.files[i].name, NULL, 0,
              (enum fuse_fill_dir_flags)0);
    }
  return 0;
}

int
read_callback (const char *path, char *buf, size_t size, off_t offset,
               struct fuse_file_info *fi)
{
  if (strcmp (path, "/") == 0)
    {
      return -EISDIR;
    }
  size_t data_offset = 0;
  for (int i = 0; i < configs.files_count; ++i)
    {
      if (!strncmp (path + 1, configs.files[i].name, PATH_MAX))
        {
          if (offset > configs.files[i].size)
            {
              return 0;
            }
          if (configs.files[i].size - offset < size)
            {
              size = configs.files[i].size - offset;
            }
          memcpy (buf, configs.data + configs.offset + data_offset + offset,
                  size);
          return size;
        }
      else
        {
          data_offset += configs.files[i].size;
        }
    }
  return -EIO;
}

struct fuse_operations fuse_example_operations
    = { .getattr = getattr_callback,
        .read = read_callback,
        .readdir = readdir_callback };

struct fuse_opt opt_specs[]
    = { { "--src %s", offsetof (fuse_options_t, file_name), 0 },
        FUSE_OPT_END };

int
main (int argc, char **argv)
{
  struct fuse_args args = FUSE_ARGS_INIT (argc, argv);

  fuse_opt_parse (&args, &options, opt_specs, NULL);

  char *full_file_path = realpath (options.file_name, NULL);
  free (options.file_name);
  options.file_name = full_file_path;

  int ret = fuse_main (args.argc, args.argv, &fuse_example_operations, NULL);
  fuse_opt_free_args (&args);
  return ret;
}