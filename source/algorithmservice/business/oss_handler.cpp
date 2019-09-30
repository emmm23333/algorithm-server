#include "oss_handler.h"
#include <unistd.h>
#include <ustd/log.h>
#include <ustd/path_string.h>
#include <ustd/string.h>
#include <vector>
using namespace AlibabaCloud::OSS;

namespace algorithmservice
{
namespace business
{

OssHandle::OssHandle(ControlConfig cfg)
{
    m_config = cfg;
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

    int retries0 = m_config.net_retries;
    while (retries0--)
    {
        auto header = client.HeadObject(obj.BucketName, obj.ObjectName);
        if (!header.isSuccess())
        {
            log_print(kError, "get meta data error, code: %s, message: %s, requestId:%s", header.error().Code().c_str(), header.error().Message().c_str(), header.error().RequestId().c_str());
            if (0 == retries0)
                return false;
            sleep(m_config.net_interval);
        }
        else
        {
            obj.FileName = header.result().ContentDisposition();
            break;
        }
    }

    std::vector<std::string> names;
    ustd::split_string(obj.FileName, "=", names);
    if (names.size() >= 2)
    {
        obj.FileName = names[1];
    }

    log_print(kTrace, "get object file name: %s", obj.FileName.c_str());

    // DONE...考虑跨平台，不应该简单+"/"，后面有空再改吧。

    /*auto outcome = client.GetObject(obj.BucketName, obj.ObjectName, obj.FilePath + "/" + obj.FileName);
    if (outcome.isSuccess())
    {
        log_print(kDebug, "GetObjectToFile success， meta: len(%d), name(%s)", outcome.result().Metadata().ContentLength(), outcome.result().Metadata().ContentDisposition().c_str());
    }
    else
    {
        log_print(kError, "GetObjectToFile fail, code:%s, message: %s, requestId:%s", outcome.error().Code().c_str(), outcome.error().Message().c_str(), outcome.error().RequestId().c_str());
        return false;
    }*/

    // 断点续传下载API
    int retries = m_config.net_retries;
    while (retries--)
    {
        DownloadObjectRequest request(obj.BucketName, obj.ObjectName, ustd::compose_path(obj.FilePath, obj.FileName), obj.FilePath);
        auto outcome = client.ResumableDownloadObject(request);
        if (outcome.isSuccess())
        {
            log_print(kDebug, "ResumableDownloadObject success， meta: len(%d), name(%s)", outcome.result().Metadata().ContentLength(), outcome.result().Metadata().ContentDisposition().c_str());
            break;
        }
        else
        {
            if (0 == retries)
                return false;
            log_print(kError, "ResumableDownloadObject fail, code:%s, message: %s, requestId:%s", outcome.error().Code().c_str(), outcome.error().Message().c_str(), outcome.error().RequestId().c_str());
            log_print(kError, "net retry interval: %d, net retries: %d", m_config.net_interval, m_config.net_retries);
            sleep(m_config.net_interval);
        }
    }

    return true;
}

bool OssHandle::upload(OssInfo oss, ObjectInfo obj)
{
    ClientConfiguration conf;
    OssClient client(oss.Endpoint, oss.AccessKeyId, oss.AccessKeySecret, conf);
    auto meta = ObjectMetaData();
    meta.setContentDisposition("filename="+obj.FileName);
    // printf("------- set content: %s \n", obj.FileName.c_str());
    std::string file = ustd::compose_path(obj.FilePath.c_str(), obj.FileName.c_str());
    log_print(kInfo, "uploading file %s to bucket %s with dfsId %s", file.c_str(), obj.BucketName.c_str(), obj.ObjectName.c_str());

    /* auto outcome = client.PutObject(obj.BucketName, obj.ObjectName, file, meta);
    if (!outcome.isSuccess())
    {
        log_print(kError, "PutObject fail, code: %s, message: %s, requestId:%s", outcome.error().Code().c_str(), outcome.error().Message().c_str(), outcome.error().RequestId().c_str());
        return false;
    }*/

    int retries = m_config.net_retries;
    while (retries--)
    {
        if (m_config.part_size < 102400)
            m_config.part_size = 102400;
        if (m_config.thread_num == 0)
            m_config.thread_num = 1;

        UploadObjectRequest request(obj.BucketName, obj.ObjectName, file, obj.FilePath, m_config.part_size, m_config.thread_num, meta);
        auto outcome = client.ResumableUploadObject(request);
        if (!outcome.isSuccess())
        {
            if (0 == retries)
                return false;
            log_print(kError, "ResumableUploadObject fail, file: %s, code: %s, message: %s, requestId:%s", file.c_str(), outcome.error().Code().c_str(), outcome.error().Message().c_str(), outcome.error().RequestId().c_str());
            log_print(kError, "net retry interval: %d, net retries: %d", m_config.net_interval, m_config.net_retries);
            sleep(m_config.net_interval);
        }
        else
        {
            break;
        }
    }
    return true;
}

} // namespace business
} // namespace algorithmservice