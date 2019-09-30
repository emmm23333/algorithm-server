#include <ustd/time_convert.h>
#include <sstream>
#include <iomanip>
namespace ustd
{
std::string get_current_time()
{
    auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::stringstream ss;
    ss << std::put_time(std::localtime(&t), "%Y%m%d%H%M%S");
    return ss.str();
}

} // namespace ustd