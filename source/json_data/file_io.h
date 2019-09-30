#ifndef FILE_IO_H_
#define FILE_IO_H_
#include <string>
namespace json_data
{
bool set_json_dir(const std::string &dir);
bool read_json(const std::string &file_name, std::string &data);
bool write_json(const std::string &file_name, const std::string &data);
} // namespace json_data

#endif