#ifndef COMMON_CONSOLE_APP_H_
#define COMMON_CONSOLE_APP_H_

#include <memory>
#include <functional>
#include <string>
#include <stdint.h>

namespace common
{

class ConsoleApp : public std::enable_shared_from_this<ConsoleApp>
{
public:
    explicit ConsoleApp(const std::string &name);
    ~ConsoleApp();

    ConsoleApp() = delete;
    ConsoleApp(ConsoleApp &&) = delete;
    ConsoleApp(const ConsoleApp &) = delete;
    ConsoleApp &operator=(ConsoleApp &&) = delete;
    ConsoleApp &operator=(const ConsoleApp &) = delete;

    bool start(
        const std::function<bool()> &on_init,
        const std::function<void()> &on_exit,
        const std::function<void()> &on_idle);
    void run();

protected:
    friend void notify_exit();
    bool exit_flag_ = false;

private:
    std::string name_;
    std::function<bool()> on_init_;
    std::function<void()> on_exit_;
    std::function<void()> on_idle_;
};

} // namespace common

#endif // COMMON_CONSOLE_APP_H_