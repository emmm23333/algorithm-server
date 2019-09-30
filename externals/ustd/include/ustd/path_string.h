#ifndef USTD_PATH_STRING_H_
#define USTD_PATH_STRING_H_

#include <string>

namespace ustd
{
/// @brief 合并路径
/// @param[in] first_part
/// @param[in] last_part
/// @return 合并路径
std::string compose_path(const char *first, const char *last);
std::string compose_path(const std::string &first, const std::string &last);
std::wstring compose_path(const wchar_t *first, const wchar_t *last);
std::wstring compose_path(const std::wstring &first, const std::wstring &last);

/// @brief 切割文件路径，返回父路径
/// @param[in] path
/// @return
std::string get_parent_path(const char *path);
std::string get_parent_path(const std::string &path);
std::wstring get_parent_path(const wchar_t *path);
std::wstring get_parent_path(const std::wstring &path);

/// @brief 不包含路径的短文件名，或者最后一级目录
/// @param[in] path
/// @return
std::string get_last_path(const char *path);
std::string get_last_path(const std::string &path);
std::wstring get_last_path(const wchar_t *path);
std::wstring get_last_path(const std::wstring &path);
} // namespace ustd

#endif // USTD_PATH_STRING_H_
