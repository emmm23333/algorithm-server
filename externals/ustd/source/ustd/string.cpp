#include <ustd/string.h>
#include <locale>
#include <algorithm>
#include <stdarg.h>

namespace ustd
{
namespace detail
{
int vscprintf(const char *format, va_list ap)
{
#ifdef WIN32
    return _vscprintf(format, ap);
#else
    va_list ap_copy;
    va_copy(ap_copy, ap);
    int size = vsnprintf(nullptr, 0, format, ap_copy);
    va_end(ap_copy);
    return size;
#endif
}

int vscprintf(const wchar_t *format, va_list ap)
{
#ifdef WIN32
    return _vscwprintf(format, ap);
#else
    va_list ap_copy;
    va_copy(ap_copy, ap);
    int size = vswprintf(nullptr, 0, format, ap_copy);
    va_end(ap_copy);
    return size;
#endif
}

int vsnprintf(char *buffer, size_t size, const char *format, va_list ap)
{
#ifdef WIN32
    return ::vsprintf_s(buffer, size, format, ap);
#else
    return ::vsnprintf(buffer, size, format, ap);
#endif
}

int vsnprintf(wchar_t *buffer, size_t size, const wchar_t *format, va_list ap)
{
#ifdef WIN32
    return ::vswprintf_s(buffer, size, format, ap);
#else
    return ::vswprintf(buffer, size, format, ap);
#endif
}

template <typename CharT>
std::basic_string<CharT> format_string_v(const CharT *format, va_list ap)
{
    int size = detail::vscprintf(format, ap);
    std::vector<CharT> buffer(size + 1);
    detail::vsnprintf(buffer.data(), buffer.size(), format, ap);
    return std::basic_string<CharT>(buffer.data(), size);
}

#define HEX_HALF_CHAR(x) ((x) < 0xa ? (x) + '0' : (x) + 'a' - 0xa)
template <typename CharT>
std::basic_string<CharT> hex_string(const void *data, int size)
{
    std::basic_string<CharT> result;
    int pos = 0;
    CharT buffer[1024]; // 长度必须是偶数
    for (int i = 0; i < size; i++)
    {
        unsigned char input_char = *((unsigned char *)data + i);
        unsigned char left_part = (input_char & 0xf0) >> 4;
        unsigned char right_part = input_char & 0x0f;
        buffer[pos++] = HEX_HALF_CHAR(left_part);
        buffer[pos++] = HEX_HALF_CHAR(right_part);
        if ((i + 1) == size ||
            pos == std::extent<decltype(buffer)>::value)
        {
            result.append(buffer, pos);
            pos = 0;
        }
    }

    return result;
}

template <typename CharT>
std::basic_string<CharT> hex_display_string(const void *data, int size)
{
    std::basic_string<CharT> result;
    int pos = 0;
    CharT buffer[1024]; // 长度必须是偶数
    for (int i = 0; i < size; i++)
    {
        unsigned char input_char = *((unsigned char *)data + i);
        unsigned char left_part = (input_char & 0xf0) >> 4;
        unsigned char right_part = input_char & 0x0f;
        buffer[pos++] = HEX_HALF_CHAR(left_part);
        buffer[pos++] = HEX_HALF_CHAR(right_part);
        (i + 1) == size ? buffer[pos++] = '\n' : (i + 1) % 32 == 0 ? buffer[pos++] = '\n' : (i + 1) % 16 == 0 ? (buffer[pos++] = ' ', buffer[pos++] = ' ') : (i + 1) % 8 == 0 ? buffer[pos++] = ' ' : 0;
        if ((i + 1) == size ||
            pos > (int)std::extent<decltype(buffer)>::value - 8)
        {
            result.append(buffer, pos);
            pos = 0;
        }
    }

    return result;
}

template <typename CharT>
void replace_string(
    std::basic_string<CharT> &source,
    const std::basic_string<CharT> &old_value,
    const std::basic_string<CharT> &new_value)
{
    for (
        size_t pos = source.find(old_value);
        pos != std::basic_string<CharT>::npos;
        pos = source.find(old_value, pos))
    {
        source.replace(pos, old_value.size(), new_value);
        pos += new_value.size();
    }
}

template <typename CharT>
void trim_left_string(std::basic_string<CharT> &source)
{
    size_t pos = source.find_first_not_of(' ');
    if (pos != std::basic_string<CharT>::npos)
    {
        source.erase(0, pos);
    }
    else
    {
        source.clear();
    }
}

template <typename CharT>
void trim_right_string(std::basic_string<CharT> &source)
{
    size_t pos = source.find_last_not_of(' ');
    if (pos != std::basic_string<CharT>::npos)
    {
        source.erase(pos + 1);
    }
    else
    {
        source.clear();
    }
}

template <typename CharT>
void trim_string(std::basic_string<CharT> &source)
{
    trim_right_string(source);
    trim_left_string(source);
}

template <typename CharT>
void trim_left_string(
    std::basic_string<CharT> &source,
    const std::basic_string<CharT> &targets)
{
    auto iter = std::find_if_not(source.begin(), source.end(),
                                 [&targets](CharT ch) {
                                     return targets.find(ch) != std::basic_string<CharT>::npos;
                                 });

    source.erase(source.begin(), iter);
}

template <typename CharT>
void trim_right_string(
    std::basic_string<CharT> &source,
    const std::basic_string<CharT> &targets)
{
    auto iter = std::find_if_not(source.rbegin(), source.rend(),
                                 [&targets](CharT ch) {
                                     return targets.find(ch) != std::basic_string<CharT>::npos;
                                 });

    source.erase(iter.base(), source.end());
}

template <typename CharT>
void trim_string(
    std::basic_string<CharT> &source,
    const std::basic_string<CharT> &targets)
{
    trim_right_string(source, targets);
    trim_left_string(source, targets);
}

template <typename CharT>
void split_string(
    const std::basic_string<CharT> &source,
    const std::basic_string<CharT> &delimiter,
    std::vector<std::basic_string<CharT>> &entries)
{
    for (size_t pos = 0;;)
    {
        size_t previous_pos = pos;
        pos = source.find(delimiter, pos);
        if (pos != std::basic_string<CharT>::npos)
        {
            entries.emplace_back(source, previous_pos, pos - previous_pos);
            pos += delimiter.size();

            // 分割符如果在结尾，需要增加一个元素
            if (pos == source.size())
            {
                entries.push_back(std::basic_string<CharT>());
                break;
            }
        }
        else
        {
            entries.push_back(source.substr(previous_pos));
            break;
        }
    }
}

template <typename CharT>
std::basic_string<CharT> to_upper_string(const std::basic_string<CharT> &source)
{
    if (source.empty())
        return std::basic_string<CharT>();

    std::locale system_locale(""); // 这是本地语言
    const std::ctype<CharT> &facet =
        std::use_facet<std::ctype<CharT>>(system_locale);
    std::vector<CharT> the_upper(source.begin(), source.end());
    facet.toupper(the_upper.data(), the_upper.data() + the_upper.size());

    return std::basic_string<CharT>(the_upper.begin(), the_upper.end());
}

template <typename CharT>
std::basic_string<CharT> to_lower_string(const std::basic_string<CharT> &source)
{
    if (source.empty())
        return std::basic_string<CharT>();

    std::locale system_locale(""); // 这是本地语言
    const std::ctype<CharT> &facet =
        std::use_facet<std::ctype<CharT>>(system_locale);
    std::vector<CharT> the_lower(source.begin(), source.end());
    facet.tolower(the_lower.data(), the_lower.data() + the_lower.size());

    return std::basic_string<CharT>(the_lower.begin(), the_lower.end());
}
} // namespace detail

std::string format_string(const char *format, ...)
{
    va_list vars;
    va_start(vars, format);
    std::string result = detail::format_string_v(format, vars);
    va_end(vars);
    return result;
}

std::wstring format_string(const wchar_t *format, ...)
{
    va_list vars;
    va_start(vars, format);
    std::wstring result = detail::format_string_v(format, vars);
    va_end(vars);
    return result;
}

template <>
std::string hex_string<char>(const void *data, int size)
{
    return detail::hex_string<char>(data, size);
}

template <>
std::wstring hex_string<wchar_t>(const void *data, int size)
{
    return detail::hex_string<wchar_t>(data, size);
}

template <>
std::string hex_display_string<char>(const void *data, int size)
{
    return detail::hex_display_string<char>(data, size);
}

template <>
std::wstring hex_display_string<wchar_t>(const void *data, int size)
{
    return detail::hex_display_string<wchar_t>(data, size);
}

void replace_string(
    std::string &source,
    const std::string &old_value,
    const std::string &new_value)
{
    detail::replace_string(source, old_value, new_value);
}

void replace_string(
    std::wstring &source,
    const std::wstring &old_value,
    const std::wstring &new_value)
{
    detail::replace_string(source, old_value, new_value);
}

void trim_left_string(std::string &source)
{
    detail::trim_left_string(source);
}

void trim_left_string(std::wstring &source)
{
    detail::trim_left_string(source);
}

void trim_right_string(std::string &source)
{
    detail::trim_right_string(source);
}

void trim_right_string(std::wstring &source)
{
    detail::trim_right_string(source);
}

void trim_string(std::string &source)
{
    detail::trim_string(source);
}

void trim_string(std::wstring &source)
{
    detail::trim_string(source);
}

void trim_left_string(std::string &source, const std::string &targets)
{
    detail::trim_left_string(source, targets);
}

void trim_left_string(std::wstring &source, const std::wstring &targets)
{
    detail::trim_left_string(source, targets);
}

void trim_right_string(std::string &source, const std::string &targets)
{
    detail::trim_right_string(source, targets);
}

void trim_right_string(std::wstring &source, const std::wstring &targets)
{
    detail::trim_right_string(source, targets);
}

void trim_string(std::string &source, const std::string &targets)
{
    detail::trim_string(source, targets);
}

void trim_string(std::wstring &source, const std::wstring &targets)
{
    detail::trim_string(source, targets);
}

void split_string(
    const std::string &source,
    const std::string &delimiter,
    std::vector<std::string> &entries)
{
    detail::split_string(source, delimiter, entries);
}

void split_string(
    const std::wstring &source,
    const std::wstring &delimiter,
    std::vector<std::wstring> &entries)
{
    detail::split_string(source, delimiter, entries);
}

std::string to_upper_string(const std::string &source)
{
    return detail::to_upper_string(source);
}

std::wstring to_upper_string(const std::wstring &source)
{
    return detail::to_upper_string(source);
}

std::string to_lower_string(const std::string &source)
{
    return detail::to_lower_string(source);
}

std::wstring to_lower_string(const std::wstring &source)
{
    return detail::to_lower_string(source);
}

std::string trim_extension(const std::string &source)
{
    std::string ret(source);
    auto iter = ret.find_last_of('.');
    if (iter == std::string::npos)
    {
        return ret;
    }
    return ret.substr(0, iter);
}
} // namespace ustd
