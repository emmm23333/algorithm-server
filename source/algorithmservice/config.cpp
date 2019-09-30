#include <algorithmservice/config.h>
#include <json_data/file_io.h>
#include <rapidjson/document.h>
#include <rapidjson/pointer.h>
namespace algorithmservice
{
std::string g_config_file;
bool load_from_json(Config &config)
{
    using namespace rapidjson;
    std::string content;
    if (!json_data::read_json(g_config_file, content))
    {
        return false;
    }
    Document d;
    d.Parse(content.c_str());

    // log config
    config.log_config.path = Pointer("/globle/log/path").Get(d)->GetString();
    config.log_config.level = Pointer("/globle/log/level").Get(d)->GetUint64();
    config.log_config.prefix_name = Pointer("/globle/log/prefix").Get(d)->GetString();
    config.log_config.storage_type = Pointer("/globle/log/output").Get(d)->GetUint64();
    config.log_config.alg_log = Pointer("/globle/log/algorithmLog").Get(d)->GetBool();
    config.log_config.alg_log_path = Pointer("/globle/log/algorithmLogPath").Get(d)->GetString();

    // save config
    config.business_config.save_config.msg = Pointer("/globle/savePath/msg").Get(d)->GetString();
    config.business_config.save_config.download = Pointer("/globle/savePath/download").Get(d)->GetString();
    config.business_config.save_config.new_save = Pointer("/globle/savePath/newSave").Get(d)->GetString();

    // mqtt config
    config.business_config.maqtt_config.host = Pointer("/globle/mqtt/host").Get(d)->GetString();
    config.business_config.maqtt_config.port = Pointer("/globle/mqtt/port").Get(d)->GetInt();
    config.business_config.maqtt_config.username = Pointer("/globle/mqtt/username").Get(d)->GetString();
    config.business_config.maqtt_config.password = Pointer("/globle/mqtt/password").Get(d)->GetString();
    config.business_config.maqtt_config.clean_start = Pointer("/globle/mqtt/cleanStart").Get(d)->GetBool();
    config.business_config.maqtt_config.client_id = Pointer("/globle/mqtt/clientId").Get(d)->GetString();
    config.business_config.maqtt_config.recieve = Pointer("/globle/mqtt/receive").Get(d)->GetString();
    config.business_config.maqtt_config.send = Pointer("/globle/mqtt/send").Get(d)->GetString();

    // control config
    config.business_config.control_config.processes = Pointer("/globle/control/processes").Get(d)->GetInt();
    config.business_config.control_config.cpu_rate = Pointer("/globle/control/cpu").Get(d)->GetInt();
    config.business_config.control_config.net_interval = Pointer("/globle/control/netRetryInterval").Get(d)->GetInt();
    config.business_config.control_config.net_retries = Pointer("/globle/control/netRetries").Get(d)->GetInt();
    config.business_config.control_config.part_size = Pointer("/globle/control/uploadPartSize").Get(d)->GetUint64();
    config.business_config.control_config.thread_num = Pointer("/globle/control/uploadThreadNum").Get(d)->GetUint();

    // oss config
    auto buckets = Pointer("/globle/oss").Get(d)->GetObject();
    for (auto &bucket : buckets)
    {
        printf("bucket name: %s\n", bucket.name.GetString());
        business::BucketInfo info;
        for (auto &item : bucket.value.GetObject())
        {
            std::string name(item.name.GetString());
            printf("info name: %s\n", name.c_str());
            if (name == "endpoint")
            {
                info.end_point = item.value.GetString();
                printf("info value: %s\n", item.value.GetString());
            }
            if (name == "accessKey")
            {
                info.access_key = item.value.GetString();
                printf("info value: %s\n", item.value.GetString());
            }
            if (name == "accessSecret")
            {
                info.access_secret = item.value.GetString();
                printf("info value: %s\n", item.value.GetString());
            }
        }
        config.business_config.oss_config.buckets.insert(std::make_pair(bucket.name.GetString(), info));
    }

    // modules
    // readOp
    config.business_config.modules.readop.format = Pointer("/modules/readOp/format").Get(d)->GetString();
    config.business_config.modules.readop.is_zip = Pointer("/modules/readOp/isZip").Get(d)->GetBool();

    // 3mfOp
    config.business_config.modules.mfop.with_color = Pointer("/modules/3mfOp/withColor").Get(d)->GetBool();
    config.business_config.modules.mfop.with_texture = Pointer("/modules/3mfOp/withTexture").Get(d)->GetBool();

    // objOp
    config.business_config.modules.objop.read_color_if_exists = Pointer("/modules/objOp/readVColorIfExists").Get(d)->GetBool();

    // simplify
    config.business_config.modules.simplify.remain_rate = Pointer("/modules/simplify/remainRate").Get(d)->GetFloat();
    config.business_config.modules.simplify.boundary_weight = Pointer("/modules/simplify/boundaryWeight").Get(d)->GetDouble();
    config.business_config.modules.simplify.is_check_face_flip = Pointer("/modules/simplify/isCheckFaceFlip").Get(d)->GetBool();
    config.business_config.modules.simplify.is_feature_sensitive = Pointer("/modules/simplify/isFeatureSensitive").Get(d)->GetBool();

    // smooth
    config.business_config.modules.smooth.move_dist = Pointer("/modules/smooth/moveDist").Get(d)->GetFloat();
    config.business_config.modules.smooth.iter = Pointer("/modules/smooth/iter").Get(d)->GetInt();

    // saveGlb
    config.business_config.modules.saveglb.writeASCII = Pointer("/modules/saveGlb/writeASCII").Get(d)->GetBool();
    config.business_config.modules.saveglb.need_upload = Pointer("/modules/saveGlb/needUpload").Get(d)->GetBool();
    config.business_config.modules.saveglb.pub_bucket = Pointer("/modules/saveGlb/pubBucket").Get(d)->GetBool();

    // saveStl
    config.business_config.modules.savestl.need_upload = Pointer("/modules/saveStl/needUpload").Get(d)->GetBool();
    config.business_config.modules.savestl.pub_bucket = Pointer("/modules/saveStl/pubBucket").Get(d)->GetBool();

    // savePly
    config.business_config.modules.saveply.need_upload = Pointer("/modules/savePly/needUpload").Get(d)->GetBool();
    config.business_config.modules.saveply.pub_bucket = Pointer("/modules/savePly/pubBucket").Get(d)->GetBool();
    config.business_config.modules.saveply.writeASCII = Pointer("/modules/savePly/writeASCII").Get(d)->GetBool();

    // saveBeb
    config.business_config.modules.savebeb.need_upload = Pointer("/modules/saveBeb/needUpload").Get(d)->GetBool();
    config.business_config.modules.savebeb.pub_bucket = Pointer("/modules/saveBeb/pubBucket").Get(d)->GetBool();

    // save3mf
    config.business_config.modules.save3mf.need_upload = Pointer("/modules/save3mf/needUpload").Get(d)->GetBool();
    config.business_config.modules.save3mf.pub_bucket = Pointer("/modules/save3mf/pubBucket").Get(d)->GetBool();

    // saveObj
    config.business_config.modules.saveobj.write_v_color_if_exists = Pointer("/modules/saveObj/writeVColorIfExists").Get(d)->GetBool();
    config.business_config.modules.saveobj.need_upload = Pointer("/modules/saveObj/needUpload").Get(d)->GetBool();
    config.business_config.modules.saveobj.pub_bucket = Pointer("/modules/saveObj/pubBucket").Get(d)->GetBool();

    // SaveZip
    config.business_config.modules.savezip.relpace_zip = Pointer("/modules/saveZip/replaceZip").Get(d)->GetBool();
    config.business_config.modules.savezip.need_upload = Pointer("/modules/saveZip/needUpload").Get(d)->GetBool();
    config.business_config.modules.savezip.pub_bucket = Pointer("/modules/saveZip/pubBucket").Get(d)->GetBool();

    // repair
    config.business_config.modules.repair.max_retries = Pointer("/modules/repair/maxRetries").Get(d)->GetInt();

    // pngFile
    business::PngFile pngfile;
    pngfile.width = Pointer("/modules/pngFile/width").Get(d)->GetInt();
    pngfile.height = Pointer("/modules/pngFile/height").Get(d)->GetInt();
    pngfile.longitude = Pointer("/modules/pngFile/longitude").Get(d)->GetFloat();
    pngfile.latitude = Pointer("/modules/pngFile/latitude").Get(d)->GetFloat();
    pngfile.need_upload = Pointer("/modules/pngFile/needUpload").Get(d)->GetBool();
    pngfile.pub_bucket = Pointer("/modules/pngFile/pubBucket").Get(d)->GetBool();
    config.business_config.modules.pngfiles.push_back(pngfile);

    // flow
    auto objs = Pointer("/flow").Get(d)->GetObject();
    for (auto &flow : objs)
    {
        printf("flow obj name: %s\n", flow.name.GetString());
        std::map<std::string, std::vector<std::string>> task;
        for (auto &tasks : flow.value.GetObject())
        {
            printf("task name: %s\n", tasks.name.GetString());
            std::vector<std::string> steps;
            for (auto &step : tasks.value.GetArray())
            {
                printf("step name: %s\n", step.GetString());
                steps.push_back(step.GetString());
            }
            task.insert(std::make_pair(tasks.name.GetString(), steps));
        }
        config.business_config.flow.insert(std::make_pair(flow.name.GetString(), task));
        // config.business_config.flow.insert
    }

#ifdef TEST_STEPS
    std::vector<std::string> test_vector;
    business::get_first_steps(config.business_config.flow["preview_convert"], test_vector);
    for (auto &n : test_vector)
    {
        printf("get first steps: %s\n", n.c_str());
    }
    business::get_next_steps(config.business_config.flow["preview_convert"], "repair", test_vector);
    for (auto &n : test_vector)
    {
        printf("get next steps: %s\n", n.c_str());
    }
#endif
    return true;
}

bool load_config(const std::string filename, Config &config)
{
    g_config_file = filename;
    if (!load_from_json(config))
    {
        printf("load config from json failed.\n");
        return false;
    }
    return true;
}
} // namespace algorithmservice