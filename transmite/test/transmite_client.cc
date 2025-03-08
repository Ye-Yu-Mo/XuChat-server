//speech_server的测试客户端实现
//1. 进行服务发现--发现speech_server的服务器节点地址信息并实例化的通信信道
//2. 读取语音文件数据
//3. 发起语音识别RPC调用

#include "etcd.hpp"
#include "channel.hpp"
#include "utils.hpp"
#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <thread>
#include "transmite.pb.h"


DEFINE_bool(run_mode, false, "程序的运行模式，false-调试； true-发布；");
DEFINE_string(log_file, "", "发布模式下，用于指定日志的输出文件");
DEFINE_int32(log_level, 0, "发布模式下，用于指定日志输出等级");

DEFINE_string(etcd_host, "http://127.0.0.1:2379", "服务注册中心地址");
DEFINE_string(base_service, "/service", "服务监控根目录");
DEFINE_string(transmite_service, "/service/transmite_service", "服务监控根目录");

XuChat::ServiceManager::ptr sm;

void string_message(const std::string &uid, const std::string &sid, const std::string &msg) {
    auto channel = sm->choose(FLAGS_transmite_service);
    if (!channel) {
        std::cout << "获取通信信道失败！" << std::endl;
        return;
    }
    XuChat::MsgTransmitService_Stub stub(channel.get());
    XuChat::NewMessageReq req;
    XuChat::GetTransmitTargetRsp rsp;
    req.set_request_id(XuChat::uuid());
    req.set_user_id(uid);
    req.set_chat_session_id(sid);
    req.mutable_message()->set_message_type(XuChat::MessageType::STRING);
    req.mutable_message()->mutable_string_message()->set_content(msg);
    brpc::Controller cntl;
    stub.GetTransmitTarget(&cntl, &req, &rsp, nullptr);
    ASSERT_FALSE(cntl.Failed());
    ASSERT_TRUE(rsp.success());
}
void image_message(const std::string &uid, const std::string &sid, const std::string &msg) {
    auto channel = sm->choose(FLAGS_transmite_service);
    if (!channel) {
        std::cout << "获取通信信道失败！" << std::endl;
        return;
    }
    XuChat::MsgTransmitService_Stub stub(channel.get());
    XuChat::NewMessageReq req;
    XuChat::GetTransmitTargetRsp rsp;
    req.set_request_id(XuChat::uuid());
    req.set_user_id(uid);
    req.set_chat_session_id(sid);
    req.mutable_message()->set_message_type(XuChat::MessageType::IMAGE);
    req.mutable_message()->mutable_image_message()->set_image_content(msg);
    brpc::Controller cntl;
    stub.GetTransmitTarget(&cntl, &req, &rsp, nullptr);
    ASSERT_FALSE(cntl.Failed());
    ASSERT_TRUE(rsp.success());
}

void speech_message(const std::string &uid, const std::string &sid, const std::string &msg) {
    auto channel = sm->choose(FLAGS_transmite_service);
    if (!channel) {
        std::cout << "获取通信信道失败！" << std::endl;
        return;
    }
    XuChat::MsgTransmitService_Stub stub(channel.get());
    XuChat::NewMessageReq req;
    XuChat::GetTransmitTargetRsp rsp;
    req.set_request_id(XuChat::uuid());
    req.set_user_id(uid);
    req.set_chat_session_id(sid);
    req.mutable_message()->set_message_type(XuChat::MessageType::SPEECH);
    req.mutable_message()->mutable_speech_message()->set_file_contents(msg);
    brpc::Controller cntl;
    stub.GetTransmitTarget(&cntl, &req, &rsp, nullptr);
    ASSERT_FALSE(cntl.Failed());
    ASSERT_TRUE(rsp.success());
}

void file_message(const std::string &uid, const std::string &sid, 
    const std::string &filename, const std::string &content) {
    auto channel = sm->choose(FLAGS_transmite_service);
    if (!channel) {
        std::cout << "获取通信信道失败！" << std::endl;
        return;
    }
    XuChat::MsgTransmitService_Stub stub(channel.get());
    XuChat::NewMessageReq req;
    XuChat::GetTransmitTargetRsp rsp;
    req.set_request_id(XuChat::uuid());
    req.set_user_id(uid);
    req.set_chat_session_id(sid);
    req.mutable_message()->set_message_type(XuChat::MessageType::FILE);
    req.mutable_message()->mutable_file_message()->set_file_contents(content);
    req.mutable_message()->mutable_file_message()->set_file_name(filename);
    req.mutable_message()->mutable_file_message()->set_file_size(content.size());
    brpc::Controller cntl;
    stub.GetTransmitTarget(&cntl, &req, &rsp, nullptr);
    ASSERT_FALSE(cntl.Failed());
    ASSERT_TRUE(rsp.success());
}

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    
    //1. 先构造Rpc信道管理对象
    sm = std::make_shared<XuChat::ServiceManager>();
    sm->declared(FLAGS_transmite_service);
    auto put_cb = std::bind(&XuChat::ServiceManager::onServiceOnline, sm.get(), std::placeholders::_1, std::placeholders::_2);
    auto del_cb = std::bind(&XuChat::ServiceManager::onServiceOffline, sm.get(), std::placeholders::_1, std::placeholders::_2);
    //2. 构造服务发现对象
    XuChat::Discovery::ptr dclient = std::make_shared<XuChat::Discovery>(FLAGS_etcd_host, FLAGS_base_service, put_cb, del_cb);
    
    //3. 通过Rpc信道管理对象，获取提供Echo服务的信道
    // string_message("731f-50086884-0000", "会话ID1", "吃饭了吗？");
    // string_message("c4dc-68239a9a-0001", "会话ID1", "吃的盖浇饭！！");
    // image_message("731f-50086884-0000", "会话ID1", "可爱表情图片数据");
    // speech_message("731f-50086884-0000", "会话ID1", "动听猪叫声数据");
    file_message("731f-50086884-0000", "0d90-755571d8-0003", "猪爸爸的文件名称", "猪爸爸的文件数据");
    return 0;
}