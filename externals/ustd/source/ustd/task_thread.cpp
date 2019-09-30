#include <ustd/task_thread.h>

namespace ustd
{
TaskThread::TaskThread()
{
}

TaskThread::~TaskThread()
{
}

void TaskThread::begin(int thread_number)
{
    for (int i = 0; i < thread_number; i++)
    {
        threads_.emplace_back(&TaskThread::thread_proc, this);
    }
}

void TaskThread::end()
{
    queue_.notify_exit();
    for (auto &thread : threads_)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
}

void TaskThread::post_task(const std::function<void()> &task)
{
    queue_.post_task(std::function<void()>(task));
}

void TaskThread::post_task(std::function<void()> &&task)
{
    queue_.post_task(task);
}

int TaskThread::task_count() const
{
    return queue_.task_count();
}

void TaskThread::set_timer(
    uint32_t id,
    uint32_t elapse,
    const std::function<void(uint32_t)> &on_timer)
{
    queue_.set_timer(id, elapse, on_timer);
}

void TaskThread::kill_timer(uint32_t id)
{
    queue_.kill_timer(id);
}

void TaskThread::thread_proc()
{
    queue_.do_while();
}
} // namespace ustd
