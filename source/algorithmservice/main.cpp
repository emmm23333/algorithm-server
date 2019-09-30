#include <algorithmservice/config.h>
#include <algorithmservice/option_parser.h>
#include <Sn3DAlgorithm/common/baseTypeIO/baseTypeIO.h>
#include <Sn3DAlgorithm/common/baseType/TriMesh.h>
#include <Sn3DAlgorithm/common/baseType/DataArchiveConfig.h>
#include <common/console_app.h>
#include <business/module_api.h>
#include <business/config.h>
#include <ustd/log.h>
#include <ustd/file_system.h>
bool algorithmservice_init(const algorithmservice::business::Config &config)
{
    // test algorithm
    // using namespace Sn3DAlgorithm;
    // TriMesh mesh;
    // int ret = read_mesh("/Users/1", mesh);
    // printf("init! %d\n", ret);

    // init
    algorithmservice::business::init(config);

    return true;
}

int main(int argc, char *argv[])
{
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
    if (!ustd::is_exists_directory(config.log_config.path))
    {
        bool ret = ustd::create_directories(config.log_config.path);
        printf("not exist,create: %d,errno:%d\n", ret, errno);
    }

    if (config.log_config.alg_log)
    {
        printf("enable algorithm log, path: %s\n", config.log_config.alg_log_path.c_str());
        if (!ustd::is_exists_directory(config.log_config.alg_log_path))
        {
            bool ret = ustd::create_directories(config.log_config.alg_log_path);
            printf("not exist,create: %d,errno:%d\n", ret, errno);
        }
        gDAConfig.set_enable(true);
        gDAConfig.set_write_mode(true);
        gDAConfig.set_store_path(config.log_config.alg_log_path);
        gDAConfig.set_time_stamp(true);
    }

    ustd::log::set_directory(config.log_config.path.c_str());
    ustd::log::set_prefix_name(config.log_config.prefix_name.c_str());
    ustd::log::set_level(ustd::log::kLevelMask & ~config.log_config.level);
    ustd::log::set_storage_type(ustd::log::kStorageTypeMask &
                                ~config.log_config.storage_type);
    ustd::log::set_reserve_days(7);

    auto app = std::make_shared<common::ConsoleApp>("algorithm service");
    app->start(std::bind(&algorithmservice_init, config.business_config),
               0,
               0);
    app->run();
    return 0;
}