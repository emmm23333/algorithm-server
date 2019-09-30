#ifndef ALGORITHMSERVICE_BUSINESS_TASK_HANDLE_H_
#define ALGORITHMSERVICE_BUSINESS_TASK_HANDLE_H_

#include <stdint.h>
#include <string>
#include <memory>
#include <atomic>
#include <iostream>
#include <functional>
#include <ustd/singleton.h>
#include <ustd/task_thread.h>
#include <mosquittopp.h>
#include <protocol/algorithmservice/mosquitto_message.h>
#include <protocol/algorithmservice/mqtt_message.h>
#include "config.h"
#include <business/oss_handler.h>
#include <business/algorithm_functions.h>
#include <ustd/task_thread.h>
#include <atomic>
namespace algorithmservice
{
namespace business
{

class MqttClient : public mosqpp ::mosquittopp
{
public:
    MqttClient(const char *id, bool clean_session) : mosquittopp(id, clean_session) {}
    void on_connect(int rc);
    void on_disconnect() { std::cout << "on_disconnect" << std::endl; }
    void on_publish(int mid) { std::cout << "on_publish" << std::endl; }
    void on_subscribe(int mid, int qos_count, const int *granted_qos);
    void on_message(const struct mosquitto_message *message);

public:
    void set_on_message_callback(const std::function<int(const struct mosquitto_message *message)> func);
    void set_config(algorithmservice::business::MqttConfig &);

private:
    std::function<int(const struct mosquitto_message *message)> on_message_callback_;
    algorithmservice::business::MqttConfig m_config;
};

class TaskHandler
{
public:
    bool init(int, int, const algorithmservice::business::Config &);
    bool uninit();
    bool process(std::shared_ptr<protocol::algorithmservice::mosquitto_message> msg, std::string &result);

private:
    int m_epollfd;
    int m_mqfd;
    algorithmservice::business::Config m_config;
    std::shared_ptr<OssHandle> oss_handle_;
    std::shared_ptr<AlgorithmFunctions> algorithm_funcs_;
    std::shared_ptr<ustd::TaskThread> task_thread_;
    std::atomic<int> m_upcounts;

private:
    bool download_file(protocol::algorithmservice::MqRequest &request, std::string &file_path, std::string &file_name);
    bool upload_files(protocol::algorithmservice::MqRequest &request, std::map<std::string, std::vector<protocol::algorithmservice::SingleFile>> &sfs);
    int send_to_parent(std::string);
};

class ScheHandler
{
public:
    bool init(int, int, const algorithmservice::business::Config &);
    void process();
    void publish_to_mq(std::string);

private:
    int send_msg(const struct mosquitto_message *message);

private:
    int m_epollfd;
    algorithmservice::business::Config m_config;
    int m_mqfd;

private:
    std::shared_ptr<MqttClient> mq_client;
};

} // namespace business
} // namespace algorithmservice

#endif //!ALGORITHMSERVICE_BUSINESS_TASK_HANDLE_H_