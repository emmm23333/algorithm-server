#include <ustd/log.h>
#include <mutex>
#include <vector>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string.h>
#ifdef WIN32
#include <filesystem>
#include <Windows.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/syscall.h>
#endif

namespace ustd
{
namespace log
{
namespace detail
{
template <typename CharT>
const CharT *select_char(const char *, const wchar_t *);

template <>
const char *select_char<char>(const char *sz, const wchar_t *)
{
    return sz;
}

template <>
const wchar_t *select_char<wchar_t>(const char *, const wchar_t *sz)
{
    return sz;
}

#define CHAR_T(type, sz) select_char<type>(sz, L##sz)
#define kHeaderTimestamp(type) CHAR_T(type, "[%02u:%02u:%02u.%03u]")
#define kHeaderThreadId(type) CHAR_T(type, "[%04x]")
#define kHeaderFileAndLine(type) CHAR_T(type, "[%s:%d]")
#define kHeaderFunction(type) CHAR_T(type, "[%s]")
#define kHeaderLogLevel(type) CHAR_T(type, "[%s]")
#define kLogFilenameSuffix(type) CHAR_T(type, "%04u%02u%02u.log")
#define kLogFilenameSuffixSize 16

template <typename CharT>
CharT get_system_slash()
{
#ifdef WIN32
    return '\\';
#else
    return '/';
#endif
}

template <typename CharT>
const CharT *get_level_string(unsigned int level)
{
    switch (level)
    {
    case kStartup:
        return CHAR_T(CharT, "Startup");
    case kShutdown:
        return CHAR_T(CharT, "Shutdown");
    case kInfo:
        return CHAR_T(CharT, "Info");
    case kWarning:
        return CHAR_T(CharT, "Warning");
    case kTrace:
        return CHAR_T(CharT, "Trace");
    case kError:
        return CHAR_T(CharT, "Error");
    case kDebug:
        return CHAR_T(CharT, "Debug");
    default:
        return CHAR_T(CharT, "Unknown");
    }
}

struct SystemTime
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int millisecond;
};

tm to_tm(time_t t)
{
    struct tm _tm;
#ifdef WIN32
    localtime_s(&_tm, &t);
#else
    localtime_r(&t, &_tm);
#endif
    return _tm;
}

SystemTime to_system_time(time_t t)
{
    tm _tm = to_tm(t);
    return SystemTime{
        _tm.tm_year + 1900,
        _tm.tm_mon + 1,
        _tm.tm_mday,
        _tm.tm_hour,
        _tm.tm_min,
        _tm.tm_sec,
        0};
}

SystemTime get_system_time()
{
    std::chrono::system_clock::time_point now =
        std::chrono::system_clock::now();
    time_t t = std::chrono::system_clock::to_time_t(now);
    int milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
                           now.time_since_epoch())
                           .count() %
                       1000;
    SystemTime system_time = to_system_time(t);
    system_time.millisecond = milliseconds;
    return system_time;
}

#ifdef WIN32
namespace fs
{
using namespace std::tr2::sys;
path make_path(const char *_path)
{
    return path(_path);
}

wpath make_path(const wchar_t *_path)
{
    return wpath(_path);
}
} // namespace fs
#endif // #ifdef WIN32

#ifdef WIN32
template <typename CharT>
bool create_directory(const CharT *path)
{
    return fs::create_directory(fs::make_path(path));
}
#else
bool create_directory(const char *path)
{
    return ::mkdir(path, 0775) != -1;
}
#endif // #ifdef WIN32

uint32_t get_thread_id()
{
#ifdef WIN32
    return ::GetCurrentThreadId();
#else
    return ::syscall(SYS_gettid);
#endif
}

int vsnprintf(char *buffer, size_t size, const char *format, va_list ap)
{
#ifdef WIN32
    int pos = ::vsnprintf_s(buffer, size, _TRUNCATE, format, ap);
    return pos == -1 ? ::_vscprintf(format, ap) : pos;
#else
    return ::vsnprintf(buffer, size, format, ap);
#endif
}

int vsnprintf(wchar_t *buffer, size_t size, const wchar_t *format, va_list ap)
{
#ifdef WIN32
    int pos = ::_vsnwprintf_s(buffer, size, _TRUNCATE, format, ap);
    return pos == -1 ? ::_vscwprintf(format, ap) : pos;
#else
    return ::vswprintf(buffer, size, format, ap);
#endif
}

size_t strlen(const char *sz)
{
    return ::strlen(sz);
}

size_t strlen(const wchar_t *sz)
{
    return ::wcslen(sz);
}

int strcmp(const char *sz1, const char *sz2)
{
    return ::strcmp(sz1, sz2);
}

int strcmp(const wchar_t *sz1, const wchar_t *sz2)
{
    return ::wcscmp(sz1, sz2);
}

#ifdef WIN32
void strcpy(char *dest, size_t dest_size, const char *src)
{
    ::strcpy_s(dest, dest_size, src);
}

void strcpy(wchar_t *dest, size_t dest_size, const wchar_t *src)
{
    ::wcscpy_s(dest, dest_size, src);
}
#else
void strcpy(char *dest, size_t dest_size, const char *src)
{
    ::strncpy(dest, src, dest_size);
}
#endif

template <typename CharT>
int snprintf(CharT *buffer, size_t size, const CharT *format, ...)
{
    va_list ap;
    va_start(ap, format);
    int pos = vsnprintf(buffer, size, format, ap);
    va_end(ap);
    return pos;
}

template <typename CharT>
int append_splitter(CharT *buffer, int pos)
{
    buffer[pos++] = '|';
    return 1;
}

template <typename CharT>
int append_line_end(CharT *buffer, int pos)
{
#ifdef WIN32
    buffer[pos++] = '\r';
    buffer[pos++] = '\n';
    return 2;
#else
    buffer[pos++] = '\n';
    return 1;
#endif
}

template <typename CharT>
void open_file(std::ofstream &file, const CharT &path)
{
#if defined(WIN32) && defined(UNICODE)
    bool is_exists = std::ifstream(path).good();
#endif
    file.open(path, std::ios::binary | std::ios::app);
#if defined(WIN32) && defined(UNICODE)
    // 如果是UNICODE文件，加文件头0xFFFE
    if (!is_exists && file.good())
    {
        file.put(-1);
        file.put(-2);
    }
#endif
}

void print_to_screen(const wchar_t *message)
{
    std::wcout << message;
}

void print_to_screen(const char *message)
{
    std::cout << message;
}

#ifdef WIN32
bool delete_file(const char *filename)
{
    return FALSE != ::DeleteFileA(filename);
}

bool delete_file(const wchar_t *filename)
{
    return FALSE != ::DeleteFileW(filename);
}
#else
bool delete_file(const char *filename)
{
    return (::unlink(filename) != -1);
}
#endif

template <typename CharT>
std::basic_string<CharT> hex_display_string(const void *data, int size)
{
#define HEX_HALF_CHAR(x) ((x) < 0xa ? (x) + '0' : (x) + 'a' - 0xa)
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
        (i + 1) == size ? pos += append_line_end(buffer, pos) : (i + 1) % 32 == 0 ? pos += append_line_end(buffer, pos) : (i + 1) % 16 == 0 ? (buffer[pos++] = ' ', buffer[pos++] = ' ') : (i + 1) % 8 == 0 ? buffer[pos++] = ' ' : 0;
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
const CharT *get_last_path(const CharT *path, int slash_count)
{
    const CharT *p = path + strlen(path) - 1;
    while (p-- != path)
    {
        if (*p == get_system_slash<CharT>())
            slash_count--;
        if (slash_count < 0)
            return ++p;
    }
    return path;
}

template <typename CharT>
int sprintf_timestamp(
    CharT *buffer,
    int size,
    const SystemTime &t)
{
    return snprintf(
        buffer,
        size,
        kHeaderTimestamp(CharT),
        t.hour,
        t.minute,
        t.second,
        t.millisecond);
}

template <typename CharT>
int sprintf_thread_id(
    CharT *buffer,
    int size,
    unsigned int thread_id)
{
    return snprintf(
        buffer,
        size,
        kHeaderThreadId(CharT),
        thread_id);
}

template <typename CharT>
int sprintf_file_and_line(
    CharT *buffer,
    int size,
    const CharT *filename,
    unsigned int line)
{
    return snprintf(
        buffer, size,
        kHeaderFileAndLine(CharT),
        get_last_path(filename, 1), // 第二个参数表示保留几级目录
        line);
}

template <typename CharT>
int sprintf_function(
    CharT *buffer,
    int size,
    const CharT *function)
{
    return snprintf(
        buffer,
        size,
        kHeaderFunction(CharT),
        function);
}

template <typename CharT>
int sprintf_level(
    CharT *buffer,
    int size,
    unsigned int level)
{
    return snprintf(
        buffer,
        size,
        kHeaderLogLevel(CharT),
        get_level_string<CharT>(level));
}

template <typename CharT>
int sprintf_filename_suffix(
    CharT *buffer,
    int size,
    const SystemTime &system_time)
{
    return snprintf(
        buffer,
        size,
        kLogFilenameSuffix(CharT),
        system_time.year,
        system_time.month,
        system_time.day);
}

template <typename CharT>
int format_line_head(
    unsigned int field_type,
    CharT *buffer,
    int size,
    unsigned int level,
    const CharT *filename,
    unsigned int line,
    const CharT *function,
    const SystemTime &system_time)
{
    int pos = 0;
    if (field_type & kTimestamp)
    {
        pos += sprintf_timestamp(buffer + pos, size - pos, system_time);
    }
    if (pos >= size)
        return size;

    if (field_type & kThreadId)
    {
        pos += sprintf_thread_id(buffer + pos, size - pos, get_thread_id());
    }
    if (pos >= size)
        return size;

    pos += sprintf_level(buffer + pos, size - pos, level);
    if (pos >= size)
        return size;

    if (field_type & kFileLine)
    {
        pos += sprintf_file_and_line(buffer + pos, size - pos, filename, line);
    }
    if (pos >= size)
        return size;

    if (field_type & kFunction)
    {
        pos += sprintf_function(buffer + pos, size - pos, function);
    }
    if (pos >= size)
        return size;

    pos += append_splitter(buffer, pos);
    return pos;
}

template <typename CharT>
class Log
{
private:
    Log()
    {
#if defined(WIN32) && defined(UNICODE)
        static std::locale locale("");
        std::wcout.imbue(locale);
#endif
    }

    ~Log()
    {
    }

public:
    static Log &instance()
    {
        static Log instance;
        return instance;
    }

public:
    void set_directory(const CharT *path)
    {
        log_directory_.assign(path);
        if (!log_directory_.empty() &&
            log_directory_.back() != get_system_slash<CharT>())
        {
            log_directory_.push_back(get_system_slash<CharT>());
        }
        create_directory(log_directory_.c_str());
    }

    void set_prefix_name(const CharT *prefix_name)
    {
        prefix_name_ = prefix_name;
    }

    void set_level(unsigned int level)
    {
        level_ = level;
    }

    void set_storage_type(unsigned int storage_type)
    {
        storage_type_ = storage_type;
    }

    void set_field_type(unsigned int field_type)
    {
        field_type_ = field_type;
    }

    void set_reserve_days(int days)
    {
        reserve_days_ = days;
    }

    void print_v(
        unsigned int level,
        const CharT *filename,
        unsigned int line,
        const CharT *function,
        const CharT *format,
        va_list ap)
    {
        if ((level_ & level) == 0)
            return;

        // 日志时间
        SystemTime system_time = get_system_time();

        // 初始化行头
        const int kMaxBufferSize = 4096;
        CharT buffer[kMaxBufferSize + 16];
        int pos = format_line_head(
            field_type_,
            buffer,
            kMaxBufferSize,
            level,
            filename,
            line,
            function,
            system_time);
        // TODO: 如果头太长...

        // 格式化打印数据
        int body_size = vsnprintf(buffer + pos, kMaxBufferSize - pos,
                                  format, ap);

        // 如果行太长
        if (body_size > kMaxBufferSize)
        {
            pos += snprintf(buffer + pos, kMaxBufferSize - pos,
                            CHAR_T(CharT, "LINE TOO LONG"));
        }
        else
        {
            pos += body_size;
        }
        pos += append_line_end(buffer, pos);
        buffer[pos++] = 0;

        // 打印输出
        write(buffer, pos - 1, system_time);
    }

    void hexdump(
        unsigned int level,
        const CharT *filename,
        unsigned int line,
        const CharT *function,
        const void *data,
        int size)
    {
        if (size <= 0)
            return;
        if ((level_ & level) == 0)
            return;

        // 日志时间
        SystemTime system_time = get_system_time();

        // 初始化行头
        const int kMaxHeadSize = 2048;
        CharT head_buffer[kMaxHeadSize + 16];
        int pos = format_line_head(
            field_type_,
            head_buffer,
            kMaxHeadSize,
            level,
            filename,
            line,
            function,
            system_time);
        pos += append_line_end(head_buffer, pos);
        head_buffer[pos++] = 0;

        // 格式化打印数据
        std::basic_string<CharT> hex_data_buffer =
            hex_display_string<CharT>(data, size);

        std::basic_string<CharT> buffer(head_buffer);
        buffer.append(hex_data_buffer);

        // 打印输出
        write(buffer.c_str(), buffer.size(), system_time);
    }

private:
    void write(
        const CharT *message,
        int size,
        const SystemTime &system_time)
    {
        std::lock_guard<std::mutex> guard(lock_);
        if (storage_type_ & kScreen)
            print_to_screen(message);
        if (storage_type_ & kDiskFile)
            wirte_to_file(message, size, system_time);
    }

    void wirte_to_file(
        const CharT *message,
        int size,
        const SystemTime &system_time)
    {
        sprintf_filename_suffix(
            new_suffix_,
            std::extent<decltype(new_suffix_)>::value,
            system_time);
        if (strcmp(old_suffix_, new_suffix_) != 0)
        {
            clear_files();
            std::basic_string<CharT> path = get_log_filename(new_suffix_);
            file_.close();
            open_file(file_, path);
            strcpy(
                old_suffix_,
                std::extent<decltype(old_suffix_)>::value,
                new_suffix_);
        }
        if (file_.good())
        {
            file_.write((const char *)message, size * sizeof(CharT));
            file_.flush();
        }
    }

    void clear_files()
    {
        time_t seconds = time(NULL) - reserve_days_ * 86400;
        for (seconds -= 24 * 3600;; seconds -= 24 * 3600)
        {
            std::basic_string<CharT> filename = get_log_filename(seconds);
            if (!delete_file(filename.c_str()))
                break;
        }
    }

private:
    std::basic_string<CharT> get_log_filename(const CharT *suffix)
    {
        return log_directory_ + prefix_name_ + suffix;
    }

    std::basic_string<CharT> get_log_filename(time_t t)
    {
        SystemTime system_time = to_system_time(t);
        CharT suffix[kLogFilenameSuffixSize];
        sprintf_filename_suffix(
            suffix,
            std::extent<decltype(suffix)>::value,
            system_time);
        return get_log_filename(suffix);
    }

private:
    std::mutex lock_;
    unsigned int storage_type_ = kStorageTypeMask;
    unsigned int field_type_ = kFieldTypeMask;
    unsigned int level_ = kLevelMask;
    unsigned int reserve_days_ = 30; // 日志保留天数，默认30天
    int filename_depth_ = 1;
    std::basic_string<CharT> log_directory_ = CHAR_T(CharT, "");
    std::basic_string<CharT> prefix_name_ = CHAR_T(CharT, "");
    CharT old_suffix_[kLogFilenameSuffixSize];
    CharT new_suffix_[kLogFilenameSuffixSize];
    std::ofstream file_;
};

#if defined(WIN32) && defined(UNICODE)
typedef Log<wchar_t> Logger;
#else
typedef Log<char> Logger;
#endif
} // namespace detail

#if defined(WIN32) && defined(UNICODE)
void print_v(
    unsigned int level,
    const wchar_t *filename,
    unsigned int line,
    const wchar_t *function,
    const wchar_t *format,
    va_list ap)
{
    detail::Logger::instance().print_v(
        level,
        filename,
        line,
        function,
        format,
        ap);
}
#else
void print_v(
    unsigned int level,
    const char *filename,
    unsigned int line,
    const char *function,
    const char *format,
    va_list ap)
{
    detail::Logger::instance().print_v(
        level,
        filename,
        line,
        function,
        format,
        ap);
}
#endif

#if defined(WIN32) && defined(UNICODE)
void print(
    unsigned int level,
    const wchar_t *filename,
    unsigned int line,
    const wchar_t *function,
    const wchar_t *format,
    ...)
{
    va_list ap;
    va_start(ap, format);
    print_v(level, filename, line, function, format, ap);
    va_end(ap);
}
#else
void print(
    unsigned int level,
    const char *filename,
    unsigned int line,
    const char *function,
    const char *format,
    ...)
{
    va_list ap;
    va_start(ap, format);
    print_v(level, filename, line, function, format, ap);
    va_end(ap);
}
#endif

#if defined(WIN32) && defined(UNICODE)
void hexdump(
    unsigned int level,
    const wchar_t *filename,
    unsigned int line,
    const wchar_t *function,
    const void *data,
    int size)
{
    detail::Logger::instance().hexdump(
        level,
        filename,
        line,
        function,
        data,
        size);
}
#else
void hexdump(
    unsigned int level,
    const char *filename,
    unsigned int line,
    const char *function,
    const void *data,
    int size)
{
    detail::Logger::instance().hexdump(
        level,
        filename,
        line,
        function,
        data,
        size);
}
#endif

#if defined(WIN32) && defined(UNICODE)
void set_directory(const wchar_t *path)
{
    detail::Logger::instance().set_directory(path);
}
#else
void set_directory(const char *path)
{
    detail::Logger::instance().set_directory(path);
}
#endif

#if defined(WIN32) && defined(UNICODE)
void set_prefix_name(const wchar_t *prefix_name)
{
    detail::Logger::instance().set_prefix_name(prefix_name);
}
#else
void set_prefix_name(const char *prefix_name)
{
    detail::Logger::instance().set_prefix_name(prefix_name);
}
#endif

void set_level(unsigned int level)
{
    detail::Logger::instance().set_level(level);
}

void set_storage_type(unsigned int storage_type)
{
    detail::Logger::instance().set_storage_type(storage_type);
}

void set_field_type(unsigned int field_type)
{
    detail::Logger::instance().set_field_type(field_type);
}

void set_reserve_days(int days)
{
    detail::Logger::instance().set_reserve_days(days);
}
} // namespace log
} // namespace ustd
