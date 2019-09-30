#include "config.h"

namespace algorithmservice
{
namespace business
{
bool get_first_steps(task task, std::vector<std::string> &steps)
{
    steps.clear();
    auto it = task.find("read_op");
    if (it == task.end())
    {
        return false;
    }
    steps = it->second;
    return true;
}

bool get_next_steps(task task, std::string step, std::vector<std::string> &steps)
{
    steps.clear();
    auto it = task.find(step);
    if (it == task.end())
    {
        return false;
    }
    steps = it->second;
    return true;
}

} // namespace business
} // namespace algorithmservice