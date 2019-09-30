#ifndef ALGORITHMSERVICE_SYSTEM_HELPER_H_
#define ALGORITHMSERVICE_SYSTEM_HELPER_H_
#include <string>
#include <vector>
namespace algorithmservice
{
namespace systemhelper
{
//随机生成个uuid
std::string create_uuid();

bool execute_cmd(const std::string &cmd, std::vector<std::string> &results);
bool execute_cmd(const std::string &cmd, std::string &result);

} // namespace systemhelper
} // namespace algorithmservice

#endif // ALGORITHMSERVICE_SYSTEM_HELPER_H_