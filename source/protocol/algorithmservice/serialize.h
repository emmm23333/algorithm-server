
#ifndef PROTOCOL_ALGORITHMSERVICE_SERIALIZE_H_
#define PROTOCOL_ALGORITHMSERVICE_SERIALIZE_H_
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
#include <iostream>

namespace protocol
{
namespace algorithmservice
{
template <class T>
std::string BoostBinarySerialize(std::shared_ptr<T> t)
{
    // printf("before BoostBinarySerialize\n");
    std::stringstream os(std::ios_base::binary | std::ios_base::out | std::ios_base::in);
    boost::archive::binary_oarchive oa(os, boost::archive::no_header);
    oa << t;
    // printf("after BoostBinarySerialize\n");
    return os.str();
}

template <class T>
std::shared_ptr<T> BoostBinaryDeserialize(std::string data)
{
    // printf("data: %s datalen: %d\n", data.data(), data.length());
    std::stringstream is(data, std::ios_base::binary | std::ios_base::out | std::ios_base::in);
    boost::archive::binary_iarchive oa(is, boost::archive::no_header);
    auto t = std::make_shared<T>();
    oa >> t;
    // printf("after BoostBinaryDeserialize\n");
    return t;
}

template <class T>
void BoostBinarySerializeToFile(std::shared_ptr<T> t, std::string filename)
{
    std::ofstream ofs(filename);
    boost::archive::text_oarchive oa(ofs);
    oa << t;
    ofs.close();
}

template <class T>
std::shared_ptr<T> BoostBinaryDerializeFromFile(std::string filename)
{
    std::ifstream ifs(filename, std::ios::binary);
    boost::archive::text_iarchive ia(ifs);
    auto t = std::make_shared<T>();
    ia >> t;
    ifs.close();
    return t;
}

} // namespace algorithmservice
} // namespace protocol
#endif