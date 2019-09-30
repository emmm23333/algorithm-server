#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <mutex>
#include <ustd/log.h>
#include <sys/wait.h>
#include <mosquittopp.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
class MqttClient : public mosqpp ::mosquittopp
{
public:
    MqttClient(const char *id) : mosquittopp(id) {}
    void on_connect(int rc);
    void on_disconnect() { std::cout << "on_disconnect" << std::endl; }
    void on_publish(int mid) { std::cout << "on_publish" << std::endl; }
    void on_subscribe(int mid, int qos_count, const int *granted_qos); //订阅回调函数
    void on_message(const struct mosquitto_message *message);          //订阅主题接收到消息
};

std::string g_subTopic = "subTopic";
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
    bool res = false;
    mosqpp::topic_matches_sub(g_subTopic.c_str(), message->topic, &res);
    if (res)
    {
        std::string strRcv = (char *)message->payload;
        std::cout << "来自<" << message->topic << ">的消息：" << strRcv << std::endl;
    }
}

int main()
{

    // log_print(kInfo, "main process id: %ld\n", long(getpid()));
    // pid_t pid = 0;
    // if ((pid = fork()) < 0)
    // {
    //     log_print(kInfo, "fork error");
    // }
    // else if (pid > 0)
    // {
    //     log_print(kInfo, "in parent, child pid: %ld\n", long(pid));
    // }
    // else if (pid == 0)
    // {
    //     log_print(kInfo, "child will crash");
    //     int a = 1, b = 0;
    //     printf("%d", a / b);
    // }
    // int status = 0;
    // waitpid(-1, &status, 0);
    // if (WIFEXITED(status))
    // {
    //     printf("normal\n");
    // }
    // else if (WIFSIGNALED(status))
    // {
    //     printf("abnormal,signal id: %d, core: %d\n", WTERMSIG(status), WCOREDUMP(status));
    // }
    // sleep(1000);

    mosqpp::lib_init();
    MqttClient test("client6");

    int rc;
    char buf[1024] = "This is test";
    test.username_pw_set("testuser", "testpassword");
    // rc = test.connect("mq.tongxinmao.com", 18832); //本地IP
    rc = test.connect("10.20.2.164", 8882);
    char err[1024];
    if (rc == MOSQ_ERR_ERRNO)
        std::cout << "连接错误:" << mosqpp::strerror(rc) << std::endl; //连接出错
    else if (MOSQ_ERR_SUCCESS == rc)
    {
        //发布测试
        rc = test.loop();
        if (MOSQ_ERR_SUCCESS == rc)
        {
            std::cout << "before publish" << std::endl;
            rc = test.publish(NULL, "topic/test", strlen(buf), (const void *)buf);
            rc = test.loop();
            std::cout << "after publish" << std::endl;
        }
        // rc = test.disconnect(); //订阅测试时注释该行
        //  rc = test.loop(); //订阅测试时注释该行

        test.subscribe(NULL, g_subTopic.c_str()); //订阅测试取消注释该行
        rc = test.loop_forever();                 //订阅测试取消注释该行
        std::cout << "rc = " << rc << std::endl;
    }
    std::cout << "cleanup!" << std::endl;
    mosqpp::lib_cleanup();
    pause();
    return 0;
}