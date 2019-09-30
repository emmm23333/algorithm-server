#include <vector>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <sys/socket.h>
#include <protocol/algorithmservice/mosquitto_message.h>
#include <protocol/algorithmservice/serialize.h>
#include <business/algorithm_functions.h>
#include <ustd/log.h>
#include <ustd/path_string.h>
#include <ustd/time_convert.h>
#include <ustd/file_system.h>
#include <ustd/string.h>
#include <business/task_handle.h>
#include <common/system_helper.h>
namespace algorithmservice
{
namespace business
{

void MqttClient::on_subscribe(int mid, int qos_count, const int *granted_qos)
{
    log_print(kDebug, "订阅mid：%d", mid);
}

void MqttClient::on_connect(int rc)
{
    log_print(kDebug, "connect mq rc: %d", rc);
    subscribe(NULL, m_config.recieve.c_str(), 1);
}

void MqttClient::on_message(const struct mosquitto_message *message)
{
    bool res = true;
    // mosqpp::topic_matches_sub(g_subTopic.c_str(), message->topic, &res);
    if (res)
    {
        std::string strRcv = (char *)message->payload;
        log_print(kDebug, "receive message from topic %s: %s", message->topic, strRcv.c_str());
        on_message_callback_(message);
    }
}

void MqttClient::set_on_message_callback(const std::function<int(const struct mosquitto_message *message)> func)
{
    on_message_callback_ = func;
}

void MqttClient::set_config(algorithmservice::business::MqttConfig &config)
{
    m_config = config;
}

bool TaskHandler::init(int epollfd, int mqfd, const algorithmservice::business::Config &config)
{
    std::cout << "int task handler init" << std::endl;
    m_epollfd = epollfd;
    m_mqfd = mqfd;
    m_config = config;

    oss_handle_ = std::make_shared<OssHandle>(m_config.control_config);
    algorithm_funcs_ = std::make_shared<AlgorithmFunctions>();

    task_thread_ = std::make_shared<ustd::TaskThread>();
    task_thread_->begin(4);
    m_upcounts = 0;
    return true;
}

bool TaskHandler::uninit()
{
    task_thread_->end();
    return true;
}

bool TaskHandler::process(std::shared_ptr<protocol::algorithmservice::mosquitto_message> msg, std::string &result)
{
    std::cout << "in task process" << std::endl;
    log_print(kDebug, "recv mid:%d, qos:%d, retain:%d, topic:%s", msg->m_mid, msg->m_qos, msg->m_retain, msg->m_topic.data());
    protocol::algorithmservice::MqRequest req;
    req.de_serialize(msg->m_payload, m_config.modules);
    log_print(kDebug, "process(%d) after de_serialize, method: %s\n", getpid(), req.m_method.c_str());

    // 序列化任务到tmp/task。进程池那里序列化比较好，但是不想那里获取消息的id
    if (!ustd::is_exists_directory(m_config.save_config.msg))
    {
        ustd::create_directory(m_config.save_config.msg);
    }

    protocol::algorithmservice::BoostBinarySerializeToFile(msg, ustd::compose_path(m_config.save_config.msg, req.m_id));

    protocol::algorithmservice::TaskResult task_result;
    task_result.id = req.m_id;
    task_result.method = req.m_method;
    task_result.backMsg = req.m_backMsg;
    task_result.targetFormat = req.m_targetFormat;
    task_result.status = "success";

    std::string save_path = ustd::compose_path(m_config.save_config.new_save.c_str(), (req.m_method + "_" + ustd::get_current_time()).c_str());
    std::string file_path, file_name;
    if (!ustd::is_exists_directory(save_path) && !ustd::create_directories(save_path))
    {
        task_result.status = "error";
        task_result.error = "create save path error";
        log_print(kError, "create upload path: %s error", save_path.c_str());
        goto send;
    }

    if (!download_file(req, file_path, file_name))
    {
        task_result.status = "error";
        task_result.error = "download error";
        goto send;
    }

    if (req.m_params.config.readop.is_zip)
    {
        log_print(kDebug, "is zip, unziping...");
        // std::string cmd("unzip -o -d " + save_path + " " + ustd::compose_path(file_path.c_str(), file_name.c_str()));
        std::string cmd("unzip -f " + ustd::compose_path(file_path.c_str(), file_name.c_str()) + " -o " + save_path);
        std::vector<std::string> results;
        if (!systemhelper::execute_cmd(cmd, results))
        {
            log_print(kError, "unzip failed.");
        }
        for (auto &n : results)
        {
            if (n.find("err") != std::string::npos)
            {
                task_result.status = "error";
                task_result.error = "unzip error";
                goto send;
            }
        }

        auto func = [this, &req, &task_result](const std::string &path, bool is_dir) -> bool {
            log_print(kTrace, "traversing directory, find: %s, is dir: %d", path.c_str(), is_dir);
            std::vector<std::string> slices;
            ustd::split_string(path, ".", slices);
            // printf("==== %s\n", req.m_params.config.readop.format.c_str());
            auto it = std::find(slices.begin(), slices.end(), req.m_params.config.readop.format);
            if (!is_dir && (it != slices.end()))
            {
                log_print(kDebug, "calling read_mesh_ex, parent_path:%s, file:%s", ustd::get_parent_path(path).c_str(), ustd::get_last_path(path).c_str());
                algorithm_funcs_->read_mesh_ex(ustd::get_parent_path(path), ustd::get_last_path(path), ustd::get_parent_path(path), req, m_config);
                algorithm_funcs_->serialize_result(task_result);
                algorithm_funcs_->clear();
                if (req.m_params.config.savezip.relpace_zip)
                {
                    log_print(kDebug, "deleting %s", path.c_str());
                    ustd::delete_file(path);
                }
            }
            return true;
        };
        ustd::traverse_directory(save_path, func);

        // 遍历目录之后，打包整个下载路径，生成dfsid和task_result
        std::string zip_name = ustd::compose_path(save_path.c_str(), file_name.c_str());

        // std::string cmd_zip("cd " + save_path + " && zip -r " + file_name + " " + ustd::trim_extension(file_name) + " && cd -");
        std::string cmd_zip("cd " + save_path + " && zip -r " + file_name + " " + "./" + " && cd -");

        std::vector<std::string> results_zip;
        if (!systemhelper::execute_cmd(cmd_zip, results))
        {
            log_print(kError, "zip failed.");
            return false;
        }

        std::string dfs_id(req.m_params.oss.dfsId + "_out" + "/saveZip/" + ustd::trim_extension(file_name) + "_" + systemhelper::create_uuid() + ".zip");
        log_print(kDebug, "save zip dfsId: %s", dfs_id.c_str());

        protocol::algorithmservice::SingleFile sf;
        sf.dfsId = dfs_id;
        sf.bucket = req.m_params.config.savezip.need_upload ? req.m_params.oss.bucket.data : req.m_params.oss.bucket.pub;
        // printf("========= zip upload bucket: %s\n", sf.bucket.c_str());
        sf.isPub = !req.m_params.config.savezip.need_upload;
        auto itt = m_config.oss_config.buckets.find(sf.bucket);
        if (m_config.oss_config.buckets.end() == itt)
        {
            log_print(kError, "can not find bucket %s in config", sf.bucket.c_str());
            return false;
        }
        sf.endpoint = itt->second.end_point;
        sf.size = ustd::get_file_size(zip_name);
        sf.file_path = save_path;
        sf.file_name = file_name;

        // TODO...看着就丑

        task_result.fileResult.saveResult.insert(std::make_pair("saveZip", std::vector<protocol::algorithmservice::SingleFile>{sf}));
    }
    else
    {
        algorithm_funcs_->read_mesh_ex(file_path, file_name, save_path, req, m_config);
        algorithm_funcs_->serialize_result(task_result);
        algorithm_funcs_->clear();
    }

    algorithm_funcs_->m_err_msg.clear();
    if (!upload_files(req, task_result.fileResult.saveResult))
    {
        task_result.status = "error";
        task_result.error = "upload timeout";
    }
    // 序列化task_result发送给主进程

send:
    std::string error_msg;

    if (!task_result.serialize(result, error_msg))
    {
        log_print(kError, "task_result serialize error: %s", error_msg.c_str());
        return false;
    }

    log_print(kInfo, "task_result serialize success: %s", result.c_str());

    // 清理下载和生成的文件
    if (!file_path.empty())
        ustd::delete_directories(file_path);
    if (!save_path.empty())
        ustd::delete_directories(save_path);
    ustd::delete_file(ustd::compose_path(m_config.save_config.msg, req.m_id));

    return true;
}

int TaskHandler::send_to_parent(std::string jsonstr)
{
    uint64_t length = jsonstr.length();
    // printf("send_msg send: %d bytes\n", length);
    send(m_mqfd, &length, sizeof(uint64_t), 0);
    send(m_mqfd, jsonstr.data(), jsonstr.length(), 0);
}

bool TaskHandler::upload_files(protocol::algorithmservice::MqRequest &request, std::map<std::string, std::vector<protocol::algorithmservice::SingleFile>> &save_results)
{
    std::atomic<bool> b_upload(true);
    for (auto &it : save_results)
    {
        for (auto &n : it.second)
        {
            OssInfo oss_info;
            ObjectInfo obj_info;
            if (!n.isPub)
            {
                auto it = m_config.oss_config.buckets.find(request.m_params.oss.bucket.data);
                if (m_config.oss_config.buckets.end() == it)
                {
                    log_print(kError, "can not find bucket %s in config", request.m_params.oss.bucket.data.c_str());
                    return false;
                }
                oss_info = {it->second.access_key, it->second.access_secret, it->second.end_point};
                obj_info = {request.m_params.oss.bucket.data, n.dfsId, n.file_path, n.file_name};
            }
            else
            {
                auto it = m_config.oss_config.buckets.find(request.m_params.oss.bucket.pub);
                if (m_config.oss_config.buckets.end() == it)
                {
                    log_print(kError, "can not find bucket %s in config", request.m_params.oss.bucket.pub.c_str());
                    return false;
                }
                oss_info = {it->second.access_key, it->second.access_secret, it->second.end_point};
                obj_info = {request.m_params.oss.bucket.pub, n.dfsId, n.file_path, n.file_name};
            }

            // if (!oss_handle_->upload(oss_info, obj_info))
            // {
            //     log_print(kError, "upload failed.");
            //     // return false;
            // }

            // TODO...应该做个超时
            m_upcounts++;
            task_thread_->post_task([this, oss_info, obj_info, &b_upload]() {
                b_upload = oss_handle_->upload(oss_info, obj_info);
                m_upcounts--;
            });
        }
    }

    int count = 0;
    while (m_upcounts != 0)
    {
        // log_print(kError, "waiting uploads");
        sleep(1);
        if (++count == 500)
        {
            return false;
        }
    }

    if (!b_upload)
    {
        return false;
    }

    log_print(kDebug, "all has been uploaded!");
    return true;
}

bool TaskHandler::download_file(protocol::algorithmservice::MqRequest &request, std::string &file_path, std::string &file_name)
{
    // if (m_config.flow.find(request.m_method) == m_config.flow.end())
    // {
    //     log_print(kError, "receive invalid method");
    //     return false;
    // }

    auto it = m_config.oss_config.buckets.find(request.m_params.oss.bucket.data);
    if (m_config.oss_config.buckets.end() == it)
    {
        log_print(kError, "can not find bucket %s in config", request.m_params.oss.bucket.data.c_str());
        return false;
    }

    OssInfo oss_info = {it->second.access_key, it->second.access_secret, it->second.end_point};
    std::string save_path = ustd::compose_path(m_config.save_config.download.c_str(), (request.m_method + "_" + ustd::get_current_time()).c_str());
    if (!ustd::create_directories(save_path))
    {
        log_print(kError, "create download directory failed");
        return false;
    }
    ObjectInfo obj_info = {request.m_params.oss.bucket.data, request.m_params.oss.dfsId, save_path, ""};
    if (!oss_handle_->download(oss_info, obj_info))
    {
        return false;
    }
    file_path = obj_info.FilePath;
    file_name = obj_info.FileName;
    return true;
}

bool ScheHandler::init(int epollfd, int mqfd, const algorithmservice::business::Config &config)
{
    std::cout << "int sche init" << std::endl;
    m_epollfd = epollfd;
    m_mqfd = mqfd;
    m_config = config;

    mosqpp::lib_init();
    int rc;

    mq_client = std::make_shared<MqttClient>(config.maqtt_config.client_id.c_str(), true);
    mq_client->set_config(m_config.maqtt_config);
    mq_client->username_pw_set("testuser", "testpassword");
    mq_client->set_on_message_callback(std::bind(&ScheHandler::send_msg, this, std::placeholders::_1));
    log_print(kDebug, "connect to mqtt host: %s port: %d\n", m_config.maqtt_config.host.c_str(), m_config.maqtt_config.port);
    rc = mq_client->connect(m_config.maqtt_config.host.c_str(), m_config.maqtt_config.port);
    char err[1024];
    if (rc == MOSQ_ERR_ERRNO)
        std::cout << "连接错误:" << mosqpp::strerror(rc) << std::endl;
    else if (MOSQ_ERR_SUCCESS == rc)
    {
        rc = mq_client->loop();
        if (MOSQ_ERR_SUCCESS == rc)
        {
            std::cout << "after one mq loop" << std::endl;
        }

        // sleep(1);
        // mq_client->subscribe(NULL, m_config.maqtt_config.recieve.c_str(), 1);
        // TODO...多个topic mqtt返回 父子进程那里通信有问题。后面再查吧。。。
        // mq_client->subscribe(NULL, m_config.maqtt_config.send.c_str());
        rc = mq_client->loop_start();
        std::cout << "mq rc = " << rc << std::endl;
    }
    // std::cout << "cleanup!" << std::endl;
    // mosqpp::lib_cleanup();

    return true;
}

void ScheHandler::process()
{
    std::cout << "in sche process" << std::endl;
}

int ScheHandler::send_msg(const struct mosquitto_message *message)
{
    log_print(kDebug, "in send_msg: mid:%d qos:%d retain:%d payloadLen:%d topic:%s", message->mid, message->qos, message->retain, message->payloadlen, message->topic);
    // TODO...后面可以用共享内存做。
    auto msg = std::make_shared<protocol::algorithmservice::mosquitto_message>(message->mid, std::string(message->topic), std::string((char *)message->payload, message->payloadlen), message->qos, message->retain);

    std::string binary_msg = protocol::algorithmservice::BoostBinarySerialize<protocol::algorithmservice::mosquitto_message>(msg);

    uint64_t length = binary_msg.length();
    // printf("send_msg send: %d\n", length);
    send(m_mqfd, &length, sizeof(uint64_t), 0);
    send(m_mqfd, binary_msg.data(), binary_msg.length(), 0);
    // send(m_mqfd, (const void *)message, sizeof(*message), 0);
    //  send(m_mqfd, (const void *)os.str().data(), os.str().length(), 0);
    return 0;
}

void ScheHandler::publish_to_mq(std::string data)
{
    log_print(kTrace, "publishing %s to mqtt topic: %s", data.c_str(), m_config.maqtt_config.send.c_str());
    mq_client->publish(NULL, m_config.maqtt_config.send.c_str(), data.length(), data.c_str(), 1);
    mq_client->loop();
}

} // namespace business
} // namespace algorithmservice