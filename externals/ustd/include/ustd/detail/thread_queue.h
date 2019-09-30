#ifndef USTD_DETAIL_THREAD_QUEUE_H_
#define USTD_DETAIL_THREAD_QUEUE_H_

#include <mutex>
#include <condition_variable>
#include <ustd/detail/timer_queue.h>

namespace ustd
{
namespace detail
{
template <typename TaskT, template <typename> class TaskQueueT>
class ThreadQueue
{
public:
    ThreadQueue() = default;
    ~ThreadQueue() = default;
    ThreadQueue(ThreadQueue &&other);
    ThreadQueue &operator=(ThreadQueue &&other);
    ThreadQueue(const ThreadQueue &) = delete;
    ThreadQueue &operator=(const ThreadQueue &) = delete;

    void post_task(const TaskT &task)
    {
        mutex_.lock();
        task_queue_.push(task);
        mutex_.unlock();
        condition_.notify_one();
    }

    void post_task(TaskT &&task)
    {
        mutex_.lock();
        task_queue_.push(task);
        mutex_.unlock();
        condition_.notify_one();
    }

    int task_count() const
    {
        std::lock_guard<std::mutex> guard(mutex_);
        return task_queue_.size();
    }

    void set_timer(
        uint32_t id,
        uint32_t elapse /* milliseconds */,
        const std::function<void(uint32_t)> &on_timer)
    {
        mutex_.lock();
        timer_queue_.set_timer(id, elapse, on_timer);
        mutex_.unlock();
        condition_.notify_one();
    }

    void kill_timer(uint32_t id)
    {
        std::lock_guard<std::mutex> guard(mutex_);
        timer_queue_.kill_timer(id);
    }

public:
    void do_while()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while (!exit_flag)
        {
            if (task_queue_.empty())
            {
                uint64_t t = timer_queue_.get_wait_time();
                condition_.wait_for(lock, std::chrono::milliseconds(t));
            }
            if (exit_flag)
                break;
            do_one_timer(lock);
            if (exit_flag)
                break;
            do_one_task(lock);
            if (exit_flag)
                break;
        }
    }

    void notify_exit()
    {
        mutex_.lock();
        exit_flag = true;
        mutex_.unlock();
        condition_.notify_all();
    }

private:
    void do_one_timer(std::unique_lock<std::mutex> &lock)
    {
        uint32_t id;
        std::function<void(uint32_t)> on_timer;
        if (timer_queue_.peek(id, on_timer))
        {
            lock.unlock();
            on_timer(id);
            lock.lock();
        }
    }

    void do_one_task(std::unique_lock<std::mutex> &lock)
    {
        TaskT task;
        if (task_queue_.pop(task))
        {
            lock.unlock();
            task();
            lock.lock();
        }
    }

private:
    TaskQueueT<TaskT> task_queue_;
    detail::TimerQueue timer_queue_;
    mutable std::mutex mutex_;
    std::condition_variable condition_;
    bool exit_flag = false;
};
} // namespace detail
} // namespace ustd

#endif // USTD_DETAIL_THREAD_QUEUE_H_
