#ifndef ALGORITHMSERVICE_CONFIG_H_
#define ALGORITHMSERVICE_CONFIG_H_

#include <string>
#include <business/config.h>
namespace algorithmservice
{
struct LogConfig
{
    std::string path;
    std::string prefix_name;
    int level;
    int storage_type;
    bool alg_log;
    std::string alg_log_path;
};

struct Config
{
    LogConfig log_config;
    business::Config business_config;
};

bool load_from_json(Config &config);
bool load_config(const std::string filename, Config &config);
} // namespace algorithmservice

#endif // ALGORITHMSERVICE_CONFIG_H_