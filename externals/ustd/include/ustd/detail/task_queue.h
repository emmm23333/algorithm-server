#ifndef USTD_DETAIL_TASK_QUEUE_H_
#define USTD_DETAIL_TASK_QUEUE_H_

#include <queue>

namespace ustd
{
namespace detail
{
template <typename TaskT>
class TaskQueue
{
public:
    TaskQueue() = default;
    ~TaskQueue() = default;
    TaskQueue(TaskQueue &&other) = delete;
    TaskQueue &operator=(TaskQueue &&other) = delete;
    TaskQueue(const TaskQueue &) = delete;
    TaskQueue &operator=(const TaskQueue &) = delete;

    void push(const TaskT &task)
    {
        tasks_.push(task);
    }

    void push(TaskT &&task)
    {
        tasks_.push(task);
    }

    bool pop(TaskT &task)
    {
        if (!tasks_.empty())
        {
            task = tasks_.front();
            tasks_.pop();
            return true;
        }
        else
        {
            return false;
        }
    }

    bool empty() const
    {
        return tasks_.empty();
    }

    int size() const
    {
        return tasks_.size();
    }

private:
    std::queue<TaskT> tasks_;
};

} // namespace detail
} // namespace ustd

#endif // USTD_DETAIL_TASK_QUEUE_H_
