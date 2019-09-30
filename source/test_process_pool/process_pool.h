#ifndef _PROCESSPOOL_H_
#define _PROCESSPOOL_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/signalfd.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>

#include <mosquittopp.h>
#include <protocol/algorithmservice/serialize.h>
#include <protocol/algorithmservice/mosquitto_message.h>
#include <protocol/algorithmservice/mqtt_message.h>
#include <protocol/algorithmservice/task_result.h>
#include <business/config.h>
#include <ustd/log.h>
namespace ustd
{

const int MAX_BUFF = 1024 * 1024;
// 从阻塞socket一直读到指定大小。
// 应该放到网络库里，这个项目就放这先用着吧。
ssize_t readn(int fd, void *vptr, size_t n);
ssize_t readMsg(int fd, void *vptr);
void sendMsg(int fd, void *vptr, uint64_t len);

enum SelectMode
{
    RandomFree,       // 空闲中随机挑一个
    InTurn,           // 轮流
    RandomFreeAbility // 空闲中按优先级（能力）随机挑选
};

struct ProcessState
{
    unsigned respawn : 1;
    unsigned busy : 1;
    unsigned exited : 1;
    unsigned coredumped : 1;
    int exitsig;
};

class process
{
public:
    process() : m_pid(-1) {}

    // 进程id号
    pid_t m_pid;
    // 父子进程通信管道
    int m_pipefd[2];
    // 信号接收管道
    int m_sigfd;
    // 独立的进程IO缓冲区
    char recv_buff[MAX_BUFF] = {0};
    // 进程状态
    ProcessState m_state;
    // 任务参数
    protocol::algorithmservice::mosquitto_message m_task_msg;
};

template <typename T, typename P>
class processpool
{
private:
    processpool(int process_number = 8);

public:
    static processpool<T, P> *create(int process_number = 8)
    {
        if (m_instance == NULL)
        {
            m_instance = new processpool<T, P>(process_number);
        }
        return m_instance;
    }
    ~processpool()
    {
        delete[] m_sub_process;
    }
    void run(const algorithmservice::business::Config &config);

private:
    void setup_sig_pipe();
    void run_parent(const algorithmservice::business::Config &config);
    void run_child(const algorithmservice::business::Config &config);
    int isChildFd(int fd);

    int select_process(SelectMode);
    bool generate_err_json(const std::string &, const std::string &, const std::string &, std::string &);

private:
    static const int MAX_PROCESS_NUMBER = 16;
    //epoll最多能处理的事件数
    static const int MAX_EVENT_NUMBER = 10000;
    //进程池中的进程总数
    int m_process_number;
    //子进程在进程池中的序号，从0开始
    int m_idx;
    //每个进程都有一个epoll内核事件表
    int m_epollfd;
    //子进程是否停止运行标志
    int m_stop;
    //保存所有子进程的描述信息
    process *m_sub_process;
    static processpool<T, P> *m_instance;

    // linux中专用于信号-epoll事件转换的fd；
    // “捕获信号->发送事件”的方法经测试会丢信号，不知道为什么。用这个fd测试没发现问题。
    int m_psigfd;
    // 事件读写缓冲区
    char m_fd_buf[1024 * 1024];
    // // 和子进程通信的缓冲区，这种业务应该不需要这么大，后面看情况吧
    // char m_recv_child_buf[1024 * 1024];
    std::map<int, std::shared_ptr<protocol::algorithmservice::mosquitto_message>> m_task_record;
    algorithmservice::business::Config m_config;
};

//初始化类静态变量
template <typename T, typename P>
processpool<T, P> *processpool<T, P>::m_instance = NULL;

// 用于处理信号的管道,用来实现统一事件源
static int sig_pipefd[2];

// mqtt消息管道，用来实现统一事件源
static int sig_mqfd[2];

//设置非阻塞模式
static int setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

//向epoll中注册要监听的文件描述符
static void addfd(int epollfd, int fd)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

static void addfdNonBlock(int epollfd, int fd)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
}

