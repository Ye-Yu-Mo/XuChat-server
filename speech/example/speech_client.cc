// speech_server 客户端样例
#include "channel.hpp"
#include "etcd.hpp"
#include "speech.pb.h"
#include "speech.h"
#include <brpc/channel.h>
#include <gflags/gflags.h>

DEFINE_string(etcd_host, "http://127.0.0.1:2379", "注册中心地址");
DEFINE_string(base_service, "/service", "服务监控根目录");

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    auto sm = std::make_shared<Common::ServiceManager>();
    sm->declared("/service/speech_service");
    auto online = std::bind(&Common::ServiceManager::serviceOnline, sm.get(), std::placeholders::_1, std::placeholders::_2);
    auto offline = std::bind(&Common::ServiceManager::serviceOffline, sm.get(), std::placeholders::_1, std::placeholders::_2);
    Common::Discovery::ptr client = std::make_shared<Common::Discovery>(FLAGS_etcd_host, FLAGS_base_service, online, offline);
    Common::ServiceChannel::ChannelPtr channel = channel = sm->choose("/service/speech_service");
    // std::cout << "to here" << std::endl;

    if (!channel)
    {
        channel = sm->choose("/service/speech");
        std::this_thread::sleep_for(std::chrono::seconds(1));
        abort();
    }

    std::string file_content;
    aip::get_file_content("16k.pcm", &file_content);


    XuChat::SpeechService stub(channel.get());
    std::cout << "输入:";
    std::string buffer;
    std::cin >> buffer;
    XuChat::SpeechRecognitionReq req;
    req.set_speech_content(file_content);
    req.set_request_id("11");

    brpc::Controller *cntl = new brpc::Controller();
    XuChat::SpeechRecognitionRsp *resp = new XuChat::SpeechRecognitionRsp();
    stub.SpeechRecognition(cntl, &req, resp, nullptr);
    if (cntl->Failed())
    {
        std::cout << "rpc调用失败" << std::endl;
        return -1;
    }
    std::cout << "收到响应：" << resp->request_id() << " " << resp->recognition_result() << std::endl;
    delete cntl;
    delete resp;

    std::this_thread::sleep_for(std::chrono::seconds(600));
    return 0;
}