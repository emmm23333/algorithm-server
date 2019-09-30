#ifndef PROTOCOL_ALGORITHMSERVICE_TASK_RESULT_H_
#define PROTOCOL_ALGORITHMSERVICE_TASK_RESULT_H_

#include <protocol/algorithmservice/compute.h>
#include <string>
#include <map>
#include <vector>

#include <rapidjson/document.h>
#include <rapidjson/pointer.h>
#include <rapidjson/json_stream.h>

namespace protocol
{
namespace algorithmservice
{
struct SingleFile : public rapidjson::JsonInterface
{
public:
    std::string dfsId;
    std::string endpoint;
    std::string bucket;
    std::string file_path;
    std::string file_name;
    int size;
    bool isPub;

protected:
    void encode(rapidjson::JsonStreamType &strm, rapidjson::Value &json)
    {
        strm.encode("dfsId", json, dfsId);
        strm.encode("endpoint", json, endpoint);
        strm.encode("bucket", json, bucket);
        strm.encode("size", json, size);
        strm.encode("isPub", json, isPub);
    }
    void decode(rapidjson::JsonStreamType &strm, rapidjson::Value &json)
    {
        strm.decode("dfsId", json, dfsId);
        strm.decode("endpoint", json, endpoint);
        strm.decode("bucket", json, bucket);
        strm.decode("size", json, size);
        strm.decode("isPub", json, isPub);
    }
};

struct FileResult : public rapidjson::JsonInterface
{
public:
    std::map<std::string, std::vector<SingleFile>> saveResult;
    ComputeResult cr;
    ComputeResult cr_final;

protected:
    void encode(rapidjson::JsonStreamType &strm, rapidjson::Value &json)
    {
        for (auto &n : saveResult)
        {
            strm.encode(n.first.c_str(), json, n.second);
        }
        strm.encode("computeResult", json, cr);
        strm.encode("finalComputeResult", json, cr_final);
    }
    void decode(rapidjson::JsonStreamType &strm, rapidjson::Value &json)
    {
        for (auto &n : saveResult)
        {
            strm.decode(n.first.c_str(), json, n.second);
        }
        strm.decode("computeResult", json, cr);
        strm.decode("finalComputeResult", json, cr_final);
    }
};

class TaskResult : public rapidjson::JsonInterface
{
public:
    std::string id;
    std::string method;
    std::string backMsg;
    std::string targetFormat;
    std::string status;
    std::string error;
    std::string srcParams;
    FileResult fileResult;

protected:
    void encode(rapidjson::JsonStreamType &strm, rapidjson::Value &json)
    {
        strm.encode("id", json, id);
        strm.encode("method", json, method);
        strm.encode("backMsg", json, backMsg);
        strm.encode("targetFormat", json, targetFormat);
        strm.encode("status", json, status);
        strm.encode("error", json, error);
        strm.encode("srcParams", json, srcParams);
        strm.encode("result", json, fileResult);
    }
    void decode(rapidjson::JsonStreamType &strm, rapidjson::Value &json)
    {
        strm.decode("id", json, id);
        strm.decode("method", json, method);
        strm.decode("backMsg", json, backMsg);
        strm.decode("targetFormat", json, targetFormat);
        strm.decode("status", json, status);
        strm.decode("error", json, error);
        strm.decode("srcParams", json, srcParams);
        strm.decode("result", json, fileResult);
    }
};
} // namespace algorithmservice
} // namespace protocol
#endif