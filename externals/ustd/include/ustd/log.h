#ifndef USTD_LOG_H_
#define USTD_LOG_H_

#include <stdarg.h>

namespace ustd
{
namespace log
{
/// 日志级别
enum Level
{
    kStartup = 0x01,  ///< 启动级别
    kShutdown = 0x02, ///< 关闭级别
    kInfo = 0x04,     ///< 普通信息级别
    kWarning = 0x08,  ///< 警告级别
    kTrace = 0x10,    ///< 跟踪级别
    kError = 0x20,    ///< 错误级别
    kDebug = 0x40,    ///< 调试级别

    kLevelMask = 0x7F, ///< mask
};

/// 日志输出类型
enum StorageType
{
    kScreen = 0x01,   ///< 屏幕输出
    kDiskFile = 0x02, ///< 日志文件输出

    kStorageTypeMask = 0x0F, ///< mask
};

/// 字段类型
enum FieldType
{
    kTimestamp = 0x01, ///< 输出时间标签
    kThreadId = 0x02,  ///< 输出线程Id标签
    kFileLine = 0x04,  ///< 输出在源代码文件和行数标签
    kFunction = 0x08,  ///< 输出在源代码的函数名称

    kFieldTypeMask = 0x0F, // mask
};

/// @brief
/// @param[in] level 日志级别
/// @param[in] filename 文件名
/// @param[in] line 行
/// @param[in] function 函数
/// @param[in] format 格式
/// @param[in] ap
#if !(defined(WIN32) && defined(UNICODE))
void print_v(
    unsigned int level,
    const char *filename,
    unsigned int line,
    const char *function,
    const char *format,
    va_list ap);
#else
void print_v(
    unsigned int level,
    const wchar_t *filename,
    unsigned int line,
    const wchar_t *function,
    const wchar_t *format,
    va_list ap);
#endif

/// @brief
/// @param[in] level 日志级别
/// @param[in] filename 文件名
/// @param[in] line 行
/// @param[in] function 函数
/// @param[in] format 格式
#if !(defined(WIN32) && defined(UNICODE))
void print(
    unsigned int level,
    const char *filename,
    unsigned int line,
    const char *function,
    const char *format,
    ...);
#else
void print(
    unsigned int level,
    const wchar_t *filename,
    unsigned int line,
    const wchar_t *function,
    const wchar_t *format,
    ...);
#endif

/// @brief
/// @param[in] level 日志级别
/// @param[in] filename 文件名
/// @param[in] line 行
/// @param[in] function 函数
/// @param[in] format 格式
/// @param[in] ap
#if !(defined(WIN32) && defined(UNICODE))
void hexdump(
    unsigned int level,
    const char *filename,
    unsigned int line,
    const char *function,
    const void *data,
    int size);
#else
void hexdump(
    unsigned int level,
    const wchar_t *filename,
    unsigned int line,
    const wchar_t *function,
    const void *data,
    int size);
#endif

/// @brief 设置日志路径
/// @param[in] path
#if !(defined(WIN32) && defined(UNICODE))
void set_directory(const char *path);
#else
void set_directory(const wchar_t *path);
#endif

/// @brief 设置日志文件名前缀
/// @param[in] prefix_name
#if !(defined(WIN32) && defined(UNICODE))
void set_prefix_name(const char *prefix_name);
#else
void set_prefix_name(const wchar_t *prefix_name);
#endif

/// @brief 设置日志级别
/// @param[in] level 日志级别
void set_level(unsigned int level);
/// @brief 设置日志存储类型
/// @param[in] storage_type
void set_storage_type(unsigned int storage_type);
/// @brief
/// @param[in] field_type
void set_field_type(unsigned int field_type);
/// @brief 设置日志保留天数
/// @param[in] days 日志保存天数
/// @note 日志保留天数，默认为30天
void set_reserve_days(int days);

} // namespace log
} // namespace ustd

enum LogLevel // 日志输出级别
{
    kStartup = ustd::log::kStartup,   // 启动级别
    kShutdown = ustd::log::kShutdown, // 关闭级别
    kInfo = ustd::log::kInfo,         // 普通信息级别
    kWarning = ustd::log::kWarning,   // 警告级别
    kTrace = ustd::log::kTrace,       // 跟踪级别
    kError = ustd::log::kError,       // 错误级别
    kDebug = ustd::log::kDebug        // 调试级别
};

#if !(defined(WIN32) && defined(UNICODE))
#define log_print(level, ...) \
    ustd::log::print(level, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define log_hex(level, data, size) \
    ustd::log::hexdump(level, __FILE__, __LINE__, __FUNCTION__, data, size)
#else
#define log_print(level, ...) \
    ustd::log::print(level, __FILEW__, __LINE__, __FUNCTIONW__, __VA_ARGS__)
#define log_hex(level, data, size) \
    ustd::log::hexdump(level, __FILEW__, __LINE__, __FUNCTIONW__, data, size)
#endif

#endif // USTD_LOG_H_
