#ifndef ALGORITHMSERVICE_BUSINESS_CONFIG_DETAIL_H_
#define ALGORITHMSERVICE_BUSINESS_CONFIG_DETAIL_H_

#include <string>
#include <vector>
#include <map>
#include <stdint.h>
namespace algorithmservice
{
namespace business
{

struct SaveConfig
{
    std::string msg;
    std::string download;
    std::string new_save;
};

struct MqttConfig
{
    std::string host;
    uint16_t port;
    std::string username;
    std::string password;
    std::string clean_start;
    std::string client_id;
    std::string recieve;
    std::string send;
};

struct BucketInfo
{
    std::string end_point;
    std::string access_key;
    std::string access_secret;
};

struct OssConfig
{
    std::map<std::string, BucketInfo> buckets;
};

struct ControlConfig
{
    uint32_t processes;
    uint32_t cpu_rate;
    uint32_t net_interval;
    uint32_t net_retries;
    uint64_t part_size;
    uint32_t thread_num;
};

struct ReadOp
{
    std::string format;
    bool is_zip;
};

struct MfOp
{
    bool with_color;
    bool with_texture;
};

struct ObjOp
{
    bool read_color_if_exists;
};

struct Simplify
{
    float remain_rate;
    double boundary_weight;
    bool is_check_face_flip;
    bool is_feature_sensitive;
};

struct Smooth
{
    float move_dist;
    uint32_t iter;
};

struct SaveGlb
{
    bool need_upload;
    bool writeASCII;
    bool pub_bucket;
};

struct SaveStl
{
    bool need_upload;
    bool pub_bucket;
};

struct SavePly
{
    bool need_upload;
    bool pub_bucket;
    bool writeASCII;
};

struct SaveBeb
{
    bool need_upload;
    bool pub_bucket;
};

struct Save3mf
{
    bool need_upload;
    bool pub_bucket;
};

struct SaveObj
{
    bool write_v_color_if_exists;
    bool need_upload;
    bool pub_bucket;
};

struct SaveZip
{
    bool relpace_zip;
    bool need_upload;
    bool pub_bucket;
};

struct Repair
{
    uint32_t max_retries;
};

struct PngFile
{
    uint32_t width;
    uint32_t height;
    float longitude;
    float latitude;
    bool need_upload;
    bool pub_bucket;
};

struct Modules
{
    ReadOp readop;
    MfOp mfop;
    ObjOp objop;
    SaveGlb saveglb;
    SaveStl savestl;
    SavePly saveply;
    SaveBeb savebeb;
    Save3mf save3mf;
    SaveObj saveobj;
    SaveZip savezip;
    Simplify simplify;
    Smooth smooth;
    Repair repair;
    std::vector<PngFile> pngfiles;
};

} // namespace business
} // namespace algorithmservice
#endif