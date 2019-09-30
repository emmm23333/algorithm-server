#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <ustd/file_system.h>
#include <ustd/path_string.h>
#include <ustd/log.h>
#include "file_io.h"
#include "lock.h"
#include <unistd.h>
namespace json_data
{
static std::string json_dir = "";
bool set_json_dir(const std::string &dir)
{
    json_dir = dir;
    return ustd::create_directories(json_dir);
}
bool read_json(const std::string &file_name, std::string &data)
{
    // std::string dir = ustd::compose_path(json_dir, file_name);
    // FILE *pf = fopen(dir.c_str(), "r");
    FILE *pf = fopen(file_name.c_str(), "r");
    if (pf == NULL)
    {
        printf("open %s fail.\n", file_name.c_str());
        return false;
    }
    int fd = fileno(pf);
    read_lock_wait(fd);
    data.clear();
    fseek(pf, 0L, SEEK_END);
    int flen = ftell(pf);
    data.resize(flen);
    fseek(pf, 0L, SEEK_SET);
    fread(const_cast<char *>(data.c_str()), data.size(), 1, pf);
    unlock(fd);
    fclose(pf);
    return true;
}
bool write_json(const std::string &file_name, const std::string &data)
{
    // std::string swp_name = ustd::compose_path(json_dir, "." + file_name + ".swp");
    // std::string path_name = ustd::compose_path(json_dir, file_name);
    std::string swp_name = file_name + ".swp";
    std::string path_name = file_name;
    FILE *pf = fopen(swp_name.c_str(), "w");
    if (pf == NULL)
    {
        printf("open %s fail.", swp_name.c_str());
        return false;
    }
    int fd = fileno(pf);
    write_lock_wait(fd);
    fwrite(data.c_str(), data.size(), 1, pf);
    unlock(fd);
    fclose(pf);
    return (rename(swp_name.c_str(), path_name.c_str()) == 0);
}
} // namespace json_data