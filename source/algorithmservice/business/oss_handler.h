#ifndef ALGORITHMSERVICE_BUSINESS_OSS_HANDLER_H_
#define ALGORITHMSERVICE_BUSINESS_OSS_HANDLER_H_
#include <memory>
#include <fstream>
#include <string>
#include <alibabacloud/oss/OssClient.h>
#include <business/config_detail.h>
namespace algorithmservice
{
namespace business
{

struct OssInfo
{
    std::string AccessKeyId;
    std::string AccessKeySecret;
    std::string Endpoint;
};

struct ObjectInfo
{
    std::string BucketName;
    std::string ObjectName;
    std::string FilePath;
    std::string FileName;
};

class OssHandle
{
public:
    OssHandle(ControlConfig);
    ~OssHandle();

    bool download(OssInfo &, ObjectInfo &);
    bool upload(OssInfo, ObjectInfo);

private:
    ControlConfig m_config;
};
} // namespace business
} // namespace algorithmservice

#endif