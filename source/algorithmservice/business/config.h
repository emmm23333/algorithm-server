#ifndef ALGORITHMSERVICE_BUSINESS_CONFIG_H_
#define ALGORITHMSERVICE_BUSINESS_CONFIG_H_

#include <string>
#include <vector>
#include <map>
#include <stdint.h>

#include "config_detail.h"
namespace algorithmservice
{
namespace business
{

using task = std::map<std::string, std::vector<std::string>>;

struct Config
{
    SaveConfig save_config;
    MqttConfig maqtt_config;
    ControlConfig control_config;
    OssConfig oss_config;
    Modules modules;
    std::map<std::string, task> flow;
};

bool get_first_steps(task, std::vector<std::string> &);
bool get_next_steps(task, std::string, std::vector<std::string> &);

} // namespace business
} // namespace algorithmservice
#endif