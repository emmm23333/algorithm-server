#include <vector>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <sys/socket.h>
#include <protocol/algorithmservice/mosquitto_message.h>
#include <protocol/algorithmservice/serialize.h>
#include <ustd/log.h>
#include <ustd/path_string.h>
#include <ustd/time_convert.h>
#include <ustd/file_system.h>
#include <ustd/string.h>
#include "./task_handle.h"
#include <common/system_helper.h>
namespace algorithmservice
{
namespace business
{

void MqttClient::on_subscribe(int mid, int qos_count, const int *granted_qos)
{
    std::cout << "订阅 mid: " << mid << std::endl;
}

void MqttClient::on_connect(int rc)
{
    std::cout << "connect rc: " << rc << std::endl;
}

void MqttClient::on_message(const struct mosquitto_message *message)
{
    bool res = true;
    // mosqpp::topic_matches_sub(g_subTopic.c_str(), message->topic, &res);
    if (res)
    {
        std::string strRcv = (char *)message->payload;
        std::cout << "来自<" << message->topic << ">的消息：" << strRcv << std::endl;
        on_message_callback_(message);
    }
}

void MqttClient::set_on_message_callback(const std::function<int(const struct mosquitto_message *message)> func)
{
    on_message_callback_ = func;
}

bool TaskHandler::init(int epollfd, int mqfd, const algorithmservice::business::Config &config)
{
    std::cout << "int task handler init" << std::endl;
    m_epollfd = epollfd;
    m_mqfd = mqfd;
    m_config = config;

    return true;
}

bool TaskHandler::process(std::shared_ptr<protocol::algorithmservice::mosquitto_message> msg, std::string &result)
{
    std::cout << "in task process" << std::endl;
    std::cout << "doing wrong!" << std::endl;

    if ((msg->m_payload)[0] == 'a')
    {
        std::cout << "??????????? I AM crashing !!!!!!!!!" << std::endl;
        char a[3];
        a[99] = 0;
        int b = 4 / 0;
        char *t = new char[2];
        *(t + 1024) = 2222;
        printf("%d\n", 2 / b);
    }

    if ((msg->m_payload)[0] == 'b')
    {
        int max = 60;
        srand(time(0));
        int sec = rand() % max;
        printf("??????????? I AM Going to Sleep %d s !!!!!!!!!\n", sec);
        sleep(sec);
    }

    result = "hello world";
    std::cout << "after doing wrong!" << std::endl;
    return true;
}

int TaskHandler::send_to_parent(std::string jsonstr)
{
    uint64_t length = jsonstr.length();
    printf("send_msg send: %d bytes\n", length);
    send(m_mqfd, &length, sizeof(uint64_t), 0);
    send(m_mqfd, jsonstr.data(), jsonstr.length(), 0);
}

bool ScheHandler::init(int epollfd, int mqfd, const algorithmservice::business::Config &config)
{
    std::cout << "int sche init" << std::endl;
    m_epollfd = epollfd;
    m_mqfd = mqfd;
    m_config = config;

    mosqpp::lib_init();
    int rc;
    char buf[1024] = "This is test";
    mq_client = std::make_shared<MqttClient>("client6", true);
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
            // std::cout << "before publish" << std::endl;
            // rc = mq_client->publish(NULL, "topic/test", strlen(buf), (const void *)buf);
            // rc = mq_client->loop();
            // std::cout << "after publish" << std::endl;
            std::cout << "after one mq loop" << std::endl;
        }
        // rc = test.disconnect(); //订阅测试时注释该行
        //  rc = test.loop(); //订阅测试时注释该行

        mq_client->subscribe(NULL, m_config.maqtt_config.recieve.c_str());
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
    printf("in send_msg: mid:%d qos:%d retain:%d payloadLen:%d topic:%s\n", message->mid, message->qos, message->retain, message->payloadlen, message->topic);
    // TODO...后面可以用共享内存做。
    auto msg = std::make_shared<protocol::algorithmservice::mosquitto_message>(message->mid, std::string(message->topic), std::string((char *)message->payload, message->payloadlen), message->qos, message->retain);

    std::string binary_msg = protocol::algorithmservice::BoostBinarySerialize<protocol::algorithmservice::mosquitto_message>(msg);

    uint64_t length = binary_msg.length();
    printf("send_msg send: %d\n", length);
    send(m_mqfd, &length, sizeof(uint64_t), 0);
    send(m_mqfd, binary_msg.data(), binary_msg.length(), 0);
    // send(m_mqfd, (const void *)message, sizeof(*message), 0);
    //  send(m_mqfd, (const void *)os.str().data(), os.str().length(), 0);
    return 0;
}

void ScheHandler::publish_to_mq(std::string data)
{
    log_print(kTrace, "publishing %s to mqtt topic: %s", data.c_str(), m_config.maqtt_config.send.c_str());
    mq_client->publish(NULL, m_config.maqtt_config.send.c_str(), data.length(), data.c_str());
    mq_client->loop();
}

} // namespace business
} // namespace algorithmservice