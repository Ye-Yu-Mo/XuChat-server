#include <brpc/channel.h>
#include <thread>
#include "main.pb.h"

void callback(brpc::Controller* cntl, ::example::EchoResponse* response) {
    std::unique_ptr<brpc::Controller> cntl_guard(cntl);
    std::unique_ptr<example::EchoResponse> resp_guard(response);
    if (cntl->Failed() == true) {
        std::cout << "Rpc调用失败：" << cntl->ErrorText() << std::endl;
        return;
    }
    std::cout << "收到响应: " << response->message() << std::endl;
}

int main(int argc, char *argv[])
{
    //1. 构造Channel信道，连接服务器
    brpc::ChannelOptions options;
    options.connect_timeout_ms = -1;// 连接等待超时时间，-1表示一直等待
    options.timeout_ms = -1; //rpc请求等待超时时间，-1表示一直等待
    options.max_retry = 3;//请求重试次数
    options.protocol = "baidu_std"; //序列化协议，默认使用baidu_std
    brpc::Channel channel;
    int ret = channel.Init("127.0.0.1:8080", &options);
    if (ret == -1) {
        std::cout << "初始化信道失败！\n";
        return -1;
    }
    //2. 构造EchoService_Stub对象，用于进行rpc调用
    example::EchoService_Stub stub(&channel);
    //3. 进行Rpc调用/
    example::EchoRequest req;
    req.set_message("你好~比特~！");

    brpc::Controller *cntl = new brpc::Controller();
    example::EchoResponse *rsp = new example::EchoResponse();
    // stub.Echo(cntl, &req, rsp, nullptr);
    // if (cntl->Failed() == true) {
    //     std::cout << "Rpc调用失败：" << cntl->ErrorText() << std::endl;
    //     return -1;
    // }
    // std::cout << "收到响应: " << rsp->message() << std::endl;
    // delete cntl;
    // delete rsp;
    auto clusure = google::protobuf::NewCallback(callback, cntl, rsp);
    stub.Echo(cntl, &req, rsp, clusure); //异步调用
    std::cout << "异步调用结束！\n";
    std::this_thread::sleep_for(std::chrono::seconds(3));
    return 0;
}