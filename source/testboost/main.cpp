#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <mutex>
#include <ustd/log.h>
#include <sys/wait.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

#include <boost/serialization/serialization.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <fstream>
#include <sstream>
using namespace boost::archive; //打开名称空间

class mosquitto_message
{
public:
    int mid;
    int a;
    std::string topic;
    int b;
    mosquitto_message(){};

private:
    friend boost::serialization::access; //声明友元，授予访问权限
    template <typename Archive>
    void serialize(Archive &ar, const unsigned int version) //序列化函数
    {
        ar &mid;
        ar &topic;
        ar &a;
        ar &b;
    };
};

int main()
{
    mosquitto_message msg;
    msg.mid = 1;
    msg.a = 99;
    msg.topic = "sss";
    msg.b = 100;

#ifdef BINARY
    std::stringstream os(std::ios_base::binary | std::ios_base::out | std::ios_base::in);
    boost::archive::binary_oarchive oa(os, boost::archive::no_header);
    oa << msg;

    mosquitto_message msg2;

    boost::archive::binary_iarchive ob(os, boost::archive::no_header);
    ob >> msg2;

    printf("str:%s len: %d\n", os.str().data(), os.str().length());
    printf("msg2: %d %d %d %s\n", msg2.mid, msg2.a, msg2.b, msg2.topic.data());
#endif

    std::ofstream ofs("tmp/task/111");
    boost::archive::text_oarchive text_oa(ofs);
    text_oa << msg;
    ofs.close();

    std::ifstream ifs("tmp/task/111", std::ios::binary);
    boost::archive::text_iarchive ia(ifs);
    mosquitto_message msg2;
    ia >> msg2;
    ifs.close();
    std::cout << msg2.topic << std::endl;
    std::cout << msg2.mid << std::endl;

    return 0;
}