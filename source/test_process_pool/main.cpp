#include "process_pool.h"
#include "task_handle.h"
#include <algorithmservice/config.h>
#include <algorithmservice/option_parser.h>
#include <common/console_app.h>
#include <business/module_api.h>
#include <business/config.h>
#include <ustd/log.h>
#include <ustd/file_system.h>
int main(int argc, char *argv[])
{
    using namespace algorithmservice::business;
    std::string filename;
    algorithmservice::CommandArgument arg;
    if (algorithmservice::parse_options(argc, argv, arg) == 0)
    {
        return 0;
    }
    filename = arg.config_filename;
    algorithmservice::Config config;
    if (!algorithmservice::load_config(filename, config))
    {
        printf("load from json fail.\n");
        return -1;
    }

    printf("log config path: %s\n", config.log_config.path.c_str());
    ustd::processpool<TaskHandler, ScheHandler> *pool = ustd::processpool<TaskHandler, ScheHandler>::create(4);
    if (pool)
    {
        pool->run(config.business_config);
    }
    return 0;
}