#ifdef USTD_STRING_H_

namespace ustd
{
template <>
std::string hex_string<char>(const void *data, int size);
template <>
std::wstring hex_string<wchar_t>(const void *data, int size);
template <>
std::string hex_display_string<char>(const void *data, int size);
template <>
std::wstring hex_display_string<wchar_t>(const void *data, int size);
} // namespace ustd

#endif // USTD_STRING_H_
