#include <ustd/log.h>
#include "json_data_api.h"
#include "file_io.h"
#include "common.h"
#include <unistd.h>

namespace json_data
{
static std::string my_host_info_json = "my_host_info.json";

template <typename DataType>
bool load_json(DataType &data, const std::string &file_name)
{
    std::string content, err;
    if (!read_json(file_name, content))
    {
        log_print(kError, "read %s file fail.", file_name.c_str());
        return false; //文件不存在或者空文件
    }
    if (!data.deserialize(content, err))
    {
        log_print(kError, "json deserialize fail; err info:%s ", err.c_str());
        return false; //格式错误
    }
    return true;
}
template <typename DataType>
bool store_json(DataType &data, const std::string &file_name)
{
    std::string content, err;
    if (!data.serialize(content, err))
    {
        log_print(kError, "json serialize fail; err info:%s ", err.c_str());
        return false;
    }
    if (!write_json(file_name, content))
    {
        log_print(kError, "write %s file fail.", file_name.c_str());
        return false;
    }
    return true;
}

bool init_json_data(const std::string &json_dir)
{
    return set_json_dir(json_dir);
}
void uninit_json_data() {}

} // namespace json_data