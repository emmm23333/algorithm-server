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
#include <iostream>
#include <string>
#include <vector>

#include <sys/signalfd.h>

static int setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

int main()
{
    int m_index = -1;
    int efd_ = epoll_create(20);
    std::vector<int> listen_signals{SIGHUP, SIGINT, SIGTERM, SIGSEGV, SIGABRT, SIGCHLD, SIGPIPE};
    sigset_t cur_signals;
    int signal_fd;
    ::sigemptyset(&cur_signals);
    for (int signal : listen_signals)
    {
        sigaddset(&cur_signals, signal);
    }
    pthread_sigmask(SIG_BLOCK, &cur_signals, NULL);
    signal_fd = ::signalfd(-1, &cur_signals, SFD_NONBLOCK);
    setnonblocking(signal_fd);

    epoll_event tev;
    memset(&tev, 0, sizeof(tev));
    tev.events = EPOLLIN | EPOLLET;
    tev.data.fd = signal_fd;

    epoll_ctl(efd_, EPOLL_CTL_ADD, signal_fd, &tev);

    while (1)
    {
        for (int i = 0; i < 3; ++i)
        {
            int child_pid_ = fork();

            if (child_pid_ == 0)
            {
                m_index = i;
                break;
            }
            else
            {
                m_index = -1;
                continue;
            }
        }

        if (m_index == -1)
        {
            printf("parent!\n");
            epoll_event event[1000];
            memset(&event, 0, sizeof(event));
            while (1)
            {
                int number;
                printf("parent before epoll!\n");

                number = epoll_wait(efd_, event, 1, -1);

                sigprocmask(SIG_BLOCK, &cur_signals, NULL);

                printf("parent after epoll!\n");
                if ((number < 0) && (errno != EINTR))
                {
                    printf("epoll faliure\n");
                    break;
                }

                for (int i = 0; i < number; i++)
                {
                    int sockfd = event[i].data.fd;
                    printf("parent recv socketfd; %d\n", sockfd);

                    if (sockfd == signal_fd)
                    {
                        struct signalfd_siginfo info;
                        int ret = -2;

                        ret = read(sockfd, &info, sizeof(info));
                        printf("parent recv signal, ret: %d\n", ret);

                        // sigprocmask(SIG_BLOCK, &cur_signals, NULL);

                        if (ret <= 0)
                        {
                            // printf("parent recv signal error, ret: %d\n", ret);
                            continue;
                        }
                        else
                        {
                            while (1)
                            {
                                int status;
                                pid_t pid = waitpid(-1, &status, WNOHANG);
                                if (pid <= 0)
                                {
                                    break;
                                }
                                else
                                {
                                    printf("get %d down\n", pid);
                                }

                                // something happened with child 'pid', do something about it...
                                // Details are in 'status', see waitpid() manpage
                            }
                            unsigned sig = info.ssi_signo;
                            printf("sig: %d\n", sig);
                        }
                    }
                }
            }
            return 0;
        }
        else
        {
            printf("child!\n");
            close(signal_fd);
            close(efd_);

            while (1)
            {
                sleep(2);
            }
        }
    }
    return 0;
}