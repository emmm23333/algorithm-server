#ifndef PROTOCOL_ALGORITHMSERVICE_COMPUTE_H_
#define PROTOCOL_ALGORITHMSERVICE_COMPUTE_H_
#include <rapidjson/json_stream.h>
namespace protocol
{
namespace algorithmservice
{
struct ComputeResult : public rapidjson::JsonInterface
{
public:
    int badEdgeCount = 0;
    int tinyHolesCount = 0;
    int bigHolesCount = 0;
    int badOriTriCount = 0;
    int isctTriCount = 0;
    int faceCount = 0;
    int vertCount = 0;
    int edgeCount = 0;
    float xLen = 0;
    float yLen = 0;
    float zLen = 0;
    int retries = 0;

protected:
    void encode(rapidjson::JsonStreamType &strm, rapidjson::Value &json)
    {
        strm.encode("badEdgeCount", json, badEdgeCount);
        strm.encode("tinyHolesCount", json, tinyHolesCount);
        strm.encode("bigHolesCount", json, bigHolesCount);
        strm.encode("isctTriCount", json, isctTriCount);
        strm.encode("faceCount", json, faceCount);
        strm.encode("vertCount", json, vertCount);
        strm.encode("edgeCount", json, edgeCount);
        strm.encode("xLen", json, xLen);
        strm.encode("yLen", json, yLen);
        strm.encode("zLen", json, zLen);
        strm.encode("retries", json, retries);
    }
    void decode(rapidjson::JsonStreamType &strm, rapidjson::Value &json)
    {
        strm.decode("badEdgeCount", json, badEdgeCount);
        strm.decode("tinyHolesCount", json, tinyHolesCount);
        strm.decode("bigHolesCount", json, bigHolesCount);
        strm.decode("isctTriCount", json, isctTriCount);
        strm.decode("faceCount", json, faceCount);
        strm.decode("vertCount", json, vertCount);
        strm.decode("edgeCount", json, edgeCount);
        strm.decode("xLen", json, xLen);
        strm.decode("yLen", json, yLen);
        strm.decode("zLen", json, zLen);
        strm.decode("retries", json, retries);
    }
};
} // namespace algorithmservice
} // namespace protocol

#endif