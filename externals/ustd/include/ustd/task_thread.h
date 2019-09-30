#ifndef USTD_TASK_THREAD_H_
#define USTD_TASK_THREAD_H_

#include <thread>
#include <ustd/detail/task_queue.h>
#include <ustd/detail/thread_queue.h>

namespace ustd
{
class TaskThread
{
public:
    TaskThread();
    ~TaskThread();
    TaskThread(TaskThread &&) = delete;
    TaskThread &operator=(TaskThread &&) = delete;
    TaskThread(const TaskThread &) = delete;
    TaskThread &operator=(const TaskThread &) = delete;

    void begin(int thread_number = 1);
    void end();

    void post_task(const std::function<void()> &task);
    void post_task(std::function<void()> &&task);
    int task_count() const;
    void set_timer(uint32_t id, uint32_t elapse /* milliseconds */,
                   const std::function<void(uint32_t)> &on_timer);
    void kill_timer(uint32_t id);

private:
    void thread_proc();

private:
    std::vector<std::thread> threads_;
    detail::ThreadQueue<std::function<void()>, detail::TaskQueue> queue_;
};
} // namespace ustd

#endif // USTD_TASK_THREAD_H_