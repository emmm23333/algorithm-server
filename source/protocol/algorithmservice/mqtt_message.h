#pragma once

#include <vector>
#include <string>
#include <map>
#include <algorithmservice/business/config_detail.h>
#include <rapidjson/document.h>
#include <rapidjson/pointer.h>

using namespace algorithmservice;
namespace protocol
{
namespace algorithmservice
{
struct Bucket
{
    std::string data;
    std::string pub;
};
struct Oss
{
    Bucket bucket;
    std::string dfsId;
};

struct Params
{
    Oss oss;
    business::Modules config;
};

class MqRequest
{
public:
    MqRequest(){};
    ~MqRequest(){};
    MqRequest(const MqRequest &req)
    {
        m_id = req.m_id;
        m_method = req.m_method;
        m_backMsg = req.m_backMsg;
        m_params = req.m_params;
    };
    MqRequest &operator=(const MqRequest &req)
    {
        if (this != &req)
        {
            m_id = req.m_id;
            m_method = req.m_method;
            m_backMsg = req.m_backMsg;
            m_targetFormat = req.m_targetFormat;
            m_params = req.m_params;
        }
        return *this;
    };

    bool serialize(std::string &output){};
    bool de_serialize(const std::string &input, business::Modules config);

    // 后面再改成private吧
public:
    std::string m_id;
    std::string m_method;
    std::string m_backMsg;
    std::string m_targetFormat;
    std::map<std::string, std::vector<std::string>> m_flow;
    Params m_params;

private:
    rapidjson::Value *m_ptr;
    rapidjson::Document m_doc;

private:
    bool GetString(const std::string path, std::string &value);
    bool GetInt(const std::string path, uint32_t &value);
    bool GetBool(const std::string path, bool &value);
    bool GetFloat(const std::string path, float &value);
    bool GetDouble(const std::string path, double &value);
    bool GetMap(const std::string path, std::map<std::string, std::vector<std::string>> &value);
};

inline bool MqRequest::GetString(const std::string path, std::string &value)
{
    if (m_ptr = rapidjson::Pointer(path.c_str()).Get(m_doc))
    {
        value = m_ptr->GetString();
        return true;
    }
    return false;
}

inline bool MqRequest::GetInt(const std::string path, uint32_t &value)
{
    if (m_ptr = rapidjson::Pointer(path.c_str()).Get(m_doc))
    {
        value = m_ptr->GetInt();
        return true;
    }
    return false;
}

inline bool MqRequest::GetBool(const std::string path, bool &value)
{
    if (m_ptr = rapidjson::Pointer(path.c_str()).Get(m_doc))
    {
        value = m_ptr->GetBool();
        return true;
    }
    return false;
}

inline bool MqRequest::GetFloat(const std::string path, float &value)
{
    if (m_ptr = rapidjson::Pointer(path.c_str()).Get(m_doc))
    {
        value = m_ptr->GetFloat();
        return true;
    }
    return false;
}

inline bool MqRequest::GetDouble(const std::string path, double &value)
{
    if (m_ptr = rapidjson::Pointer(path.c_str()).Get(m_doc))
    {
        value = m_ptr->GetDouble();
        return true;
    }
    return false;
}

inline bool MqRequest::GetMap(const std::string path, std::map<std::string, std::vector<std::string>> &value)
{
    if (m_ptr = rapidjson::Pointer(path.c_str()).Get(m_doc))
    {
        auto objs = m_ptr->GetObject();
        for (auto &obj : objs)
        {
            std::vector<std::string> steps;
            for (auto &step : obj.value.GetArray())
            {
                steps.push_back(step.GetString());
            }
            value.insert(std::make_pair(obj.name.GetString(), steps));
        }
        return true;
    }
    return false;
}

inline bool MqRequest::de_serialize(const std::string &input, business::Modules config)
{
    using namespace rapidjson;

    m_doc.Parse(input.c_str());

    GetString("/id", m_id);
    GetString("/method", m_method);
    GetString("/backMsg", m_backMsg);
    GetString("/targetFormat", m_targetFormat);

    // flow
    GetMap("/flow", m_flow);

    // oss
    GetString("/params/oss/bucket/data", m_params.oss.bucket.data);
    GetString("/params/oss/bucket/pub", m_params.oss.bucket.pub);
    GetString("/params/oss/dfsId", m_params.oss.dfsId);

    // config/readOp
    if (!GetString("/params/config/readOp/format", m_params.config.readop.format))
    {
        m_params.config.readop.format = config.readop.format;
    }
    // printf("read op format: %s\n", m_params.config.readop.format.c_str());
    if (!GetBool("/params/config/readOp/isZip", m_params.config.readop.is_zip))
    {
        m_params.config.readop.is_zip = config.readop.is_zip;
    }
    // config/3mfOp
    if (!GetBool("/params/config/3mfOp/withColor", m_params.config.mfop.with_color))
    {
        m_params.config.mfop.with_color = config.mfop.with_color;
    }
    if (!GetBool("/params/config/3mfOp/withTexture", m_params.config.mfop.with_texture))
    {
        m_params.config.mfop.with_texture = config.mfop.with_texture;
    }
    // config/objOp
    if (!GetBool("/params/config/objOp/readVColorIfExists", m_params.config.objop.read_color_if_exists))
    {
        m_params.config.objop.read_color_if_exists = config.objop.read_color_if_exists;
    }
    // config/simplify
    if (!GetFloat("/params/config/simplify/remainRate", m_params.config.simplify.remain_rate))
    {
        m_params.config.simplify.remain_rate = config.simplify.remain_rate;
    }
    if (!GetDouble("/params/config/simplify/boundaryWeight", m_params.config.simplify.boundary_weight))
    {
        m_params.config.simplify.boundary_weight = config.simplify.boundary_weight;
    }
    if (!GetBool("/params/config/simplify/isCheckFaceFlip", m_params.config.simplify.is_check_face_flip))
    {
        m_params.config.simplify.is_check_face_flip = config.simplify.is_check_face_flip;
    }
    if (!GetBool("/params/config/simplify/isFeatureSensitive", m_params.config.simplify.is_feature_sensitive))
    {
        m_params.config.simplify.is_feature_sensitive = config.simplify.is_feature_sensitive;
    }
    // config/smooth
    if (!GetFloat("/params/config/smooth/moveDist", m_params.config.smooth.move_dist))
    {
        m_params.config.smooth.move_dist = config.smooth.move_dist;
    }
    if (!GetInt("/params/config/smooth/iter", m_params.config.smooth.iter))
    {
        m_params.config.smooth.iter = config.smooth.iter;
    }
    // config/saveGlb
    if (!GetBool("/params/config/saveGlb/writeASCII", m_params.config.saveglb.writeASCII))
    {
        m_params.config.saveglb.writeASCII = config.saveglb.writeASCII;
    }
    if (!GetBool("/params/config/saveGlb/needUpload", m_params.config.saveglb.need_upload))
    {
        m_params.config.saveglb.need_upload = config.saveglb.need_upload;
    }
    if (!GetBool("/params/config/saveGlb/pubBucket", m_params.config.saveglb.pub_bucket))
    {
        m_params.config.saveglb.pub_bucket = config.saveglb.pub_bucket;
    }
    // config/saveStl
    if (!GetBool("/params/config/saveStl/needUpload", m_params.config.savestl.need_upload))
    {
        m_params.config.savestl.need_upload = config.savestl.need_upload;
    }
    if (GetBool("/params/config/saveStl/pubBucket", m_params.config.savestl.pub_bucket))
    {
        m_params.config.savestl.pub_bucket = config.savestl.pub_bucket;
    }
    // printf("saveGlb: %d\n", m_params.config.saveglb.writeASCII);
    // config/savePly
    if (!GetBool("/params/config/savePly/needUpload", m_params.config.saveply.need_upload))
    {
        m_params.config.saveply.need_upload = config.saveply.need_upload;
    }
    if (!GetBool("/params/config/savePly/pubBucket", m_params.config.saveply.pub_bucket))
    {
        m_params.config.saveply.pub_bucket = config.saveply.pub_bucket;
    }
    if (!GetBool("/params/config/savePly/writeASCII", m_params.config.saveply.writeASCII))
    {
        m_params.config.saveply.writeASCII = config.saveply.writeASCII;
    }
    // config/saveBeb
    if (!GetBool("/params/config/saveBeb/needUpload", m_params.config.savebeb.need_upload))
    {
        m_params.config.savebeb.need_upload = config.savebeb.need_upload;
    }
    if (!GetBool("/params/config/saveBeb/pubBucket", m_params.config.savebeb.pub_bucket))
    {
        m_params.config.savebeb.pub_bucket = config.savebeb.pub_bucket;
    }
    // config/save3mf
    if (!GetBool("/params/config/save3mf/needUpload", m_params.config.save3mf.need_upload))
    {
        m_params.config.save3mf.need_upload = config.save3mf.need_upload;
    }
    if (!GetBool("/params/config/save3mf/pubBucket", m_params.config.save3mf.pub_bucket))
    {
        m_params.config.save3mf.pub_bucket = config.save3mf.pub_bucket;
    }
    // config/saveObj
    if (!GetBool("/params/config/saveObj/writeVColorIfExists", m_params.config.saveobj.write_v_color_if_exists))
    {
        m_params.config.saveobj.write_v_color_if_exists = config.saveobj.write_v_color_if_exists;
    }
    if (!GetBool("/params/config/saveObj/needUpload", m_params.config.saveobj.need_upload))
    {
        m_params.config.saveobj.need_upload = config.saveobj.need_upload;
    }
    if (!GetBool("/params/config/saveObj/pubBucket", m_params.config.saveobj.pub_bucket))
    {
        m_params.config.saveobj.pub_bucket = config.saveobj.pub_bucket;
    }
    // config/repair
    if (!GetInt("/params/config/repair/maxRetries", m_params.config.repair.max_retries))
    {
        m_params.config.repair.max_retries = config.repair.max_retries;
    }
    // config/saveZip
    if (!GetBool("/params/config/saveZip/replaceZip", m_params.config.savezip.relpace_zip))
    {
        m_params.config.savezip.relpace_zip = config.savezip.relpace_zip;
    }
    if (!GetBool("/params/config/saveZip/needUpload", m_params.config.savezip.need_upload))
    {
        m_params.config.savezip.need_upload = config.savezip.need_upload;
    }
    if (!GetBool("/params/config/saveZip/pubBucket", m_params.config.savezip.pub_bucket))
    {
        m_params.config.savezip.pub_bucket = config.savezip.pub_bucket;
    }

    // config/pngFile
    Value *p = Pointer("/params/config/pngFile").Get(m_doc);
    if (p)
    {
        auto pngs = p->GetArray();
        for (auto &png : pngs)
        {
            business::PngFile pngfile;
            pngfile.width = Pointer("/width").Get(png)->GetInt();
            printf("png file width:%d\n", pngfile.width);
            pngfile.height = Pointer("/height").Get(png)->GetInt();
            pngfile.longitude = Pointer("/longitude").Get(png)->GetFloat();
            pngfile.latitude = Pointer("/latitude").Get(png)->GetFloat();
            pngfile.need_upload = Pointer("/needUpload").Get(png)->GetBool();
            pngfile.pub_bucket = Pointer("/pubBucket").Get(png)->GetBool();
            m_params.config.pngfiles.push_back(pngfile);
        }
    }
    else
    {
        m_params.config.pngfiles = config.pngfiles;
    }

    // printf("latitude: %f\n", m_params.config.pngfile.latitude);
    return true;
}

} // namespace algorithmservice
} // namespace protocol
