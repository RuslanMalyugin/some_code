#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <string>
#include <unordered_set>
#include <vector>

#define FUSE_USE_VERSION 30
#include <fuse.h>

namespace fs = std::filesystem;

struct pathes {
    fs::path orig_;
    fs::path full_;
};

struct my_options_t {
    char* dirs_raw{};
    std::vector<fs::path> input_dirs;
    std::vector<pathes> file_tree;
};

my_options_t my_options;

std::vector<std::string> dir_splitter(const std::string& input_string,
                                      char split_c) {
    std::vector<std::string> splitted{};
    std::string cur_str;
    for (const auto& c : input_string) {
        if (c == split_c) {
            if (!cur_str.empty()) {
                splitted.push_back(cur_str);
                cur_str.clear();
            }
        } else {
            cur_str.push_back(c);
        }
    }
    if (!cur_str.empty()) {
        splitted.push_back(cur_str);
    }
    return splitted;
}

struct file_checker {
    bool operator()(const pathes& lhs,
                    const pathes& rhs) const {
        return (lhs.orig_ < rhs.orig_) ||
               (lhs.orig_ == rhs.orig_ &&
                fs::last_write_time(lhs.full_) > fs::last_write_time(rhs.full_));
    }
};

fs::path delete_pref(const fs::path& path, const fs::path& prefix) {
    size_t prefix_len = strlen(prefix.c_str());
    return fs::path(std::string(path.c_str() + prefix_len + 1));
}

void preprocess(const std::vector<fs::path>& dirs) {
    std::vector<pathes> all_files;
    for (const auto& dir : dirs) {
        for (const auto& file : fs::recursive_directory_iterator(dir.c_str())) {
            const auto& tmp_path = file.path();
            all_files.push_back({delete_pref(tmp_path, dir), tmp_path});
        }
        all_files.push_back({"", dir / ""});
    }
    std::sort(all_files.begin(), all_files.end(), file_checker{});
    my_options.file_tree = std::move(all_files);
}

fs::path get_real_path(const std::string& path_str) {
    return fs::absolute(fs::path(path_str));
}

int64_t get_last_changed_name(const std::string& path) {
    for (int64_t i = 0; i < my_options.file_tree.size(); ++i) {
        if (my_options.file_tree[i].orig_ == fs::path(path)) {
            return i;
        }
    }
    return -1;
}

int getattr_callback(const char* path, struct stat* stbuf,
                     struct fuse_file_info* fi) {
    (void)fi;
    auto idx = get_last_changed_name(path + 1);
    if (idx < 0) {
        return -ENOENT;
    }
    auto stat_ret = stat(my_options.file_tree[idx].full_.c_str(), stbuf);
    if (S_ISREG(stbuf->st_mode)) {
        stbuf->st_mode = S_IFREG | 0444;
    } else if (S_ISDIR(stbuf->st_mode)) {
        stbuf->st_mode = S_IFDIR | 0555;
    }
    return stat_ret;
}

int readdir_callback(const char* path, void* buf, fuse_fill_dir_t filler,
                     off_t offset, struct fuse_file_info* fi,
                     enum fuse_readdir_flags flags) {
    (void)offset;
    (void)fi;
    (void)flags;
    std::vector<fs::path> subdirs;
    for (const auto& input_dir : my_options.input_dirs) {
        auto real_path = input_dir / fs::path(path + 1);
        if (fs::exists(real_path) && fs::is_directory(fs::status(real_path))) {
            subdirs.push_back(real_path);
        }
    }
    if (subdirs.empty()) {
        return -ENOENT;
    }
    filler(buf, ".", nullptr, 0, (enum fuse_fill_dir_flags)0);
    filler(buf, "..", nullptr, 0, (enum fuse_fill_dir_flags)0);
    std::unordered_set<std::string> unique_filenames;
    for (const auto& catalogue : subdirs) {
        for (const auto& item : fs::directory_iterator(catalogue)) {
            std::string fname = item.path().filename().c_str();
            if (unique_filenames.find(fname) == unique_filenames.end()) {
                unique_filenames.insert(fname);
                filler(buf, fname.c_str(), nullptr, 0, (enum fuse_fill_dir_flags)0);
            }
        }
    }
    return 0;
}

int read_callback(const char* path, char* buf, size_t size, off_t offset,
                  struct fuse_file_info* fi) {
    if (strcmp(path, "/") == 0) {
        return -EISDIR;
    }
    auto& ftree = my_options.file_tree;
    auto idx = get_last_changed_name(path + 1);
    if (idx < 0) {
        return -ENOENT;
    }
    struct stat st{};
    stat(my_options.file_tree[idx].full_.c_str(), &st);

    if (S_ISDIR(st.st_mode)) {
        return -EISDIR;
    }

    size_t len = st.st_size;
    if (offset >= len) {
        return 0;
    }
    size = (offset + size <= len) ? size : (len - offset);
    int fd = open(my_options.file_tree[idx].full_.c_str(), O_RDONLY, 0555);
    lseek(fd, offset, SEEK_SET);
    read(fd, buf, size);
    close(fd);
    return size;
}

struct fuse_operations fuse_example_operations = {
        .getattr = getattr_callback,
        .read = read_callback,
        .readdir = readdir_callback,
};

int main(int argc, char** argv) {
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
    struct fuse_opt opt_specs[] = {
            {"--src %s", offsetof(my_options_t, dirs_raw), 0}, FUSE_OPT_END};
    fuse_opt_parse(&args, &my_options, opt_specs, nullptr);
    auto rel_dirs = dir_splitter(std::string(my_options.dirs_raw), ':');
    for (const auto& rel_dir : rel_dirs) {
        my_options.input_dirs.push_back(get_real_path(rel_dir));
    }
    preprocess(my_options.input_dirs);
    int ret = fuse_main(args.argc, args.argv, &fuse_example_operations, nullptr);
    fuse_opt_free_args(&args);
    return 0;
}