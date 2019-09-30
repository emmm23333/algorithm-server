#include <ustd/path_string.h>
#include <assert.h>

namespace ustd
{
namespace detail
{
template <typename CharT>
CharT system_slash();

#ifdef WIN32
template <>
inline char system_slash<char>()
{
    return '\\';
}
template <>
inline wchar_t system_slash<wchar_t>() { return L'\\'; }
#else
template <>
inline char system_slash<char>()
{
    return '/';
}
template <>
inline wchar_t system_slash<wchar_t>() { return L'/'; }
#endif

template <typename CharT>
CharT reverse_system_slash();

#ifdef WIN32
template <>
inline char reverse_system_slash<char>()
{
    return '/';
}
template <>
inline wchar_t reverse_system_slash<wchar_t>() { return L'/'; }
#else
template <>
inline char reverse_system_slash<char>()
{
    return '\\';
}
template <>
inline wchar_t reverse_system_slash<wchar_t>() { return L'\\'; }
#endif

template <typename CharT>
bool is_system_slash(CharT slash)
{
#ifdef WIN32
    return slash == system_slash<CharT>() ||
           slash == reverse_system_slash<CharT>();
#else
    return slash == system_slash<CharT>();
#endif
}

template <typename CharT>
typename std::basic_string<CharT>::size_type get_last_slash_pos(
    const std::basic_string<CharT> &path)
{
    std::basic_string<CharT> the_path(path);
#ifdef WIN32
    std::basic_string<CharT>::size_type pos =
        the_path.rfind(system_slash<CharT>());
    std::basic_string<CharT>::size_type reverse_pos =
        the_path.rfind(reverse_system_slash<CharT>());
    if (pos != std::basic_string<CharT>::npos &&
        reverse_pos != std::basic_string<CharT>::npos)
    {
        return pos > reverse_pos ? pos : reverse_pos;
    }
    else if (pos != std::basic_string<CharT>::npos &&
             reverse_pos == std::basic_string<CharT>::npos)
    {
        return pos;
    }
    else if (pos == std::basic_string<CharT>::npos &&
             reverse_pos != std::basic_string<CharT>::npos)
    {
        return reverse_pos;
    }
    else
    {
        return std::basic_string<CharT>::npos;
    }
#else
    return the_path.rfind(system_slash<CharT>());
#endif
}

template <typename CharT>
std::basic_string<CharT> trim_slash(const CharT *path)
{
    std::basic_string<CharT> better_path;
    if (path != nullptr)
        better_path.assign(path);

    while (!better_path.empty() && is_system_slash<CharT>(better_path.back()))
    {
        better_path.pop_back();
    }
    return better_path;
}

template <typename CharT>
std::basic_string<CharT> compose_path(
    const CharT *first,
    const CharT *last)
{
    std::basic_string<CharT> path;
    if (first != nullptr)
        path.assign(first);

    // slash('\\' or '/')个数
    int slash_count = 0;
    if (!path.empty() && is_system_slash<CharT>(path.back()))
        slash_count++;
    if (last != nullptr && is_system_slash<CharT>(*last))
        slash_count++;

    // 修正slash('\\' or '/')
    if (slash_count == 0)
        path.append(1, system_slash<CharT>());
    else if (slash_count == 2)
        last += 1;

    // 合并路径
    if (last != nullptr)
        path.append(last);

    return path;
}

template <typename CharT>
std::basic_string<CharT> get_parent_path(
    const CharT *path)
{
    std::basic_string<CharT> the_path = trim_slash(path);
    size_t pos = get_last_slash_pos(the_path);
    if (pos != std::basic_string<CharT>::npos)
    {
        return the_path.substr(0, pos);
    }
    else
    {
        return std::basic_string<CharT>();
    }
}

template <typename CharT>
std::basic_string<CharT> get_last_path(
    const CharT *path)
{
    std::basic_string<CharT> the_path = trim_slash(path);
    size_t pos = get_last_slash_pos(the_path);
    if (pos != std::basic_string<CharT>::npos)
    {
        return the_path.substr(pos + 1);
    }
    else
    {
        return std::basic_string<CharT>(path);
    }
}
} // namespace detail

std::string compose_path(const char *first, const char *last)
{
    return detail::compose_path(first, last);
}

std::string compose_path(const std::string &first, const std::string &last)
{
    return detail::compose_path(first.c_str(), last.c_str());
}

std::wstring compose_path(const wchar_t *first, const wchar_t *last)
{
    return detail::compose_path(first, last);
}

std::wstring compose_path(const std::wstring &first, const std::wstring &last)
{
    return detail::compose_path(first.c_str(), last.c_str());
}

std::string get_parent_path(const char *path)
{
    return detail::get_parent_path(path);
}

std::string get_parent_path(const std::string &path)
{
    return detail::get_parent_path(path.c_str());
}

std::wstring get_parent_path(const wchar_t *path)
{
    return detail::get_parent_path(path);
}

std::wstring get_parent_path(const std::wstring &path)
{
    return detail::get_parent_path(path.c_str());
}

std::string get_last_path(const char *path)
{
    return detail::get_last_path(path);
}

std::string get_last_path(const std::string &path)
{
    return detail::get_last_path(path.c_str());
}

std::wstring get_last_path(const wchar_t *path)
{
    return detail::get_last_path(path);
}

std::wstring get_last_path(const std::wstring &path)
{
    return detail::get_last_path(path.c_str());
}

} // namespace ustd
