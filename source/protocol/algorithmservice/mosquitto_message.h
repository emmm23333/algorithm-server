#ifndef PROTOCOL_ALGORITHMSERVICE_MOSQUITTO_H_
#define PROTOCOL_ALGORITHMSERVICE_MOSQUITTO_H_

#include <stdint.h>
#include <vector>
#include <string>

#include <boost/serialization/serialization.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <fstream>
#include <sstream>

namespace protocol
{
namespace algorithmservice
{
class mosquitto_message
{
public:
    mosquitto_message() {}
    mosquitto_message(int mid, std::string topic, std::string payload, int qos, bool retain)
        : m_mid(mid), m_topic(topic), m_payload(payload), m_qos(qos), m_retain(retain)
    {
    }

private:
    friend boost::serialization::access;
    template <typename Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar &m_mid;
        ar &m_topic;
        ar &m_payload;
        ar &m_qos;
        ar &m_retain;
    };

    // 后面再改成private把
public:
    int m_mid;
    std::string m_topic;
    std::string m_payload;
    int m_qos;
    bool m_retain;
};
} //namespace algorithmservice
} //namespace protocol

#endif //!PROTOCOL_ALGORITHMSERVICE_MOSQUITTO_H_