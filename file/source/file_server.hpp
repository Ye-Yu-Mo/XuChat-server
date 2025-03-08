// 实现文件存储子服务
// 1. 实现文件rpc服务类 --- 实现rpc调用的业务处理接口
// 2. 实现文件存储子服务的服务器类
// 3. 实现文件存储子服务类的构造者
#include <brpc/server.h>
#include <butil/logging.h>

#include "etcd.hpp"   // 服务注册模块封装
#include "logger.hpp" // 日志模块封装
#include "utils.hpp"
#include "base.pb.h"
#include "file.pb.h"

namespace XuChat
{
    class FileServiceImpl : public XuChat::FileService
    {
    public:
        FileServiceImpl(const std::string &storage_path) : _storage_path(storage_path)
        {
            umask(0);
            mkdir(storage_path.c_str(), 0775);
            if (_storage_path.back() != '/')
                _storage_path.push_back('/');
        }
        ~FileServiceImpl() {}
        void GetSingleFile(google::protobuf::RpcController *controller,
                           const ::XuChat::GetSingleFileReq *request,
                           ::XuChat::GetSingleFileRsp *response,
                           ::google::protobuf::Closure *done)
        {
            brpc::ClosureGuard rpc_guard(done);
            response->set_request_id(request->request_id());
            // 1. 取出请求中的文件ID（起始就是文件名）
            std::string fid = request->file_id();
            std::string filename = _storage_path + fid;
            // 2. 将文件ID作为文件名，读取文件数据
            std::string body;
            bool ret = XuChat::readFile(filename, body);
            if (ret == false)
            {
                response->set_success(false);
                response->set_errmsg("读取文件数据失败！");
                log_error(logger, "%s 读取文件数据失败！", request->request_id().c_str());
                return;
            }
            // 3. 组织响应
            response->set_success(true);
            response->mutable_file_data()->set_file_id(fid);
            response->mutable_file_data()->set_file_content(body);
        }
        void GetMultiFile(google::protobuf::RpcController *controller,
                          const ::XuChat::GetMultiFileReq *request,
                          ::XuChat::GetMultiFileRsp *response,
                          ::google::protobuf::Closure *done)
        {
            brpc::ClosureGuard rpc_guard(done);
            response->set_request_id(request->request_id());
            // 循环取出请求中的文件ID，读取文件数据进行填充
            for (int i = 0; i < request->file_id_list_size(); i++)
            {
                std::string fid = request->file_id_list(i);
                std::string filename = _storage_path + fid;
                std::string body;
                bool ret = XuChat::readFile(filename, body);
                if (ret == false)
                {
                    response->set_success(false);
                    response->set_errmsg("读取文件数据失败！");
                    log_error(logger, "%s 读取文件数据失败！", request->request_id().c_str());
                    return;
                }
                FileDownloadData data;
                data.set_file_id(fid);
                data.set_file_content(body);
                response->mutable_file_data()->insert({fid, data});
            }
            response->set_success(true);
        }
        void PutSingleFile(google::protobuf::RpcController *controller,
                           const ::XuChat::PutSingleFileReq *request,
                           ::XuChat::PutSingleFileRsp *response,
                           ::google::protobuf::Closure *done)
        {
            brpc::ClosureGuard rpc_guard(done);
            response->set_request_id(request->request_id());
            // 1. 为文件生成一个唯一uudi作为文件名 以及 文件ID
            std::string fid = XuChat::uuid();
            std::string filename = _storage_path + fid;
            // 2. 取出请求中的文件数据，进行文件数据写入
            bool ret = XuChat::writeFile(filename, request->file_data().file_content());
            if (ret == false)
            {
                response->set_success(false);
                response->set_errmsg("读取文件数据失败！");
                log_error(logger, "%s 写入文件数据失败！", request->request_id().c_str());
                return;
            }
            // 3. 组织响应
            response->set_success(true);
            response->mutable_file_info()->set_file_id(fid);
            response->mutable_file_info()->set_file_size(request->file_data().file_size());
            response->mutable_file_info()->set_file_name(request->file_data().file_name());
        }
        void PutMultiFile(google::protobuf::RpcController *controller,
                          const ::XuChat::PutMultiFileReq *request,
                          ::XuChat::PutMultiFileRsp *response,
                          ::google::protobuf::Closure *done)
        {
            brpc::ClosureGuard rpc_guard(done);
            response->set_request_id(request->request_id());
            for (int i = 0; i < request->file_data_size(); i++)
            {
                std::string fid = XuChat::uuid();
                std::string filename = _storage_path + fid;
                bool ret = XuChat::writeFile(filename, request->file_data(i).file_content());
                if (ret == false)
                {
                    response->set_success(false);
                    response->set_errmsg("读取文件数据失败！");
                    log_error(logger, "%s 写入文件数据失败！", request->request_id().c_str());
                    return;
                }
                XuChat::FileMessageInfo *info = response->add_file_info();
                info->set_file_id(fid);
                info->set_file_size(request->file_data(i).file_size());
                info->set_file_name(request->file_data(i).file_name());
            }
            response->set_success(true);
        }

    private:
        std::string _storage_path;
    };

    class FileServer
    {
    public:
        using ptr = std::shared_ptr<FileServer>;
        FileServer(const XuChat::Registry::ptr &reg_client,
                   const std::shared_ptr<brpc::Server> &server) : _reg_client(reg_client),
                                                                  _rpc_server(server) {}
        ~FileServer() {}
        // 搭建RPC服务器，并启动服务器
        void start()
        {
            _rpc_server->RunUntilAskedToQuit();
        }

    private:
        Registry::ptr _reg_client;
        std::shared_ptr<brpc::Server> _rpc_server;
    };

    class FileServerBuilder
    {
    public:
        // 用于构造服务注册客户端对象
        void make_reg_object(const std::string &reg_host,
                             const std::string &service_name,
                             const std::string &access_host)
        {
            _reg_client = std::make_shared<Registry>(reg_host);
            _reg_client->registry(service_name, access_host);
        }
        // 构造RPC服务器对象
        void make_rpc_server(uint16_t port, int32_t timeout,
                             uint8_t num_threads, const std::string &path = "./data/")
        {
            _rpc_server = std::make_shared<brpc::Server>();
            FileServiceImpl *file_service = new FileServiceImpl(path);
            int ret = _rpc_server->AddService(file_service,
                                              brpc::ServiceOwnership::SERVER_OWNS_SERVICE);
            if (ret == -1)
            {
                log_error(logger, "添加Rpc服务失败！");
                abort();
            }
            brpc::ServerOptions options;
            options.idle_timeout_sec = timeout;
            options.num_threads = num_threads;
            ret = _rpc_server->Start(port, &options);
            if (ret == -1)
            {
                log_error(logger, "服务启动失败！");
                abort();
            }
        }
        FileServer::ptr build()
        {
            if (!_reg_client)
            {
                log_error(logger, "还未初始化服务注册模块！");
                abort();
            }
            if (!_rpc_server)
            {
                log_error(logger, "还未初始化RPC服务器模块！");
                abort();
            }
            FileServer::ptr server = std::make_shared<FileServer>(_reg_client, _rpc_server);
            return server;
        }

    private:
        Registry::ptr _reg_client;
        std::shared_ptr<brpc::Server> _rpc_server;
    };
}