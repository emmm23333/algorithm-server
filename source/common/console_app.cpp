#include "console_app.h"
#include <mutex>
#include <string>
#include <vector>
#include <assert.h>
#include <unistd.h>
#include <execinfo.h>
#include <signal.h>
#include <ustd/log.h>

namespace common
{

std::once_flag the_once_flag;
std::weak_ptr<ConsoleApp> the_weak_app;

void notify_exit()
{
    auto shared_app = the_weak_app.lock();
    if (shared_app != nullptr)
    {
        shared_app->exit_flag_ = true;
    }
}

std::string signum_to_string(int signum)
{
    switch (signum)
    {
#define SIGNAL_CASE(signal) \
    case signal:            \
        return std::string(#signal);
        SIGNAL_CASE(SIGHUP)
        SIGNAL_CASE(SIGINT)
        SIGNAL_CASE(SIGTERM)
        SIGNAL_CASE(SIGQUIT)
        SIGNAL_CASE(SIGSEGV)
        SIGNAL_CASE(SIGABRT)
#undef SIGNAL_CASE
    }
    char other_signal[16] = {0};
    sprintf(other_signal, "%d", signum);
    return std::string(other_signal);
}

bool backtrace_to_string(std::string &bt)
{
    // const int stack_size = 64;
    // char *stack_trace[stack_size] = {0};
    // int stack_num = ::backtrace((void **)stack_trace, stack_size);
    // char **stack_trace_symbols = ::backtrace_symbols(
    //     (void **)stack_trace, stack_num);
    // if (stack_trace_symbols == nullptr)
    // {
    //     return false;
    // }

    // for (int i = 0; i < stack_num; ++i)
    // {
    //     bt.append(stack_trace_symbols[i]);
    //     bt.append("\n");
    // }
    // free(stack_trace_symbols);
    bt = "not supported in alpine";
    return true;
}

void handle_signal(int signum, siginfo_t *siginfo, void *)
{
    assert(signum == siginfo->si_signo);
    std::string signal_string = signum_to_string(signum);
    log_print(kInfo, "signal %s catched.", signal_string.c_str());

    switch (signum)
    {
    case SIGHUP:
        break;
    case SIGINT:
    case SIGTERM:
        notify_exit();
        break;
    case SIGQUIT:
    case SIGSEGV:
    case SIGABRT:
    default:
        std::string bt;
        if (backtrace_to_string(bt))
        {
            log_print(kError, "######## backtrace ########\n%s", bt.c_str());
        }
        // re-throw to default handler
        ::raise(siginfo->si_signo);
        break;
    }
}

ConsoleApp::ConsoleApp(const std::string &name)
    : name_(name)
{
}

ConsoleApp::~ConsoleApp()
{
}

bool ConsoleApp::start(
    const std::function<bool()> &on_init,
    const std::function<void()> &on_exit,
    const std::function<void()> &on_idle)
{
    on_init_ = on_init;
    on_exit_ = on_exit;
    on_idle_ = on_idle;

    std::call_once(the_once_flag, [&]() { the_weak_app = std::enable_shared_from_this<ConsoleApp>::shared_from_this(); });

    // 屏蔽无关信号
    // std::vector<int> listen_signals{SIGHUP, SIGINT, SIGTERM, SIGSEGV, SIGABRT};
    // sigset_t mask_signals, old_signals;
    // ::sigfillset(&mask_signals);
    // for (int signal : listen_signals)
    // {
    //     ::sigdelset(&mask_signals, signal);
    // }
    // ::sigprocmask(SIG_BLOCK, &mask_signals, &old_signals);

    // // 设置信号处理器
    // struct sigaction act;
    // ::sigemptyset(&act.sa_mask);
    // act.sa_flags = SA_SIGINFO | SA_RESETHAND;
    // act.sa_sigaction = handle_signal;
    // for (int signal : listen_signals)
    // {
    //     if (::sigaction(signal, &act, NULL) < 0)
    //     {
    //         printf("sigaction(%d) failed, errno:%d.\n", signal, errno);
    //         return false;
    //     }
    // }

    if (on_init_ != nullptr)
    {
        return on_init_();
    }
    return true;
}

void ConsoleApp::run()
{
    while (!exit_flag_)
    {
        ::usleep(1000000);
        if (on_idle_ != nullptr)
        {
            on_idle_();
        }
    }
    if (on_exit_ != nullptr)
    {
        on_exit_();
    }
}

} // namespace common