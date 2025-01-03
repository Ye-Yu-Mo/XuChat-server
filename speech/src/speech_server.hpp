/// @file 语音识别服务
#include <brpc/server.h>
#include <butil/logging.h>
#include "asr.hpp"              // 语音识别模块
#include "etcd.hpp"             // 服务注册模块
#include "logger.hpp"           // 日志模块
#include "speech.pb.h"          // protobuf
// 创建子类 继承 EchoService 实现rpc调用的功能

namespace XuChat
{
    class SpeechServiceImpl : public XuChat::SpeechService
    {
    public:
        SpeechServiceImpl(Common::ASRClient::ptr asr_client)
            : _asr_client(asr_client) {}
        ~SpeechServiceImpl() {}
        void SpeechRecognition(google::protobuf::RpcController *controller,
                               const ::XuChat::SpeechRecognitionReq *request,
                               ::XuChat::SpeechRecognitionRsp *response,
                               ::google::protobuf::Closure *done)
        {
            brpc::ClosureGuard rpc_guard(done);
            std::string errmsg;
            std::string res = _asr_client->recognize(request->speech_content(), errmsg);
            if (res.empty())
            {
                log_error(Common::logger, "语音识别失败! req_id : %s, reason: %s", request->request_id().c_str(), errmsg.c_str());
                response->set_request_id(request->request_id());
                response->set_success(false);
                response->set_errmsg("语音识别失败! reason: " + errmsg);
                return;
            }
            response->set_request_id(request->request_id());
            response->set_success(true);
            response->set_recognition_result(res);
        }

    private:
        Common::ASRClient::ptr _asr_client;
    };

    class SpeechServer
    {
    public:
        using ptr = std::shared_ptr<SpeechServer>;
        SpeechServer(const Common::ASRClient::ptr &asr_client,
                     const Common::Registry::ptr &reg_client,
                     const std::shared_ptr<brpc::Server> &rpc_server)
            : _asr_client(asr_client),
              _reg_client(reg_client),
              _rpc_server(rpc_server) {}
        ~SpeechServer() {}

        void start()
        {
            _rpc_server->RunUntilAskedToQuit();
        }

    private:
        Common::ASRClient::ptr _asr_client;
        Common::Registry::ptr _reg_client;
        std::shared_ptr<brpc::Server> _rpc_server;
    };

    class SpeechServerBuilder
    {
    public:
        SpeechServer::ptr build()
        {
            if (!_asr_client)
            {
                fatal(Common::logger, "语音识别模块尚未初始化!");
                abort();
            }
            if (!_reg_client)
            {
                fatal(Common::logger, "服务注册模块尚未初始化!");
                abort();
            }
            if (!_rpc_server)
            {
                fatal(Common::logger, "RPC服务模块尚未初始化!");
                abort();
            }
            return std::make_shared<SpeechServer>(_asr_client, _reg_client, _rpc_server);
        }
        /// @brief 构建语音识别客户端
        void makeAsrClient(const std::string &app_id,
                           const std::string &api_key,
                           const std::string &secret_key)
        {
            _asr_client = std::make_shared<Common::ASRClient>(app_id, api_key, secret_key);
        }

        /// @brief 构建注册中心客户端
        /// @param reg_host 注册中心地址
        /// @param service_name 服务名称
        /// @param access_host 访问服务地址
        void makeRegClient(const std::string &reg_host,
                           const std::string &service_name,
                           const std::string &access_host)
        {
            _reg_client = std::make_shared<Common::Registry>(reg_host);
            _reg_client->registry(service_name, access_host);
        }

        /// @brief 构建rpc服务端并运行
        /// @param port 监听端口号
        /// @param timeout 超时时间
        /// @param num_thread 线程数量
        void makeRpcServer(uint16_t port,
                           int32_t timeout,
                           uint8_t num_threads)
        {
            _rpc_server = std::make_shared<brpc::Server>();
            SpeechServiceImpl* speech_service = new SpeechServiceImpl(_asr_client);
            int ret = _rpc_server->AddService(speech_service, brpc::ServiceOwnership::SERVER_OWNS_SERVICE);
            if (ret = -1)
            {
                fatal(Common::logger, "服务添加失败!");
                abort();
            }
            brpc::ServerOptions options;
            options.idle_timeout_sec = timeout;
            options.num_threads = num_threads;
            ret = _rpc_server->Start(port, &options);
            if (ret == -1)
            {
                fatal(Common::logger, "服务启动失败!");
                abort();
            }
        }

    private:
        Common::ASRClient::ptr _asr_client;
        Common::Registry::ptr _reg_client;
        std::shared_ptr<brpc::Server> _rpc_server;
    };
}