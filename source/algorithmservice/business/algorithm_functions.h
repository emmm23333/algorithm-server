#ifndef ALGORITHMSERVICE_BUSINESS_FUNCTIONS_H_
#define ALGORITHMSERVICE_BUSINESS_FUNCTIONS_H_

#include <map>
#include <string>
#include <functional>
#include <memory>
#include <atomic>
#include <mutex>

#include <Sn3DAlgorithm/common/baseTypeIO/baseTypeIO.h>
#include <Sn3DAlgorithm/common/baseType/TriMesh.h>
#include <Sn3DAlgorithm/algorithm1Include.h>
#include <Sn3DAlgorithm/algorithmSlicingInclude.h>
#include <protocol/algorithmservice/mqtt_message.h>
#include <protocol/algorithmservice/compute.h>
#include <protocol/algorithmservice/task_result.h>
#include <ustd/task_thread.h>
#include "config.h"

namespace algorithmservice
{
namespace business
{

using namespace Sn3DAlgorithm;

class AlgorithmFunctions
{
public:
    AlgorithmFunctions();
    ~AlgorithmFunctions();

public:
    void init_funcs();
    int read_mesh_ex(const std::string download_path, const std::string src_file, const std::string save_path, protocol::algorithmservice::MqRequest &request, const algorithmservice::business::Config &config);
    bool serialize_result(protocol::algorithmservice::TaskResult &);
    void insert_file_result(std::string key, protocol::algorithmservice::SingleFile sf);
    std::map<std::string, std::string> m_err_msg;
    inline void clear()
    {
        m_download_path.clear();
        m_src_file.clear();
        m_upload_path.clear();
        m_thread_count = 0;
        // m_save_results.clear();
    };

private:
    std::map<std::string, std::function<bool(std::shared_ptr<BaseTriMesh>)>> m_funcs;
    std::shared_ptr<ustd::TaskThread> task_thread_;
    std::string m_download_path, m_src_file;
    std::string m_upload_path;
    int m_file_size;
    protocol::algorithmservice::MqRequest m_request;
    algorithmservice::business::Config m_config;

    std::atomic<uint32_t> m_thread_count;
    std::mutex m_ret_lock;
    std::mutex m_err_lock;

    // to generate result:
    protocol::algorithmservice::ComputeResult m_cr;
    protocol::algorithmservice::ComputeResult m_final_cr;
    std::map<std::string, std::vector<protocol::algorithmservice::SingleFile>> m_save_results;

private:
    void dispatch_task(std::vector<std::string> &, std::shared_ptr<BaseTriMesh>);

    bool save_3mf_ex(std::shared_ptr<BaseTriMesh>);
    bool save_obj_ex(std::shared_ptr<BaseTriMesh>);
    bool save_glb_ex(std::shared_ptr<BaseTriMesh>);
    bool save_stl_ex(std::shared_ptr<BaseTriMesh>);
    bool save_beb_ex(std::shared_ptr<BaseTriMesh>);
    bool png_file_ex(std::shared_ptr<BaseTriMesh>);
    bool save_ply_ex(std::shared_ptr<BaseTriMesh>);

    bool read_3mf_mesh_ex(std::shared_ptr<BaseTriMesh>);
    bool read_beb_mesh_ex(std::shared_ptr<BaseTriMesh>);
    bool read_stl_ex(std::shared_ptr<BaseTriMesh>);
    bool read_obj_ex(std::shared_ptr<BaseTriMesh>);
    bool read_ply_ex(std::shared_ptr<BaseTriMesh>);

    bool repair_ex(std::shared_ptr<BaseTriMesh>);
    bool compute_ex(std::shared_ptr<BaseTriMesh>);
    bool simplify_ex(std::shared_ptr<BaseTriMesh>);
};

// using namespace Sn3DAlgorithm;
// TriMesh mesh;
// int ret = read_mesh("/Users/1", mesh);
// printf("init! %d\n", ret);

} // namespace business
} // namespace algorithmservice

#endif