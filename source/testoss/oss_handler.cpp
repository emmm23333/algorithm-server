#include "oss_handler.h"
#include <ustd/log.h>
using namespace AlibabaCloud::OSS;

namespace algorithmservice
{
namespace business
{

OssHandle::OssHandle()
{
    log_print(kTrace, "in osshandle()");
}

OssHandle::~OssHandle()
{
    /*释放网络等资源*/
    ShutdownSdk();
}

bool OssHandle::download(OssInfo &oss, ObjectInfo &obj)
{
    ClientConfiguration conf;
    OssClient client(oss.Endpoint, oss.AccessKeyId, oss.AccessKeySecret, conf);

    // TODO...这个官方的怎么下载报IO错误？fstream这么用看起来没错，后面有空再看吧，先用GetObject下载。
    // GetObjectRequest request(obj.BucketName, obj.ObjectName);
    // request.setResponseStreamFactory([=]() { return std::make_shared<std::fstream>(obj.ObjectName, std::ios_base::out | std::ios_base::in | std::ios_base::trunc | std::ios_base::binary); });

    // auto outcome = client.GetObject(request);

    if (obj.FileName.empty())
    {
        log_print(kTrace, "download doesn't sepcify filename, getting from remote...");
    }

    auto header = client.HeadObject(obj.BucketName, obj.ObjectName);
    if (!header.isSuccess())
    {
        log_print(kError, "get meta data error, code: %s, message: %s, requestId:%s", header.error().Code().c_str(), header.error().Message().c_str(), header.error().RequestId());
        return false;
    }
    obj.FileName = header.result().ContentDisposition();
    log_print(kTrace, "get object file name: %s", obj.FileName.c_str());

    // TODO...考虑跨平台，不应该简单+"/"，后面有空再改吧。
    auto outcome = client.GetObject(obj.BucketName, obj.ObjectName, obj.FilePath + "/" + obj.FileName);
    if (outcome.isSuccess())
    {
        log_print(kDebug, "GetObjectToFile success， meta: len(%d), name(%s)", outcome.result().Metadata().ContentLength(), outcome.result().Metadata().ContentDisposition().c_str());
    }
    else
    {
        log_print(kError, "GetObjectToFile fail, code:%s, message: %s, requestId:%s", outcome.error().Code().c_str(), outcome.error().Message().c_str(), outcome.error().RequestId().c_str());
        return false;
    }
    return true;
}

bool OssHandle::upload(OssInfo &oss, ObjectInfo &obj)
{
    ClientConfiguration conf;
    OssClient client(oss.Endpoint, oss.AccessKeyId, oss.AccessKeySecret, conf);
    auto meta = ObjectMetaData();
    meta.setContentDisposition(obj.FileName);
    auto outcome = client.PutObject(obj.BucketName, obj.ObjectName, obj.FilePath + "/" + obj.FileName, meta);
    if (!outcome.isSuccess())
    {
        log_print(kError, "PutObject fail, code: %s, message: %s, requestId:%s", outcome.error().Code().c_str(), outcome.error().Message().c_str(), outcome.error().RequestId().c_str());
        return false;
    }
    return true;
}

} // namespace business
} // namespace algorithmservice