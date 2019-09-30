#include <ustd/file_system.h>
#include <vector>
#include <memory>
#include <fstream>
#include <assert.h>
#include <string.h>
#ifdef WIN32
#include <filesystem>
#include <Windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#endif

namespace ustd
{
namespace detail
{
template <typename CharT>
bool write_file(
    const CharT *filename,
    const char *data,
    int size)
{
    std::ofstream file(filename,
                       std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
    if (!file.good())
    {
        return false;
    }

    if (data != nullptr || size != 0)
    {
        file.write(data, size);
        return !file.bad();
    }
    else
    {
        return true;
    }
}

template <typename CharT>
bool read_file(
    const CharT *filename,
    std::string &data)
{
    std::ifstream file(filename, std::ios_base::binary | std::ios_base::in);
    if (!file.good())
        return false;
    data.assign(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>());
    return true;
}

template <typename CharT>
bool read_file(
    const CharT *filename,
    std::vector<char> &data)
{
    std::ifstream file(filename, std::ios_base::binary | std::ios_base::in);
    if (!file.good())
        return false;
    data.assign(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>());
    return true;
}

bool get_file_size(const char *filename, uint64_t &file_size)
{
    struct stat status;
    if (::stat(filename, &status) == -1 ||
        (status.st_mode & S_IFREG) == 0)
        return false;
    file_size = status.st_size;
    return true;
}

bool delete_file(const char *filename)
{
    return (::unlink(filename) != -1);
}

bool is_exists_file(const char *filename)
{
    struct stat status;
    return (
        ::stat(filename, &status) != -1 &&
        (status.st_mode & S_IFREG) != 0);
}

bool is_exists_directory(const char *path)
{
    struct stat status;
    return ::stat(path, &status) != -1 && (status.st_mode & S_IFDIR) != 0;
}

bool create_directory(const char *path)
{
    return ::mkdir(path, 0775) != -1;
}

bool delete_directory(const char *path)
{
    return ::rmdir(path) != -1;
}

bool create_directories(const char *path)
{
    std::string the_path(path);
    std::vector<std::string> need_create_directories;
    while (true)
    {
        if (is_exists_directory(the_path.c_str()))
            break;
        while (the_path.back() == '/')
            the_path.pop_back();
        need_create_directories.push_back(the_path);
        std::string::size_type pos = the_path.find_last_of('/');
        if (pos == std::string::npos)
            break;
        the_path = the_path.substr(0, the_path.find_last_of('/'));
    }

    if (need_create_directories.empty())
        return false;

    for (int i = need_create_directories.size(); i > 0; i--)
    {
        const std::string &directory = need_create_directories[i - 1];
        if (!create_directory(directory.c_str()))
        {
            return false;
        }
    }

    return true;
}

bool traverse_directory(
    const char *path,
    const std::function<bool(const std::string &entry_name,
                             bool is_directory)> &do_entry)
{
    if (path == nullptr || *path == 0)
        return false;
    // 删除多余的'/'
    std::string the_path(path);
    while (the_path.back() == '/')
        the_path.pop_back();

    DIR *directory = ::opendir(the_path.c_str());
    if (directory == nullptr)
        return false;
    std::shared_ptr<DIR> scope_close(directory, [](DIR *p) { ::closedir(p); });

    while (true)
    {
        // step 1. get children entry
        struct dirent *entry = ::readdir(directory);
        if (entry == nullptr)
            break;
        if (::strcmp(entry->d_name, ".") == 0 ||
            ::strcmp(entry->d_name, "..") == 0)
            continue;

        // step 2. children filename or path
        std::string child_path(the_path);
        child_path.push_back('/');
        child_path.append(entry->d_name);

        // step 3. do something for children
        struct stat status;
        int ret = ::stat(child_path.c_str(), &status);
        if (ret == -1)
            return false;

        // step 3.1. traverse sub directory
        bool is_directory = status.st_mode & S_IFDIR;
        if (is_directory)
        {
            if (!traverse_directory(child_path.c_str(), do_entry))
                return false;
        }

        // step 3.2 do_entry
        if (!do_entry(child_path, is_directory))
            return false;
    }

    return true;
}

bool delete_directories(const char *path)
{
    bool is_complete = traverse_directory(path,
                                          [](const std::string &filename, bool is_directory) -> bool {
                                              if (is_directory)
                                              {
                                                  return delete_directory(filename.c_str());
                                              }
                                              else
                                              {
                                                  return delete_file(filename.c_str());
                                              }
                                          });

    if (!is_complete)
        return false;
    return delete_directory(path);
}

} // namespace detail

bool write_file(const std::string &filename, const char *data, int size)
{
    return detail::write_file(filename.c_str(), data, size);
}

bool write_file(const char *filename, const char *data, int size)
{
    return detail::write_file(filename, data, size);
}

bool write_file(const std::string &filename, const std::string &data)
{
    return detail::write_file(filename.c_str(), data.data(), data.size());
}

bool write_file(const char *filename, const std::string &data)
{
    return detail::write_file(filename, data.data(), data.size());
}

bool write_file(const std::string &filename, const std::vector<char> &data)
{
    return detail::write_file(filename.c_str(), data.data(), data.size());
}

bool write_file(const char *filename, const std::vector<char> &data)
{
    return detail::write_file(filename, data.data(), data.size());
}

bool read_file(const std::string &filename, std::string &data)
{
    return detail::read_file(filename.c_str(), data);
}

bool read_file(const char *filename, std::string &data)
{
    return detail::read_file(filename, data);
}

bool read_file(const std::string &filename, std::vector<char> &data)
{
    return detail::read_file(filename.c_str(), data);
}

bool read_file(const char *filename, std::vector<char> &data)
{
    return detail::read_file(filename, data);
}

std::string read_file(const std::string &filename)
{
    std::string data;
    return detail::read_file(filename.c_str(), data) ? data : std::string();
}

std::string read_file(const char *filename)
{
    std::string data;
    return detail::read_file(filename, data) ? data : std::string();
}

bool get_file_size(const std::string &filename, uint64_t &file_size)
{
    return detail::get_file_size(filename.c_str(), file_size);
}

bool get_file_size(const char *filename, uint64_t &file_size)
{
    return detail::get_file_size(filename, file_size);
}

uint64_t get_file_size(const std::string &filename)
{
    uint64_t size;
    return detail::get_file_size(filename.c_str(), size) ? size : (uint64_t)-1;
}

uint64_t get_file_size(const char *filename)
{
    uint64_t size;
    return detail::get_file_size(filename, size) ? size : (uint64_t)-1;
}

bool delete_file(const std::string &filename)
{
    return detail::delete_file(filename.c_str());
}

bool delete_file(const char *filename)
{
    return detail::delete_file(filename);
}

bool is_exists_file(const std::string &filename)
{
    return detail::is_exists_file(filename.c_str());
}

bool is_exists_file(const char *filename)
{
    return detail::is_exists_file(filename);
}

bool is_exists_directory(const std::string &path)
{
    return detail::is_exists_directory(path.c_str());
}

bool is_exists_directory(const char *path)
{
    return detail::is_exists_directory(path);
}

bool create_directory(const std::string &path)
{
    return detail::create_directory(path.c_str());
}

bool create_directory(const char *path)
{
    return detail::create_directory(path);
}

bool delete_directory(const std::string &path)
{
    return detail::delete_directory(path.c_str());
}

bool delete_directory(const char *path)
{
    return detail::delete_directory(path);
}

bool create_directories(const std::string &path)
{
    return detail::create_directories(path.c_str());
}

bool create_directories(const char *path)
{
    return detail::create_directories(path);
}

bool delete_directories(const std::string &path)
{
    return detail::delete_directories(path.c_str());
}

bool delete_directories(const char *path)
{
    return detail::delete_directories(path);
}

bool traverse_directory(
    const std::string &path,
    const std::function<bool(const std::string &entry_name,
                             bool is_directory)> &do_entry)
{
    return detail::traverse_directory(path.c_str(), do_entry);
}

bool traverse_directory(
    const char *path,
    const std::function<bool(const std::string &entry_name,
                             bool is_directory)> &do_entry)
{
    return detail::traverse_directory(path, do_entry);
}

} // namespace ustd
