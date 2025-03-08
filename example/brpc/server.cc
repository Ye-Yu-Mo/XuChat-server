#include <brpc/server.h>
#include <butil/logging.h>
#include "main.pb.h"

//1. 继承于EchoService创建一个子类，并实现rpc调用的业务功能
class EchoServiceImpl : public example::EchoService {
    public:
        EchoServiceImpl(){}
        ~EchoServiceImpl(){}
        void Echo(google::protobuf::RpcController* controller,
                       const ::example::EchoRequest* request,
                       ::example::EchoResponse* response,
                       ::google::protobuf::Closure* done) {
            brpc::ClosureGuard rpc_guard(done);
            std::cout << "收到消息:" << request->message() << std::endl;

            std::string str = request->message() + "--这是响应！！";
            response->set_message(str);
            //done->Run();
        }
};
int main(int argc, char *argv[])
{
    //关闭brpc的默认日志输出
    logging::LoggingSettings settings;
    settings.logging_dest = logging::LoggingDestination::LOG_TO_NONE;
    logging::InitLogging(settings);
    //2. 构造服务器对象
    brpc::Server server;
    //3. 向服务器对象中，新增EchoService服务
    EchoServiceImpl echo_service;
    int ret = server.AddService(&echo_service, brpc::ServiceOwnership::SERVER_DOESNT_OWN_SERVICE);
    if (ret == -1) {
        std::cout << "添加Rpc服务失败！\n";
        return -1;
    }
    //4. 启动服务器
    brpc::ServerOptions options;
    options.idle_timeout_sec = -1; //连接空闲超时时间-超时后连接被关闭
    options.num_threads = 1; // io线程数量
    ret = server.Start(8080, &options);
    if (ret == -1) {
        std::cout << "启动服务器失败！\n";
        return -1;
    }
    server.RunUntilAskedToQuit();//修改等待运行结束
    return 0;
}