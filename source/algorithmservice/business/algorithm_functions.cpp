#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <vector>
#include <math.h>
#include <business/config.h>
#include <ustd/log.h>
#include <ustd/time_convert.h>
#include <ustd/path_string.h>
#include <ustd/file_system.h>
#include <ustd/string.h>
#include <common/system_helper.h>
#include <business/algorithm_functions.h>
#include <snUtils.h>
#include <gltfIO.h>
#include <algorithm2Include.h>
#include <protocol/algorithmservice/compute.h>
#include <protocol/algorithmservice/task_result.h>
namespace algorithmservice
{
namespace business
{

using namespace Sn3DAlgorithm;

AlgorithmFunctions::AlgorithmFunctions()
{
    init_funcs();
    task_thread_ = std::make_shared<ustd::TaskThread>();
    task_thread_->begin(2);
}

AlgorithmFunctions::~AlgorithmFunctions()
{
    task_thread_->end();
}

void AlgorithmFunctions::init_funcs()
{
    m_funcs.insert(std::make_pair("repair", std::bind(&AlgorithmFunctions::repair_ex, this, std::placeholders::_1)));
    m_funcs.insert(std::make_pair("compute", std::bind(&AlgorithmFunctions::compute_ex, this, std::placeholders::_1)));
    m_funcs.insert(std::make_pair("simplify", std::bind(&AlgorithmFunctions::simplify_ex, this, std::placeholders::_1)));

    m_funcs.insert(std::make_pair("3mf", std::bind(&AlgorithmFunctions::read_3mf_mesh_ex, this, std::placeholders::_1)));
    m_funcs.insert(std::make_pair("obj", std::bind(&AlgorithmFunctions::read_obj_ex, this, std::placeholders::_1)));
    m_funcs.insert(std::make_pair("stl", std::bind(&AlgorithmFunctions::read_stl_ex, this, std::placeholders::_1)));
    m_funcs.insert(std::make_pair("beb", std::bind(&AlgorithmFunctions::read_beb_mesh_ex, this, std::placeholders::_1)));
    m_funcs.insert(std::make_pair("ply", std::bind(&AlgorithmFunctions::read_ply_ex, this, std::placeholders::_1)));

    m_funcs.insert(std::make_pair("saveGlb", std::bind(&AlgorithmFunctions::save_glb_ex, this, std::placeholders::_1)));
    m_funcs.insert(std::make_pair("saveStl", std::bind(&AlgorithmFunctions::save_stl_ex, this, std::placeholders::_1)));
    m_funcs.insert(std::make_pair("saveObj", std::bind(&AlgorithmFunctions::save_obj_ex, this, std::placeholders::_1)));
    m_funcs.insert(std::make_pair("savePly", std::bind(&AlgorithmFunctions::save_ply_ex, this, std::placeholders::_1)));
    m_funcs.insert(std::make_pair("pngFile", std::bind(&AlgorithmFunctions::png_file_ex, this, std::placeholders::_1)));
}

int AlgorithmFunctions::read_mesh_ex(const std::string download_path, const std::string src_file, const std::string upload_path, protocol::algorithmservice::MqRequest &request, const algorithmservice::business::Config &config)
{
    log_print(kTrace, "read_mesh_ex download path: %s src file: %s", download_path.c_str(), src_file.c_str());
    m_download_path = download_path;
    m_src_file = src_file;
    m_upload_path = upload_path;
    m_request = request;
    m_config = config;

    int num = ceil(std::thread::hardware_concurrency() * m_config.control_config.cpu_rate / 100);
    set_tbb_num_threads(num);

    std::shared_ptr<BaseTriMesh> triMesh = std::make_shared<BaseTriMesh>();

    auto it = config.flow.find(request.m_method);
    if (it != config.flow.end() || !request.m_flow.empty())
    {
        auto itt = m_funcs.find(request.m_params.config.readop.format);
        if (itt != m_funcs.end())
        {
            if (!itt->second(triMesh))
            {
                m_err_lock.lock();
                m_err_msg["request.m_params.config.readop.format"] = " read error";
                m_err_lock.unlock();
                return -1;
            }
        }
        else
        {
            m_err_lock.lock();
            m_err_msg["request.m_params.config.readop.format"] = " function not found";
            m_err_lock.unlock();
            log_print(kError, "function %s not found", request.m_params.config.readop.format.c_str());
            return -1;
        }

        if (!request.m_flow.empty())
        {
            for (auto &n : request.m_flow)
            {
                std::shared_ptr<BaseTriMesh> triMesh_task = std::make_shared<BaseTriMesh>();
                *triMesh_task = *triMesh;
                ++m_thread_count;
                task_thread_->post_task(std::bind(&AlgorithmFunctions::dispatch_task, this, n.second, triMesh_task));
            }
        }
        else
        {
            for (auto &n : it->second)
            {
                std::shared_ptr<BaseTriMesh> triMesh_task = std::make_shared<BaseTriMesh>();
                *triMesh_task = *triMesh;
                ++m_thread_count;
                task_thread_->post_task(std::bind(&AlgorithmFunctions::dispatch_task, this, n.second, triMesh_task));
            }
        }
    }

    while (true)
    {
        if (m_thread_count == 0)
        {
            break;
        }
        sleep(1);
    }
    log_print(kInfo, "all threads finished! Result len: %d", m_save_results.size());

    return 0;
}

void AlgorithmFunctions::dispatch_task(std::vector<std::string> &task, std::shared_ptr<BaseTriMesh> triMesh)
{
    for (auto it : task)
    {
        auto itt = m_funcs.find(it);
        if (itt != m_funcs.end())
        {
            if (!(itt->second(triMesh)))
            {
                m_err_lock.lock();
                m_err_msg[itt->first] = " exec error";
                m_err_lock.unlock();
                break;
            }
        }
        else
        {
            log_print(kError, "function %s not found", it.c_str());
        }
    }
    --m_thread_count;
}

bool AlgorithmFunctions::read_3mf_mesh_ex(std::shared_ptr<BaseTriMesh> triMesh)
{
    log_print(kTrace, "read_3mf_mesh_ex, process pid(%d), thread pid(%d), points: %d", getpid(), ::syscall(SYS_gettid), triMesh->get_vn());
    int ret = read_3mf_mesh(ustd::compose_path(m_download_path.c_str(), m_src_file.c_str()), *triMesh, m_request.m_params.config.mfop.with_color, m_request.m_params.config.mfop.with_texture);
    if (ret)
    {
        log_print(kError, "read_3mf_mesh error, ret = %d", ret);
        return false;
    }
    return true;
}

bool AlgorithmFunctions::read_beb_mesh_ex(std::shared_ptr<BaseTriMesh> triMesh)
{
    log_print(kTrace, "read_stl_ex, process pid(%d), thread pid(%d), points: %d", getpid(), ::syscall(SYS_gettid), triMesh->get_vn());
    std::string head;
    int ret = decompress_file_to_mesh(ustd::compose_path(m_download_path.c_str(), m_src_file.c_str()), *triMesh, head);
    if (ret)
    {
        log_print(kError, "read_beb_mesh error, ret = %d", ret);
        return false;
    }
    return true;
}

bool AlgorithmFunctions::read_stl_ex(std::shared_ptr<BaseTriMesh> triMesh)
{
    log_print(kTrace, "read_stl_ex, process pid(%d), thread pid(%d), points: %d", getpid(), ::syscall(SYS_gettid), triMesh->get_vn());

    int ret = read_stl_mesh(ustd::compose_path(m_download_path.c_str(), m_src_file.c_str()), *triMesh);

    if (ret)
    {
        log_print(kError, "read stl error: %d", ret);
        return false;
    }
    else
    {
        log_print(kTrace, "read stl success");
    }

    return true;
}

bool AlgorithmFunctions::read_obj_ex(std::shared_ptr<BaseTriMesh> triMesh)
{
    log_print(kTrace, "read_obj_ex, process pid(%d), thread pid(%d), points: %d", getpid(), ::syscall(SYS_gettid), triMesh->get_vn());

    int ret = read_obj_with_texture(ustd::compose_path(m_download_path.c_str(), m_src_file.c_str()), *triMesh, m_request.m_params.config.objop.read_color_if_exists);

    if (ret)
    {
        log_print(kError, "read obj error: %d", ret);
        return false;
    }
    else
    {
        log_print(kTrace, "read obj success");
    }

    return true;
}

bool AlgorithmFunctions::read_ply_ex(std::shared_ptr<BaseTriMesh> triMesh)
{
    log_print(kTrace, "read_ply_ex, process pid(%d), thread pid(%d), points: %d", getpid(), ::syscall(SYS_gettid), triMesh->get_vn());

    int ret = read_ply_mesh(ustd::compose_path(m_download_path.c_str(), m_src_file.c_str()), *triMesh);

    if (ret)
    {
        log_print(kError, "read ply error: %d", ret);
        return false;
    }
    else
    {
        log_print(kTrace, "read ply success");
    }

    return true;
}

bool AlgorithmFunctions::save_stl_ex(std::shared_ptr<BaseTriMesh> triMesh)
{
    log_print(kTrace, "save_stl_ex, process pid(%d), thread pid(%d)", getpid(), ::syscall(SYS_gettid));
    // std::string name = ustd::trim_extension(m_src_file) + "_" + systemhelper::create_uuid() + ".stl";
    // log...不用uuid了，下同
    std::string name = ustd::trim_extension(m_src_file) + ".stl";
    std::string save = ustd::compose_path(m_upload_path.c_str(), name.c_str());

    int ret = write_stl_mesh(save, *triMesh);

    if (ret)
    {
        log_print(kError, "save stl error: %d", ret);
        return false;
    }
    else
    {
        log_print(kTrace, "save stl success");
    }

    if (m_request.m_params.config.savestl.need_upload)
    {
        std::string dfs_uuid(systemhelper::create_uuid());
        std::string dfs_id(m_request.m_params.oss.dfsId + "_out" + "/saveStl/" + dfs_uuid + "/" + name);
        protocol::algorithmservice::SingleFile sf;
        sf.dfsId = dfs_id;
        sf.bucket = m_request.m_params.config.savestl.pub_bucket ? m_request.m_params.oss.bucket.pub : m_request.m_params.oss.bucket.data;
        sf.isPub = m_request.m_params.config.savestl.pub_bucket;
        auto itt = m_config.oss_config.buckets.find(sf.bucket);
        if (m_config.oss_config.buckets.end() == itt)
        {
            log_print(kError, "can not find bucket %s in config", sf.bucket.c_str());
            return false;
        }
        sf.endpoint = itt->second.end_point;
        sf.size = ustd::get_file_size(save);
        sf.file_path = m_upload_path;
        sf.file_name = name;

        m_ret_lock.lock();
        auto it = m_save_results.find("saveStl");
        if (it == m_save_results.end())
        {
            std::vector<protocol::algorithmservice::SingleFile> sfs;
            sfs.push_back(sf);
            m_save_results.insert(std::make_pair("saveStl", sfs));
        }
        else
        {
            it->second.push_back(sf);
        }
        m_ret_lock.unlock();
    }

    return true;
}

bool AlgorithmFunctions::save_glb_ex(std::shared_ptr<BaseTriMesh> triMesh)
{
    log_print(kTrace, "save_glb_ex, process pid(%d), thread pid(%d)", getpid(), ::syscall(SYS_gettid));
    // std::string name = ustd::trim_extension(m_src_file) + "_" + systemhelper::create_uuid();
    std::string name = ustd::trim_extension(m_src_file);
    std::string save = ustd::compose_path(m_upload_path.c_str(), name.c_str());

    log_print(kDebug, "====== before save glb, points: %d", triMesh->get_vn());
    int ret = write_gltf_mesh(*triMesh, save.c_str(), m_request.m_params.config.saveglb.writeASCII);
    if (ret)
    {
        log_print(kError, "save glb error: %d", ret);
        return false;
    }
    else
    {
        log_print(kTrace, "save glb success with name: %s, writeASCII: %d", save.c_str(), m_request.m_params.config.saveglb.writeASCII);
    }

    if (m_request.m_params.config.saveglb.need_upload)
    {
        std::string dfs_uuid(systemhelper::create_uuid());
        std::string tail = m_request.m_params.config.saveglb.writeASCII ? ".gltf" : ".glb";
        std::string dfs_id(m_request.m_params.oss.dfsId + "_out" + "/saveGlb/" + dfs_uuid + "/" + name + tail);
        protocol::algorithmservice::SingleFile sf;
        sf.dfsId = dfs_id;
        sf.bucket = m_request.m_params.config.saveglb.pub_bucket ? m_request.m_params.oss.bucket.pub : m_request.m_params.oss.bucket.data;
        sf.isPub = m_request.m_params.config.saveglb.pub_bucket;
        auto itt = m_config.oss_config.buckets.find(sf.bucket);
        if (m_config.oss_config.buckets.end() == itt)
        {
            log_print(kError, "can not find bucket %s in config", sf.bucket.c_str());
            return false;
        }
        sf.endpoint = itt->second.end_point;
        sf.size = ustd::get_file_size(save + tail);
        log_print(kDebug, "sf name ========%d   %s",sf.size, (save + tail).c_str());
        sf.file_path = m_upload_path;
        sf.file_name = name + tail;

        m_ret_lock.lock();
        auto it = m_save_results.find("saveGlb");
        if (it == m_save_results.end())
        {
            std::vector<protocol::algorithmservice::SingleFile> sfs;
            sfs.push_back(sf);
            m_save_results.insert(std::make_pair("saveGlb", sfs));
        }
        else
        {
            it->second.push_back(sf);
        }
        m_ret_lock.unlock();
    }

    return true;
}

bool AlgorithmFunctions::save_obj_ex(std::shared_ptr<BaseTriMesh> triMesh)
{
    log_print(kTrace, "save_obj_ex, process pid(%d), thread pid(%d)", getpid(), ::syscall(SYS_gettid));

    // std::string name = ustd::trim_extension(m_src_file) + "_" + systemhelper::create_uuid() + ".obj";
    // std::string zip_name = ustd::trim_extension(m_src_file) + "_" + systemhelper::create_uuid() + ".zip";
    std::string name = ustd::trim_extension(m_src_file) + ".obj";
    // log...现在obj及其生成的文件 不保存成zip了
    std::string save = ustd::compose_path(m_upload_path.c_str(), name.c_str());
    // std::string save_zip = ustd::compose_path(m_upload_path.c_str(), zip_name.c_str());
    std::string mtlFile, imageFile;
    int ret = -1;
    log_print(kTrace, "write_obj_with_texture with params: %d", m_request.m_params.config.saveobj.write_v_color_if_exists);
    log_print(kTrace, "====== before save obj, count: %d", triMesh->get_vn());
    if ((ret = write_obj_with_texture(save, *triMesh, m_request.m_params.config.saveobj.write_v_color_if_exists, mtlFile, imageFile)) != 0)
    {
        log_print(kError, "write obj with texture error: %d", ret);
        return false;
    }
    else
    {
        log_print(kTrace, "write obj with texture success. File name: %s, Mtl name: %s, Image name: %s", name.c_str(), mtlFile.c_str(), imageFile.c_str());
    }

    // std::string cmd("zip -D -j " + ustd::compose_path(m_upload_path.c_str(), zip_name.c_str()) + " " + ustd::compose_path(m_upload_path.c_str(), name.c_str()) + " " + ustd::compose_path(m_upload_path.c_str(), mtlFile.c_str()) + " " + ustd::compose_path(m_upload_path.c_str(), imageFile.c_str()));
    // log_print(kDebug, "zip cmd: %s", cmd.c_str());
    // std::vector<std::string> results;
    // if (!systemhelper::execute_cmd(cmd, results))
    // {
    //     log_print(kError, "zip failed.");
    //     return false;
    // }

    if (m_request.m_params.config.saveobj.need_upload)
    {
        // std::string dfs_id(m_request.m_params.oss.dfsId + "_out" + "/saveObj/" + zip_name);
        std::string dfs_uuid(systemhelper::create_uuid());
        std::string dfs_id(m_request.m_params.oss.dfsId + "_out" + "/saveObj/" + dfs_uuid + "/" + name);
        protocol::algorithmservice::SingleFile sf;
        sf.dfsId = dfs_id;
        sf.bucket = m_request.m_params.config.saveobj.pub_bucket ? m_request.m_params.oss.bucket.pub : m_request.m_params.oss.bucket.data;
        sf.isPub = m_request.m_params.config.saveobj.pub_bucket;
        auto itt = m_config.oss_config.buckets.find(sf.bucket);
        if (m_config.oss_config.buckets.end() == itt)
        {
            log_print(kError, "can not find bucket %s in config", sf.bucket.c_str());
            return false;
        }
        sf.endpoint = itt->second.end_point;
        // sf.size = ustd::get_file_size(save_zip);
        sf.size = ustd::get_file_size(save);
        sf.file_path = m_upload_path;
        sf.file_name = name;

        auto func = [this, &sf, &mtlFile, &imageFile, &dfs_uuid](std::vector<protocol::algorithmservice::SingleFile> &sfs) {
            if (!mtlFile.empty())
            {
                sf.dfsId = m_request.m_params.oss.dfsId + "_out" + "/saveObj/" + dfs_uuid + "/" + mtlFile;
                sf.size = ustd::get_file_size(ustd::compose_path(m_upload_path.c_str(), mtlFile.c_str()));
                sf.file_name = mtlFile;
                sfs.push_back(sf);
            }
            if (!imageFile.empty())
            {
                sf.dfsId = m_request.m_params.oss.dfsId + "_out" + "/saveObj/" + dfs_uuid + "/" + imageFile;
                sf.size = ustd::get_file_size(ustd::compose_path(m_upload_path.c_str(), imageFile.c_str()));
                sf.file_name = imageFile;
                sfs.push_back(sf);
            }
        };

        m_ret_lock.lock();
        auto it = m_save_results.find("saveObj");
        if (it == m_save_results.end())
        {
            std::vector<protocol::algorithmservice::SingleFile> sfs;
            sfs.push_back(sf);
            func(sfs);
            m_save_results.insert(std::make_pair("saveObj", sfs));
        }
        else
        {
            it->second.push_back(sf);
            func(it->second);
        }
        m_ret_lock.unlock();
    }
    return true;
}

bool AlgorithmFunctions::save_beb_ex(std::shared_ptr<BaseTriMesh> triMesh)
{
    log_print(kTrace, "save_beb_ex, process pid(%d), thread pid(%d)", getpid(), ::syscall(SYS_gettid));
    // std::string name = ustd::trim_extension(m_src_file) + "_" + systemhelper::create_uuid() + ".beb";
    std::string name = ustd::trim_extension(m_src_file) + ".beb";
    std::string save = ustd::compose_path(m_upload_path.c_str(), name.c_str());

    std::string head;
    int ret = compress_mesh_to_file(*triMesh, head, save);

    if (ret)
    {
        log_print(kError, "save beb error: %d", ret);
        return false;
    }
    else
    {
        log_print(kTrace, "save beb success");
    }

    if (m_request.m_params.config.savebeb.need_upload)
    {
        std::string dfs_uuid(systemhelper::create_uuid());
        std::string dfs_id(m_request.m_params.oss.dfsId + "_out" + "/saveBeb/" + dfs_uuid + "/" + name);
        protocol::algorithmservice::SingleFile sf;
        sf.dfsId = dfs_id;
        sf.bucket = m_request.m_params.config.savebeb.pub_bucket ? m_request.m_params.oss.bucket.pub : m_request.m_params.oss.bucket.data;
        sf.isPub = m_request.m_params.config.savebeb.pub_bucket;
        auto itt = m_config.oss_config.buckets.find(sf.bucket);
        if (m_config.oss_config.buckets.end() == itt)
        {
            log_print(kError, "can not find bucket %s in config", sf.bucket.c_str());
            return false;
        }
        sf.endpoint = itt->second.end_point;
        sf.size = ustd::get_file_size(save);
        sf.file_path = m_upload_path;
        sf.file_name = name;

        m_ret_lock.lock();
        auto it = m_save_results.find("saveBeb");
        if (it == m_save_results.end())
        {
            std::vector<protocol::algorithmservice::SingleFile> sfs;
            sfs.push_back(sf);
            m_save_results.insert(std::make_pair("saveBeb", sfs));
        }
        else
        {
            it->second.push_back(sf);
        }
        m_ret_lock.unlock();
    }

    return true;
}

bool AlgorithmFunctions::save_3mf_ex(std::shared_ptr<BaseTriMesh> triMesh)
{
    log_print(kTrace, "save_smf_ex, process pid(%d), thread pid(%d)", getpid(), ::syscall(SYS_gettid));
    // std::string name = ustd::trim_extension(m_src_file) + "_" + systemhelper::create_uuid() + ".3mf";
    std::string name = ustd::trim_extension(m_src_file) + ".3mf";
    std::string save = ustd::compose_path(m_upload_path.c_str(), name.c_str());

    int ret = write_3mf_mesh(save, *triMesh, false, false);

    if (ret)
    {
        log_print(kError, "save 3mf error: %d", ret);
        return false;
    }
    else
    {
        log_print(kTrace, "save 3mf success");
    }

    if (m_request.m_params.config.save3mf.need_upload)
    {
        std::string dfs_uuid(systemhelper::create_uuid());
        std::string dfs_id(m_request.m_params.oss.dfsId + "_out" + "/save3mf/" + dfs_uuid + "/" + name);
        protocol::algorithmservice::SingleFile sf;
        sf.dfsId = dfs_id;
        sf.bucket = m_request.m_params.config.save3mf.pub_bucket ? m_request.m_params.oss.bucket.pub : m_request.m_params.oss.bucket.data;
        sf.isPub = m_request.m_params.config.save3mf.pub_bucket;
        auto itt = m_config.oss_config.buckets.find(sf.bucket);
        if (m_config.oss_config.buckets.end() == itt)
        {
            log_print(kError, "can not find bucket %s in config", sf.bucket.c_str());
            return false;
        }
        sf.endpoint = itt->second.end_point;
        sf.size = ustd::get_file_size(save);
        sf.file_path = m_upload_path;
        sf.file_name = name;

        m_ret_lock.lock();
        auto it = m_save_results.find("save3mf");
        if (it == m_save_results.end())
        {
            std::vector<protocol::algorithmservice::SingleFile> sfs;
            sfs.push_back(sf);
            m_save_results.insert(std::make_pair("save3mf", sfs));
        }
        else
        {
            it->second.push_back(sf);
        }
        m_ret_lock.unlock();
    }

    return true;
}

bool AlgorithmFunctions::save_ply_ex(std::shared_ptr<BaseTriMesh> triMesh)
{
    log_print(kTrace, "save_ply_ex, process pid(%d), thread pid(%d)", getpid(), ::syscall(SYS_gettid));
    // std::string name = ustd::trim_extension(m_src_file) + "_" + systemhelper::create_uuid() + ".ply";
    std::string name = ustd::trim_extension(m_src_file) + ".ply";
    std::string save = ustd::compose_path(m_upload_path.c_str(), name.c_str());

    std::string texture_name = ustd::trim_extension(m_src_file) + ".png";
    std::string texture_file = ustd::compose_path(m_upload_path.c_str(), texture_name.c_str());
    // printf(" ........ %s\n", save.c_str());
    int ret = write_ply_mesh(save, *triMesh, texture_file, !m_request.m_params.config.saveply.writeASCII);

    if (ret)
    {
        log_print(kError, "save ply error: %d", ret);
        return false;
    }
    else
    {
        log_print(kTrace, "save ply success");
    }

    if (m_request.m_params.config.saveply.need_upload)
    {
        std::string dfs_uuid(systemhelper::create_uuid());
        std::string dfs_id(m_request.m_params.oss.dfsId + "_out" + "/savePly/" + dfs_uuid + "/" + name);
        protocol::algorithmservice::SingleFile sf;
        sf.dfsId = dfs_id;
        sf.bucket = m_request.m_params.config.saveply.pub_bucket ? m_request.m_params.oss.bucket.pub : m_request.m_params.oss.bucket.data;
        sf.isPub = m_request.m_params.config.saveply.pub_bucket;
        auto itt = m_config.oss_config.buckets.find(sf.bucket);
        if (m_config.oss_config.buckets.end() == itt)
        {
            log_print(kError, "can not find bucket %s in config", sf.bucket.c_str());
            return false;
        }
        sf.endpoint = itt->second.end_point;
        sf.size = ustd::get_file_size(save);
        sf.file_path = m_upload_path;
        sf.file_name = name;

        m_ret_lock.lock();
        auto it = m_save_results.find("savePly");
        if (it == m_save_results.end())
        {
            std::vector<protocol::algorithmservice::SingleFile> sfs;
            sfs.push_back(sf);
            if (!texture_file.empty())
            {
                sf.file_name = texture_name;
                sf.dfsId = m_request.m_params.oss.dfsId + "_out" + "/savePly/" + dfs_uuid + "/" + texture_name;
                sf.size = ustd::get_file_size(texture_file);
                log_print(kDebug, "------ ply texture file: %s", sf.file_name.c_str());
                sfs.push_back(sf);
            }
            else
            {
                log_print(kDebug, "------ ply texture file empty");
            }
            m_save_results.insert(std::make_pair("savePly", sfs));
        }
        else
        {
            it->second.push_back(sf);
            if (!texture_file.empty())
            {
                sf.file_name = texture_name;
                sf.dfsId = m_request.m_params.oss.dfsId + "_out" + "/savePly/" + dfs_uuid + "/" + texture_name;
                sf.size = ustd::get_file_size(texture_file);
                it->second.push_back(sf);
                log_print(kDebug, "------ ply texture file: %s", sf.file_name.c_str());
            }
        }
        m_ret_lock.unlock();
    }

    return true;
}

bool AlgorithmFunctions::png_file_ex(std::shared_ptr<BaseTriMesh> triMesh)
{
    log_print(kTrace, "png file ex, process pid(%d), thread pid(%d)", getpid(), ::syscall(SYS_gettid));

    PRenderData render_data = nullptr;
    PRenderCPU render_cpu = nullptr;

    for (auto &it : m_request.m_params.config.pngfiles)
    {
        int i = 0;
        int ret;
        log_print(kTrace, "creating render cpu with width:%d, height:%d", it.width, it.height);
        if ((ret = create_render_cpu(&render_cpu, it.width, it.height)) != 0)
        {
            log_print(kError, "create render cpu error: %d", ret);
            return false;
        }

        // fflush(stdout);
        log_print(kTrace, "creating render data");

        if ((ret = create_render_data(triMesh.get(), &render_data)) != 0)
        {
            log_print(kError, "create render data error: %d", ret);
            return false;
        }

        log_print(kTrace, "rendering with cpu with longitude: %f latitude: %f", it.longitude, it.latitude);
        std::string name = ustd::trim_extension(m_src_file) + "_" + systemhelper::create_uuid() + ".png";
        std::string save = ustd::compose_path(m_upload_path.c_str(), name.c_str());
        if ((ret = render_with_cpu(render_cpu, render_data, it.longitude, it.latitude, save.c_str())) != 0)
        {
            log_print(kError, "rendering with cpu error: %d", ret);
            return false;
        }
        std::string dfs_id(m_request.m_params.oss.dfsId + "_out" + "pngFile" + "/" + name);
        // m_task_results.insert(std::make_pair("pngFile", dfs_id));

        if (it.need_upload)
        {
            std::string dfs_id(m_request.m_params.oss.dfsId + "_out" + "/pngFile/" + name);
            protocol::algorithmservice::SingleFile sf;
            sf.dfsId = dfs_id;
            sf.bucket = it.pub_bucket ? m_request.m_params.oss.bucket.pub : m_request.m_params.oss.bucket.data;
            sf.isPub = it.pub_bucket;
            auto itt = m_config.oss_config.buckets.find(sf.bucket);
            if (m_config.oss_config.buckets.end() == itt)
            {
                log_print(kError, "can not find bucket %s in config", sf.bucket.c_str());
                return false;
            }
            sf.endpoint = itt->second.end_point;
            sf.size = ustd::get_file_size(save);
            sf.file_path = m_upload_path;
            sf.file_name = name;

            m_ret_lock.lock();
            auto it = m_save_results.find("pngFile");
            if (it == m_save_results.end())
            {
                std::vector<protocol::algorithmservice::SingleFile> sfs;
                sfs.push_back(sf);
                m_save_results.insert(std::make_pair("pngFile", sfs));
            }
            else
            {
                it->second.push_back(sf);
            }
            m_ret_lock.unlock();
        }
    }
    release_render_cpu(&render_cpu);
    release_render_data(&render_data);
    return true;
}

bool AlgorithmFunctions::compute_ex(std::shared_ptr<BaseTriMesh> triMesh)
{
    log_print(kTrace, "compute_ex, process pid(%d), thread pid(%d)", getpid(), ::syscall(SYS_gettid));
    protocol::algorithmservice::ComputeResult cr;
    get_mesh_error_info(*triMesh, true, cr.badEdgeCount, cr.tinyHolesCount, cr.bigHolesCount, cr.badOriTriCount, cr.isctTriCount,
                        cr.faceCount, cr.vertCount, cr.edgeCount, cr.xLen, cr.yLen, cr.zLen);
    log_print(kTrace, "begin repair with, badEdgeCount: %d, tinyHolesCount: %d, bigHolesCount: %d, \
    badOriTriCount: %d, isctTriCount: %d, faceCount: %d, vertCount: %d, edgeCount: %d, xlen: %f, ylen: %f, zlen: %f",
              cr.badEdgeCount,
              cr.tinyHolesCount, cr.bigHolesCount, cr.badOriTriCount, cr.isctTriCount, cr.faceCount, cr.vertCount, cr.edgeCount, cr.xLen, cr.yLen, cr.zLen);
    m_cr = cr;
    return true;
}

bool AlgorithmFunctions::repair_ex(std::shared_ptr<BaseTriMesh> triMesh)
{
    log_print(kTrace, "repair_ex, process pid(%d), thread pid(%d)", getpid(), ::syscall(SYS_gettid));
    log_print(kTrace, "====== before repair, points: %d", triMesh->get_vn());
    protocol::algorithmservice::ComputeResult cr;

    bool needIsct = true;
    int iter = m_request.m_params.config.repair.max_retries;

    std::shared_ptr<BaseTriMesh> triMeshOut = std::make_shared<BaseTriMesh>();

    get_mesh_error_info(*triMesh, true, cr.badEdgeCount, cr.tinyHolesCount, cr.bigHolesCount, cr.badOriTriCount, cr.isctTriCount, cr.faceCount, cr.vertCount, cr.edgeCount,
                        cr.xLen, cr.yLen, cr.zLen);

    log_print(kTrace, "begin repair with iter %d", iter);
    while ((cr.badEdgeCount > 0 || cr.badOriTriCount > 0) && iter > 0)
    {
        mesh_auto_fix(*triMesh, *triMeshOut, needIsct);
        iter--;
    }

    get_mesh_error_info(*triMeshOut, true, cr.badEdgeCount, cr.tinyHolesCount, cr.bigHolesCount, cr.badOriTriCount, cr.isctTriCount, cr.faceCount, cr.vertCount, cr.edgeCount,
                        cr.xLen, cr.yLen, cr.zLen);

    log_print(kTrace, "after repair, badEdgeCount: %d, tinyHolesCount: %d, bigHolesCount: %d, \
    badOriTriCount: %d, isctTriCount: %d, faceCount: %d, vertCount: %d, edgeCount: %d, xlen: %f, ylen: %f, zlen: %f",
              cr.badEdgeCount,
              cr.tinyHolesCount, cr.bigHolesCount, cr.badOriTriCount, cr.isctTriCount, cr.faceCount, cr.vertCount, cr.edgeCount, cr.xLen, cr.yLen, cr.zLen);
    cr.retries = m_request.m_params.config.repair.max_retries - iter;
    m_final_cr = cr;
    triMesh.swap(triMeshOut);
    log_print(kTrace, "====== after repair, points: %d", triMesh->get_vn());
    log_print(kTrace, "repair finished");
    return true;
}

bool AlgorithmFunctions::simplify_ex(std::shared_ptr<BaseTriMesh> triMesh)
{
    log_print(kTrace, "simplify_ex, process pid(%d), thread pid(%d)", getpid(), ::syscall(SYS_gettid));
    float rate = m_request.m_params.config.simplify.remain_rate;
    size_t cur_points = triMesh->get_vn();
    size_t need_points = cur_points * m_request.m_params.config.simplify.remain_rate;
    log_print(kTrace, "====== simplify cur_points: %d, need_points: %d, boudary_weight: %f", cur_points, need_points,
              m_request.m_params.config.simplify.boundary_weight);
    TriMesh t;
    BaseTriMesh_convert_to_TriMesh(*triMesh, t);
    // TODO...返回值要找算法确认下
    QEM_simplification_omp(t, need_points, m_request.m_params.config.simplify.boundary_weight, m_request.m_params.config.simplify.is_check_face_flip, m_request.m_params.config.simplify.is_feature_sensitive);

    TriMesh_convert_to_BaseTriMesh(t, *triMesh);

    log_print(kTrace, "====== after simplify, points: %d", triMesh->get_vn());
    log_print(kInfo, "simplify finished");
    return true;
}

bool AlgorithmFunctions::serialize_result(protocol::algorithmservice::TaskResult &task_result)
{
    // TODO...这里应该对TaskResult再封装一层，不暴露json底层借口。
    task_result.id = m_request.m_id;
    task_result.method = m_request.m_method;
    task_result.backMsg = m_request.m_backMsg;
    task_result.targetFormat = m_request.m_targetFormat;
    task_result.status = "success";
    if (m_err_msg.size() > 0)
    {
        task_result.status = "error";
        std::string err_msg;
        for (auto &n : m_err_msg)
        {
            err_msg += (n.first + n.second);
            task_result.error = err_msg;
        }
    }
    task_result.fileResult.saveResult = m_save_results;
    // TODO...
    task_result.fileResult.cr = m_cr;
    task_result.fileResult.cr_final = m_final_cr;

    return true;
}

void AlgorithmFunctions::insert_file_result(std::string key, protocol::algorithmservice::SingleFile sf)
{
    m_ret_lock.lock();
    auto it = m_save_results.find(key);
    if (it == m_save_results.end())
    {
        std::vector<protocol::algorithmservice::SingleFile> sfs;
        sfs.push_back(sf);
        m_save_results.insert(std::make_pair(key, sfs));
    }
    else
    {
        it->second.push_back(sf);
    }
    m_ret_lock.unlock();
}

} // namespace business
} // namespace algorithmservice