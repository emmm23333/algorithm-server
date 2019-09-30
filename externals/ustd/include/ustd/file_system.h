#ifndef USTD_FILE_SYSTEM_H_
#define USTD_FILE_SYSTEM_H_

#include <stdint.h>
#include <functional>
#include <string>
#include <vector>

namespace ustd
{
/// @brief 写文件
/// @param[in] filename 文件名
/// @param[in] data 文件数据
/// @param[in] size 数据长度
/// @return 写成功返回true，失败返回false
/// @note 清空原有数据，以std::ios_base::binary写入
bool write_file(const std::string &filename, const char *data, int size);
bool write_file(const char *filename, const char *data, int size);

/// @brief 写文件, std::ios_base::binary
/// @param[in] filename 文件名
/// @param[in] data 文件数据
/// @return 写成功返回true，失败返回false
/// @note std::ios_base::binary
bool write_file(const std::string &filename, const std::string &data);
bool write_file(const char *filename, const std::string &data);

/// @brief 写文件, std::ios_base::binary
/// @param[in] filename 文件名
/// @param[in] data 文件数据
/// @return 写成功返回true，失败返回false
/// @note std::ios_base::binary
bool write_file(const std::string &filename, const std::vector<char> &data);
bool write_file(const char *filename, const std::vector<char> &data);

/// @brief 读整个文件, std::ios_base::binary
/// @param[in] filename 文件名
/// @param[out] data 文件数据(二进制)
/// @return 读成功返回true，失败返回false
bool read_file(const std::string &filename, std::string &data);
bool read_file(const char *filename, std::string &data);

/// @brief 读整个文件, std::ios_base::binary
/// @param[in] filename 文件名
/// @param[out] data 文件数据
/// @return 读成功返回true，失败返回false
bool read_file(const std::string &filename, std::vector<char> &data);
bool read_file(const char *filename, std::vector<char> &data);

/// @brief 读整个文件, std::ios_base::binary
/// @param[in] filename 文件名
/// @return 返回文件数据(二进制)，文件不存在或为空时返回empty
std::string read_file(const std::string &filename);
std::string read_file(const char *filename);

/// @brief 读取文件长度
/// @param[in] filename 文件名
/// @param[out] file_size 文件长度
/// @return 读成功返回true，失败返回false
bool get_file_size(const std::string &filename, uint64_t &file_size);
bool get_file_size(const char *filename, uint64_t &file_size);

/// @brief 读取文件长度
/// @param[in] filename 文件名
/// @return 读成功返回文件长度，失败返回(uint64_t)-1
uint64_t get_file_size(const std::string &filename);
uint64_t get_file_size(const char *filename);

/// @brief 删除文件
/// @param[in] filename 文件名
/// @return 删除成功返回true，失败返回false
bool delete_file(const std::string &filename);
bool delete_file(const char *filename);

/// @brief 文件当前是否存在
/// @param[in] filename 文件名
/// @return 存在返回true，不存在返回false
bool is_exists_file(const std::string &filename);
bool is_exists_file(const char *filename);

/// @brief 目录当前是否存在
/// @param[in] path 目录
/// @return 存在返回true，不存在返回false
bool is_exists_directory(const std::string &path);
bool is_exists_directory(const char *path);

/// @brief 创建目录
/// @param[in] path 目录
/// @return 创建成功返回true，失败返回false
bool create_directory(const std::string &path);
bool create_directory(const char *path);

/// @brief 删除目录
/// @param[in] path 目录
/// @return 删除成功返回true，失败返回false
bool delete_directory(const std::string &path);
bool delete_directory(const char *path);

/// @brief 创建目录，支持多级目录
/// @param[in] path 目录
/// @return 创建成功返回true，失败返回false
bool create_directories(const std::string &path);
bool create_directories(const char *path);

/// @brief 删除目录，包含子目录与文件
/// @param[in] path 目录
/// @return 删除成功返回true，失败返回false
bool delete_directories(const std::string &path);
bool delete_directories(const char *path);

/// @brief 遍历目录，包含子目录与文件
/// @param[in] path 目录
/// @return 成功遍历完成返回true
/// @note 如果do_entry返回false，则遍历结束，返回false
/// @note 如果目录中存在既不是regular_file也不是regular_directory，返回false
bool traverse_directory(
    const std::string &path,
    const std::function<bool(const std::string &entry_name,
                             bool is_directory)> &do_entry);
bool traverse_directory(
    const char *path,
    const std::function<bool(const std::string &entry_name,
                             bool is_directory)> &do_entry);
} // namespace ustd

#endif // USTD_FILE_SYSTEM_H_