static void addfdLT(int epollfd, int fd)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLONESHOT;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    // setnonblocking(fd);
}

static void removefd(int epollfd, int fd)
{
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
    close(fd);
}

bool backtrace_to_string(std::string &bt)
{
    // Note:musl不支持backtrace

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

// Note: 信号处理函数，通过管道将信号发送给epoll监听的文件描述符，实现统一事件源
// 又名"Self-Pipe Trick", 既然有了signalfd，就不用这种方法了
static void sig_handler(int sig)
{
    printf("sig handler sending %d\n", sig);
    int save_errno = errno;
    int msg = sig;
    send(sig_pipefd[1], (char *)&msg, 1, 0);
    // send(sig_pipefd[0], (char *)&msg, 1, 0);
    errno = save_errno;
}

static void sig_handler2(int signum, siginfo_t *siginfo, void *)
{
    int save_errno = errno;
    int msg = signum;
    assert(signum == siginfo->si_signo);
    std::string signal_string = signum_to_string(signum);
    int ret = send(sig_pipefd[1], (char *)&msg, 1, 0);
    log_print(kInfo, "signal %s catched, pid: %ld, send ret: %d", signal_string.c_str(), long(getpid()), ret);
    errno = save_errno;
}

// 设置信号，并指定是否重新调用被该信号终止的系统调用
void addsig(int sig, void(handler)(int), bool restart = true)
{
    printf("adding sig %d \n", sig);
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = handler;
    if (restart)
    {
        // sa.sa_flags = SA_SIGINFO | SA_RESETHAND;
        sa.sa_flags |= SA_RESTART;
    }
    sigfillset(&sa.sa_mask);
    assert(sigaction(sig, &sa, NULL) != -1);
}

void addsig2()
{
    // 屏蔽无关信号
    std::vector<int> listen_signals{SIGHUP, SIGINT, SIGTERM, SIGSEGV, SIGABRT, SIGCHLD, SIGPIPE};
    sigset_t mask_signals, old_signals;
    ::sigfillset(&mask_signals);
    for (int signal : listen_signals)
    {
        ::sigdelset(&mask_signals, signal);
    }
    ::sigprocmask(SIG_BLOCK, &mask_signals, &old_signals);

    // sigset_t cur_signals;
    // ::sigemptyset(&cur_signals);
    // for (int signal : listen_signals)
    // {
    //     sigaddset(&cur_signals, signal);
    // }
    // int sig_fd = ::signalfd(-1, &cur_signals, SFD_NONBLOCK);
    // addfd(epollfd, sig_fd);

    // 设置信号处理器
    struct sigaction act;
    ::sigemptyset(&act.sa_mask);
    // RESETHAND:重置信号处理函数
    // act.sa_flags = SA_SIGINFO | SA_RESETHAND;
    // RESTART:中断系统调用的自动重新启动（unix高级环境编程P261）
    act.sa_flags |= SA_RESTART;
    act.sa_sigaction = sig_handler2;
    for (int signal : listen_signals)
    {
        // if (::sigaction(signal, &act, NULL) < 0)
        // {
        //     printf("sigaction(%d) failed, errno:%d.\n", signal, errno);
        //     return;
        // }
    }
}

//进程池构造函数，参数listenfd为监听socket,它必须在创建进程池之前被创建，否则子进程无法直接引用它
//参数process_number指定进程池中的进程数量
template <typename T, typename P>
processpool<T, P>::processpool(int process_number)
    : m_process_number(process_number), m_idx(-1), m_stop(false)
{
    assert((process_number > 0) && (process_number <= MAX_PROCESS_NUMBER));
    m_sub_process = new process[process_number];
    assert(m_sub_process);
    printf("before create processpool, parent pid: %ld\n", long(getpid()));
    //创建process_number个子进程，并建立他们和父进程之间的通道

    // Note:为什么父进程的epoll和signalfd必须在fork之前创建？？否则会偶尔收到信号？？匪夷所思
    setup_sig_pipe();
    std::vector<int> listen_signals{SIGHUP, SIGINT, SIGTERM, SIGSEGV, SIGABRT, SIGCHLD, SIGPIPE};
    sigset_t cur_signals;
    ::sigemptyset(&cur_signals);
    for (int signal : listen_signals)
    {
        sigaddset(&cur_signals, signal);
    }
    if (sigprocmask(SIG_BLOCK, &cur_signals, NULL) < 0)
    {
        printf("sigprocmask error: %d\n", errno);
    }
    m_psigfd = ::signalfd(-1, &cur_signals, 0);
    printf("singalfd: %d\n", m_psigfd);
    addfd(m_epollfd, m_psigfd);

    for (int i = 0; i < process_number; ++i)
    {
        int ret = socketpair(PF_UNIX, SOCK_STREAM, 0, m_sub_process[i].m_pipefd);
        assert(ret == 0);
        // fork子进程
        // TODO... 这对套接字既读又写，后面要思考下会不会引发问题
        m_sub_process[i].m_pid = fork();
        assert(m_sub_process[i].m_pid >= 0);
        if (m_sub_process[i].m_pid > 0)
        {
            printf("child pid: %ld\n", m_sub_process[i].m_pid);
            m_sub_process[i].m_state.busy = 0;
            m_sub_process[i].m_state.coredumped = 0;
            m_sub_process[i].m_state.exited = 0;
            m_sub_process[i].m_state.respawn = 0;
            close(m_sub_process[i].m_pipefd[1]);
            continue;
        }
        else
        {
            close(m_sub_process[i].m_pipefd[0]);
            m_idx = i;
            break;
        }
    }
}

template <typename T, typename P>
void processpool<T, P>::setup_sig_pipe()
{
    //创建epoll事件监听表和信号管道
    m_epollfd = epoll_create(20);
    assert(m_epollfd != -1);

    // int ret = socketpair(PF_UNIX, SOCK_STREAM, 0, sig_pipefd);
    // assert(ret != -1);

    // setnonblocking(sig_pipefd[1]);
    // addfd(m_epollfd, sig_pipefd[0]); //将信号事件与其他文件描述符事件统一处理

    //设置信号处理函数
    // addsig(SIGCHLD, sig_handler);
    // addsig(SIGTERM, sig_handler);
    // addsig(SIGINT, sig_handler);
    // addsig(SIGPIPE, SIG_IGN);
    //addsig2();
}

template <typename T, typename P>
void processpool<T, P>::run(const algorithmservice::business::Config &config)
{
    m_config = config;
    if (m_idx != -1)
    {
        run_child(config);
        // TODO...外部有死循环，所以这里要exit，等后面重构再改吧
        exit(0);
        // return;
    }
    run_parent(config);
    // TODO.. 要加上清理资源！
    exit(0);
}

//子进程运行函数
template <typename T, typename P>
void processpool<T, P>::run_child(const algorithmservice::business::Config &config)
{
    // close(m_psigfd);
    // close(m_epollfd);
    setup_sig_pipe();

    //每个子进程通过其在进程池中的序号m_idx找到与父进程通信的管道
    int pipefd = m_sub_process[m_idx].m_pipefd[1];

    addfd(m_epollfd, pipefd);

    std::vector<int> listen_signals{SIGHUP, SIGABRT, SIGPIPE, SIGTERM, SIGTTIN, SIGSEGV};
    sigset_t cur_signals;
    ::sigemptyset(&cur_signals);
    for (int signal : listen_signals)
    {
        sigaddset(&cur_signals, signal);
    }
    sigprocmask(SIG_BLOCK, &cur_signals, NULL);
    m_sub_process[m_idx].m_sigfd = ::signalfd(-1, &cur_signals, 0);
    addfd(m_epollfd, m_sub_process[m_idx].m_sigfd);

    epoll_event events[MAX_EVENT_NUMBER];

    std::shared_ptr<T> task = std::make_shared<T>();
    assert(task);
    task->init(m_epollfd, pipefd, config);

    int number = 0;
    int ret = -1;

    while (!m_stop)
    {
        printf("child %d before epoll\n", getpid());
        number = epoll_wait(m_epollfd, events, MAX_EVENT_NUMBER, -1);
        if ((number < 0) && (errno != EINTR))
        {
            printf("epoll failure\n");
            break;
        }
        printf("child %d after epoll, number: %d\n", getpid(), number);

        for (int i = 0; i < number; i++)
        {
            int sockfd = events[i].data.fd;
            // 接收父进程数据
            if ((sockfd == pipefd) && (events[i].events & EPOLLIN))
            {
                printf("child receive from parent pipe\n");
                char *buf = m_sub_process[m_idx].recv_buff;
                memset(buf, 0, sizeof(m_sub_process[m_idx].recv_buff));
                ret = readMsg(sockfd, buf);
                if (ret == -1)
                {
                    printf("parent send invalid data!\n");
                }
                if (((ret < 0) && (errno != EAGAIN)) || ret == 0)
                {
                    printf("child continue\n");
                    continue;
                }
                printf("child before binary serial\n");
                std::shared_ptr<protocol::algorithmservice::mosquitto_message> msg = protocol::algorithmservice::BoostBinaryDeserialize<protocol::algorithmservice::mosquitto_message>(std::string(buf, ret));
                // ret = readn(sockfd, recvBuf, sizeof(struct mosquitto_message));
                // struct mosquitto_message *ptr = (struct mosquitto_message *)&recvBuf;
                printf("child after binary serial\n");
                std::string result;
                task->process(msg, result);
                log_print(kDebug, "child sending result to parent! fd: %d", m_sub_process[m_idx].m_pipefd[0]);
                sendMsg(m_sub_process[m_idx].m_pipefd[1], (void *)result.c_str(), result.length());
            }
            // else if ((sockfd == sig_pipefd[0]) && (events[i].events & EPOLLIN)) //子进程的信号事件
            else if ((sockfd == m_sub_process[m_idx].m_sigfd) && (events[i].events & EPOLLIN)) //子进程的信号事件
            {
                // char signals[1024];
                // ret = recv(sig_pipefd[0], signals, sizeof(signals), 0);
                struct signalfd_siginfo info;
                ret = read(m_psigfd, &info, sizeof(info));
                printf("child recv signal, ret: %d\n", ret);
                if (ret <= 0)
                {
                    continue;
                }
                else
                {
                    unsigned sig = info.ssi_signo;
                    switch (sig)
                    {
                    // 按现在这种架构，一个进程干一个活，epoll_wait返回后直接开始干活，那就不可能捕获干活中的信号了（比如SIGSEGV）
                    // 那么主进程里捕获SIGCHLD统一处理吧
                    case SIGCHLD:
                    {
                        printf("child %d recv sigchild\n", getpid());
                        pid_t pid;
                        int stat;
                        while ((pid = waitpid(-1, &stat, WNOHANG)) < 0)
                        {
                            continue;
                        }
                        break;
                    }
                    case SIGTERM:
                        printf("child %d recv SIGTERM\n", getpid());
                        m_stop = true;
                        break;

                        // TODO...子进程要不要阻塞SIGINT的？有机会重构的话，参考下nginx这种怎么写的
                    case SIGINT:
                    {
                        printf("child %d recv SIGINT\n", getpid());
                        m_stop = true;
                        break;
                    }
                    case SIGSEGV:
                        printf("child(%d) recv SIGSEGV!\n", getpid());
                        break;
                    default:
                        printf("default: child %d recv %d\n", getpid(), sig);
                        break;
                    }
                }
            }
            else if (events[i].events & EPOLLIN) //其他可读数据事件
            {
                printf("other...\n");
            }
            else
            {
                printf("continue...\n");
                continue;
            }
        }
    }

    printf("child %d is cleaning...\n", getpid());
    //释放资源
    close(pipefd);
    close(m_epollfd);
    printf("child %d has cleaned...\n", getpid());
}

template <typename T, typename P>
void processpool<T, P>::run_parent(const algorithmservice::business::Config &config)
{
    // setup_sig_pipe();

    epoll_event events[MAX_EVENT_NUMBER];
    int sub_process_counter = 0;
    int number = 0;
    int ret = -1;

    P *sche = new P();
    ret = socketpair(PF_UNIX, SOCK_STREAM, 0, sig_mqfd);
    assert(ret != -1);
    // 0用作读，1用作写
    addfd(m_epollfd, sig_mqfd[0]);
    sche->init(m_epollfd, sig_mqfd[1], config);
    sche->process();

    // Note: 为啥fork之后创建不行？？
    // std::vector<int> listen_signals{SIGHUP, SIGINT, SIGTERM, SIGSEGV, SIGABRT, SIGCHLD, SIGPIPE};
    // sigset_t cur_signals;
    // ::sigemptyset(&cur_signals);
    // for (int signal : listen_signals)
    // {
    //     sigaddset(&cur_signals, signal);
    // }
    // if (sigprocmask(SIG_BLOCK, &cur_signals, NULL) < 0)
    // {
    //     printf("sigprocmask error: %d\n", errno);
    // }
    // m_psigfd = ::signalfd(-1, &cur_signals, 0);
    // printf("singalfd: %d\n", m_psigfd);
    // addfd(m_epollfd, m_psigfd);

    // 父进程监听所有子进程通道
    for (int i = 0; i < m_process_number; ++i)
    {
        addfd(m_epollfd, m_sub_process[i].m_pipefd[0]);
    }

    while (!m_stop)
    {
        printf("parent before epoll_wait!\n");
        number = epoll_wait(m_epollfd, events, MAX_EVENT_NUMBER, -1);
        printf("parent after epoll_wait,number: %d, errno:%d\n", number, errno);
        if ((number < 0) && (errno != EINTR))
        {
            printf("epoll faliure\n");
            break;
        }

        // TODO... 子线程最大任务数量控制，空闲状态维护
        for (int i = 0; i < number; i++)
        {
            int sockfd = events[i].data.fd;
            printf("parent recv socketfd: %d\n", sockfd);
            int index = -2;
            if (sockfd == sig_mqfd[0])
            {
                // 采用指定模式选择子进程
                printf("======parent recv mqtt!\n");
                // 轮流：
                // int i = sub_process_counter;
                // do
                // {
                //     if (m_sub_process[i].m_pid != -1)
                //     {
                //         break;
                //     }
                //     i = (i + 1) % m_process_number;
                // } while (i != m_process_number);

                // if (m_sub_process[i].m_pid == -1)
                // {
                //     m_stop = true;
                //     break;
                // }
                // sub_process_counter = (i + 1) % m_process_number;

                // 所有事件统一处理，带来了效率问题。不过收mq任务消息而已，应该问题不大。
                // ret = readn(sockfd, m_fd_buf, sizeof(struct mosquitto_message));
                // ret = recv(sockfd, m_fd_buf, sizeof(m_fd_buf), 0);
                // printf("sizeof message struct: %d %d %d %d %d\n", sizeof(struct mosquitto_message), sizeof(int), sizeof(char *), sizeof(void *), sizeof(bool));
                // struct mosquitto_message *ptr = (struct mosquitto_message *)&m_fd_buf;
                ret = readMsg(sockfd, m_fd_buf);
                printf("readMsg ret:%d\n", ret);
                fflush(stdout);
                if (ret == -100)
                {
                    printf("mqt send invalid data!\n");
                }

                // Note: 放读socket之后是有讲究的
                int i = select_process(SelectMode::RandomFree);
                if (i < 0)
                {
                    sche->publish_to_mq("no available service");
                    continue;
                }
                // 设置进程属性：
                m_sub_process[i].m_state.busy = 1;
                m_sub_process[i].m_state.respawn = 0;
                m_sub_process[i].m_state.exited = 0;
                m_sub_process[i].m_state.exitsig = 0;

                std::shared_ptr<protocol::algorithmservice::mosquitto_message> msg = protocol::algorithmservice::BoostBinaryDeserialize<protocol::algorithmservice::mosquitto_message>(std::string(m_fd_buf, ret));

                m_task_record[i] = msg;
                // ret = readn(sockfd, recvBuf, sizeof(struct mosquitto_message));
                // struct mosquitto_message *ptr = (struct mosquitto_message *)&recvBuf;
                // printf("recv ret:%d mid:%d, qos:%d, retain:%d, topic:%s, payload:%s\n", ret, msg->m_mid, msg->m_qos, msg->m_retain, msg->m_topic.data(), msg->m_payload.data());

                // 分发到子进程。这里用boost.serialization作序列化。
                // TODO...作为common下的进程池，这里涉及业务不太好，但暂时就这样吧。
                std::string binary_msg = protocol::algorithmservice::BoostBinarySerialize<protocol::algorithmservice::mosquitto_message>(msg);

                uint64_t length = binary_msg.length();
                send(m_sub_process[i].m_pipefd[0], &length, sizeof(uint64_t), 0);
                send(m_sub_process[i].m_pipefd[0], binary_msg.data(), binary_msg.length(), 0);
                printf("send request to child %d with fd %d\n", i, m_sub_process[i].m_pipefd[0]);
                fflush(stdout); // debug
            }
            //处理父进程接收到的信号
            // else if ((sockfd == sig_pipefd[0]) && (events[i].events & EPOLLIN))
            else if ((sockfd == m_psigfd) && (events[i].events & EPOLLIN))
            {
                // char signals[1024];
                // ret = recv(sig_pipefd[0], signals, sizeof(signals), 0);
                struct signalfd_siginfo info;
                ret = read(m_psigfd, &info, sizeof(info));
                printf("======parent recv signal, ret: %d, errno: %d\n", ret, errno);
                if (ret <= 0)
                {
                    // printf("parent recv signal error, ret: %d\n", ret);
                    continue;
                }
                else
                {
                    unsigned sig = info.ssi_signo;
                    printf("======parent recv signal number: %d\n", sig);
                    switch (sig)
                    {
                    case SIGCHLD:
                        printf("parent(%d) recv SIGCHILD!\n", getpid());
                        pid_t pid;
                        int stat;
                        while ((pid = waitpid(-1, &stat, WNOHANG)) > 0) //等待任何子进程
                        {
                            printf("waitpid: %d\n", pid);
                            for (int i = 0; i < m_process_number; i++)
                            {
                                //如果进程池中第i个进程退出了，则主进程关闭相应的通信管道，
                                //并置m_pid为-1，标记该子进程已经退出
                                if (m_sub_process[i].m_pid == pid)
                                {
                                    printf("child %d join\n", i);
                                    close(m_sub_process[i].m_pipefd[0]);

                                    if (WIFEXITED(stat))
                                    {
                                        // 这种是正常退出，比如control+c发送的SIGINT！
                                        m_sub_process[i].m_pid = -1;
                                        printf("child(%d) with pid(%d) normal exit, status = %d\n", i, m_sub_process[i].m_pid, WEXITSTATUS(stat));
                                    }
                                    else if (WIFSIGNALED(stat))
                                    {
                                        m_sub_process[i].m_state.exitsig = WTERMSIG(stat);
                                        printf("child %d with pid(%d) abnormal exit, signal number = %d\n", i, m_sub_process[i].m_pid, WTERMSIG(stat));
#ifdef WCOREDUMP
                                        if (WCOREDUMP(stat))
                                        {
                                            m_sub_process[i].m_state.coredumped = 1;
                                            printf("child %d with pid(%d) generate dump\n", i, m_sub_process[i].m_pid);
                                        }
                                        else
                                        {
                                            printf("child %d with pid(%d) didn't generate dump\n", i, m_sub_process[i].m_pid);
                                        }
#endif

                                        // 非正常退出，补充子进程！
                                        int ret = socketpair(PF_UNIX, SOCK_STREAM, 0, m_sub_process[i].m_pipefd);
                                        assert(ret == 0);
                                        m_sub_process[i].m_pid = fork();
                                        if (m_sub_process[i].m_pid < 0)
                                        {
                                            printf("generate error\n");
                                            m_sub_process[i].m_pid = -1;
                                            continue;
                                        }
                                        else if (m_sub_process[i].m_pid > 0)
                                        {
                                            printf("parent generate new child, index(%d), id(%d)\n", i, m_sub_process[i].m_pid);
                                            close(m_sub_process[i].m_pipefd[1]);
                                            addfd(m_epollfd, m_sub_process[i].m_pipefd[0]);
                                            std::string json_ret;
                                            auto iter = m_task_record.find(i);
                                            if (iter != m_task_record.end())
                                            {
                                                generate_err_json(iter->second->m_payload, "error", "crash", json_ret);
                                                sche->publish_to_mq(json_ret);
                                                printf("parent has sent error msg\n");
                                            }
                                            continue;
                                        }
                                        else
                                        {
                                            // Note:注意先把下标重新赋值了！
                                            m_idx = i;
                                            printf("in new generate child, index(%d), id(%d), m_idx(%d)\n", i, getpid(), m_idx);
                                            close(m_sub_process[i].m_pipefd[0]);
                                            run_child(config);
                                            exit(0);
                                        }
                                    }
                                }
                            }
                        }
                        // 检查所有子进程退出情况，如果所有子进程正常退出，主进程也退出！
                        printf("maybe all child gone!\n");
                        m_stop = true;
                        for (int i = 0; i < m_process_number; i++)
                        {
                            if (m_sub_process[i].m_pid != -1)
                            {
                                m_stop = false;
                                printf("yeah not all child gone!\n");
                                break;
                            }
                        }

                        break;
                    case SIGTERM:
                        printf("parent(%d) recv SIGTERM!\n", getpid());
                    case SIGINT:
                        printf("parent(%d) recv SIGINT!\n", getpid());
                        //如果父进程收到终止信号，杀死所有子进程并等待子进程全部结束
                        //更好的方式是自定义协议通知子进程来退出
                        printf("kill all child now\n");
                        for (int i = 0; i < m_process_number; i++)
                        {
                            int pid = m_sub_process[i].m_pid;
                            if (pid != -1)
                            {
                                printf("kill sending SIGTERM to pid: %d\n", pid);
                                kill(pid, SIGTERM);
                            }
                        }
                        break;
                    case SIGSEGV:
                        printf("parent(%d) recv SIGSEGV!\n", getpid());
                    default:
                        printf("unknow sig:%d\n", sig);
                        break;
                    }
                }
            }
            else if (((index = isChildFd(sockfd)) >= 0) && (events[i].events & EPOLLIN))
            {
                // 这里应该 只能 是子进程发来的数据了吧;
                printf("======parent recv from child, index: %d\n", index);
                int ret = readMsg(sockfd, m_sub_process[index].recv_buff);
                if (ret == -100)
                {
                    log_print(kError, "child sent invalid data!");
                    continue;
                }
                if (ret == -101)
                {
                    log_print(kError, "child may crash!");
                    continue;
                }
                std::string data(m_sub_process[index].recv_buff, ret);
                log_print(kInfo, "parent receive task result from child, ret = %d, data: %s", ret, data.c_str());
                // TODO...接口有点丑陋，后面有机会重构吧
                // ret==0 代表socket连接断开了，也即子进程退出了。
                if (ret != 0)
                {
                    sche->publish_to_mq(data);
                }

                m_sub_process[index].m_state.busy = 0;
            }
        }
    }
    close(m_epollfd);
    delete sche;
}

template <typename T, typename P>
int processpool<T, P>::select_process(SelectMode mode)
{
    if (mode != SelectMode::RandomFree)
    {
        return -1;
    }

    srand(time(0));

    std::vector<int> free_index;
    for (int i = 0; i < m_process_number; ++i)
    {
        if ((m_sub_process[i].m_state.busy == 0) && (m_sub_process[i].m_pid != -1))
        {
            free_index.push_back(i);
        }
    }

    printf("select_process find %d free\n", free_index.size());

    if (free_index.empty())
    {
        return -2;
    }

    int index = rand() % free_index.size();

    printf("select_process select %d\n", free_index[index]);

    return free_index[index];
}

template <typename T, typename P>
int processpool<T, P>::isChildFd(int fd)
{
    for (int i = 0; i < m_process_number; ++i)
    {
        if (m_sub_process[i].m_pipefd[0] == fd)
        {
            return i;
        }
    }
    return -1;
}

template <typename T, typename P>
bool processpool<T, P>::generate_err_json(const std::string &mq, const std::string &status, const std::string &error, std::string &msg)
{
    protocol::algorithmservice::TaskResult task_result;
    protocol::algorithmservice::MqRequest req;
    req.de_serialize(mq, m_config.modules);
    task_result.id = req.m_id;
    task_result.method = req.m_method;
    task_result.backMsg = req.m_backMsg;
    task_result.targetFormat = req.m_targetFormat;
    task_result.status = status;
    task_result.error = error;

    std::string tmp;
    return task_result.serialize(msg, tmp);
}

ssize_t /* Read "n" bytes from a descriptor. */
readn(int fd, void *vptr, size_t n)
{
    size_t nleft;
    ssize_t nread;
    char *ptr;

    ptr = (char *)vptr;
    nleft = n;
    while (nleft > 0)
    {
        if ((nread = read(fd, ptr, nleft)) < 0)
        {
            if (errno == EINTR)
                nread = 0; /* 系统中断，继续读 */
            else
            {
                // NOTE：非阻塞socket一般读到EAGAIN就可以返回了，所有的数据都传到应用层缓冲区，等待应用层的消息反序列化
                // 这里事先读了代表长度的一字节，再读够剩下的，就是一个完整消息了，所以这里不返回。

                // printf("------errno: %d nread:%d \n", errno, nread);
                nread = 0;
                // return (-1);
            }
        }
        else if (nread == 0)
            break; /* EOF 对端close */

        nleft -= nread;
        ptr += nread;
    }
    return (n - nleft); /* return >= 0 */
}

ssize_t /* Write "n" bytes to a descriptor. */
writen(int fd, const void *vptr, size_t n)
{
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;

    ptr = (char *)vptr;
    nleft = n;
    while (nleft > 0)
    {
        if ((nwritten = write(fd, ptr, nleft)) <= 0)
        {
            if (nwritten < 0 && errno == EINTR)
                nwritten = 0; /* and call write() again */
            else
                return (-1); /* error */
        }

        nleft -= nwritten;
        ptr += nwritten;
    }
    return (n);
}

// TODO...丑陋而低效，后面序列化反序列化这块要改下
ssize_t readMsg(int fd, void *vptr)
{
    uint64_t len;
    read(fd, &len, sizeof(uint64_t));
    printf("readMsg recv len:%d\n", len);
    if (len == 0)
    {
        return -101;
    }
    if (len > MAX_BUFF)
        return -100;
    return readn(fd, vptr, len);
}

void sendMsg(int fd, void *vptr, uint64_t len)
{
    send(fd, &len, sizeof(uint64_t), 0);
    send(fd, vptr, len, 0);
}

} // namespace ustd
#endif