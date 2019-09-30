#ifndef JSON_DATA_API_
#define JSON_DATA_API_
#include <string>
namespace json_data
{
bool init_json_data(const std::string &json_dir);
void uninit_json_data();
} // namespace json_data
#endif