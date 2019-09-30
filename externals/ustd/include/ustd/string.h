#ifndef USTD_STRING_H_
#define USTD_STRING_H_

#include <string>
#include <vector>

namespace ustd
{
/// @brief 格式化字符串
/// @param format
/// @return 返回格式化后字符串
std::string format_string(const char *format, ...);
std::wstring format_string(const wchar_t *format, ...);

/// @brief 转化为二进制数据为十六进制可打印字符串
/// @param[in] data
/// @param[in] size
/// @return 十六进制可打印字符串
std::string hex_string(const void *data, int size);
template <typename CharT>
std::basic_string<CharT> hex_string(const void *data, int size);

/// @brief 转化为二进制数据为十六进制格式化打印字符串(带空格、换行)
/// @param[in] data
/// @param[in] size
/// @return 十六进制格式化打印字符串(带空格、换行)
std::string hex_display_string(const void *data, int size);
template <typename CharT>
std::basic_string<CharT> hex_display_string(const void *data, int size);

/// @brief 字符串替换
/// @param source
/// @param old_value
/// @param new_value
void replace_string(
    std::string &source,
    const std::string &old_value,
    const std::string &new_value);
void replace_string(
    std::wstring &source,
    const std::wstring &old_value,
    const std::wstring &new_value);

/// @brief 去除左边的空格
/// @param[in] source
/// @param[out] source
void trim_left_string(std::string &source);
void trim_left_string(std::wstring &source);

/// @brief 去除右边的空格
/// @param[in] source
/// @param[out] source
void trim_right_string(std::string &source);
void trim_right_string(std::wstring &source);

// 去除二端的空格
/// @brief 去除二端的空格
/// @param[in] source
/// @param[out] source
void trim_string(std::string &source);
void trim_string(std::wstring &source);

/// @brief 去除左边的字符
/// @param[in] source
/// @param[out] targets
void trim_left_string(std::string &source, const std::string &targets);
void trim_left_string(std::wstring &source, const std::wstring &targets);

/// @brief 去除右边的字符
/// @param[in] source
/// @param[out] targets
void trim_right_string(std::string &source, const std::string &targets);
void trim_right_string(std::wstring &source, const std::wstring &targets);

/// @brief 去除二端的字符
/// @param[in] source
/// @param[out] targets
void trim_string(std::string &source, const std::string &targets);
void trim_string(std::wstring &source, const std::wstring &targets);

/// @brief 字符串分割
/// @param[in] source 字符串
/// @param[in] delimiter 分隔符
/// @param[out] entries 字符串, entries的个数是source中的分割符个数加一
void split_string(
    const std::string &source,
    const std::string &delimiter,
    std::vector<std::string> &entries);
void split_string(
    const std::wstring &source,
    const std::wstring &delimiter,
    std::vector<std::wstring> &entries);

/// @brief 转化字符串为大写
/// @param[in] source
/// @return 转换后字符串
/// @note 当CharT为wchar_t时，采用unicode字符集
/// @note 当CharT为char时，采用本地字符集；假如是CP936字符集，不支持'狝', 'Ɛ'等字符
std::string to_upper_string(const std::string &source);
std::wstring to_upper_string(const std::wstring &source);

/// @brief 转换字符串为小写
/// @param[in] source
/// @return 转换后字符串
/// @note 当CharT为wchar_t时，采用unicode字符集
/// @note 当CharT为char时，采用本地字符集；假如是CP936字符集，不支持'狝', 'Ɛ'等字符
std::string to_lower_string(const std::string &source);
std::wstring to_lower_string(const std::wstring &source);

/// @brief 去掉文件扩展名
std::string trim_extension(const std::string &source);
} // namespace ustd

#include <ustd/inl/string-inl.h>

#endif // USTD_STRING_H_
