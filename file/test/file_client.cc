//编写一个file客户端程序，对文件存储子服务进行单元测试
// 1. 封装四个接口进行rpc调用，实现对于四个业务接口的测试
#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <thread>
#include "etcd.hpp"
#include "channel.hpp"
#include "logger.hpp"
#include "file.pb.h"
#include "base.pb.h"
#include "utils.hpp"


DEFINE_bool(run_mode, false, "程序的运行模式，false-调试； true-发布；");
DEFINE_string(log_file, "", "发布模式下，用于指定日志的输出文件");
DEFINE_int32(log_level, 0, "发布模式下，用于指定日志输出等级");

DEFINE_string(etcd_host, "http://127.0.0.1:2379", "服务注册中心地址");
DEFINE_string(base_service, "/service", "服务监控根目录");
DEFINE_string(file_service, "/service/file_service", "服务监控根目录");


XuChat::ServiceChannel::ChannelPtr channel;
std::string single_file_id;

TEST(put_test, single_file) {
    //1. 读取当前目录下的指定文件数据
    std::string body;
    ASSERT_TRUE(XuChat::readFile("./Makefile", body));
    //2. 实例化rpc调用客户端对象，发起rpc调用
    XuChat::FileService_Stub stub(channel.get());

    XuChat::PutSingleFileReq req;
    req.set_request_id("1111");
    req.mutable_file_data()->set_file_name("Makefile");
    req.mutable_file_data()->set_file_size(body.size());
    req.mutable_file_data()->set_file_content(body);

    brpc::Controller *cntl = new brpc::Controller();
    XuChat::PutSingleFileRsp *rsp = new XuChat::PutSingleFileRsp();
    stub.PutSingleFile(cntl, &req, rsp, nullptr);
    ASSERT_FALSE(cntl->Failed());
    //3. 检测返回值中上传是否成功
    ASSERT_TRUE(rsp->success());
    ASSERT_EQ(rsp->file_info().file_size(), body.size());
    ASSERT_EQ(rsp->file_info().file_name(), "Makefile");
    single_file_id = rsp->file_info().file_id();
}

TEST(get_test, single_file) {
    //先发起Rpc调用，进行文件下载
    XuChat::FileService_Stub stub(channel.get());
    XuChat::GetSingleFileReq req;
    XuChat::GetSingleFileRsp *rsp;
    req.set_request_id("2222");
    req.set_file_id(single_file_id);

    brpc::Controller *cntl = new brpc::Controller();
    rsp = new XuChat::GetSingleFileRsp();
    stub.GetSingleFile(cntl, &req, rsp, nullptr);
    ASSERT_FALSE(cntl->Failed());
    ASSERT_TRUE(rsp->success());
    //将文件数据，存储到文件中
    ASSERT_EQ(single_file_id, rsp->file_data().file_id());
    XuChat::writeFile("make_file_download", rsp->file_data().file_content());
}

std::vector<std::string> multi_file_id;

TEST(put_test, multi_file) {
    //1. 读取当前目录下的指定文件数据
    std::string body1;
    ASSERT_TRUE(XuChat::readFile("./base.pb.h", body1));
    std::string body2;
    ASSERT_TRUE(XuChat::readFile("./file.pb.h", body2));
    //2. 实例化rpc调用客户端对象，发起rpc调用
    XuChat::FileService_Stub stub(channel.get());

    XuChat::PutMultiFileReq req;
    req.set_request_id("3333");

    auto file_data = req.add_file_data();
    file_data->set_file_name("base.pb.h");
    file_data->set_file_size(body1.size());
    file_data->set_file_content(body1);

    file_data = req.add_file_data();
    file_data->set_file_name("file.pb.h");
    file_data->set_file_size(body2.size());
    file_data->set_file_content(body2);

    brpc::Controller *cntl = new brpc::Controller();
    XuChat::PutMultiFileRsp *rsp = new XuChat::PutMultiFileRsp();
    stub.PutMultiFile(cntl, &req, rsp, nullptr);
    ASSERT_FALSE(cntl->Failed());
    //3. 检测返回值中上传是否成功
    ASSERT_TRUE(rsp->success());
    for (int i = 0; i < rsp->file_info_size(); i++){
        multi_file_id.push_back(rsp->file_info(i).file_id());
    }
}

TEST(get_test, multi_file) {
    //先发起Rpc调用，进行文件下载
    XuChat::FileService_Stub stub(channel.get());
    XuChat::GetMultiFileReq req;
    XuChat::GetMultiFileRsp *rsp;
    req.set_request_id("4444");
    req.add_file_id_list(multi_file_id[0]);
    req.add_file_id_list(multi_file_id[1]);

    brpc::Controller *cntl = new brpc::Controller();
    rsp = new XuChat::GetMultiFileRsp();
    stub.GetMultiFile(cntl, &req, rsp, nullptr);
    ASSERT_FALSE(cntl->Failed());
    ASSERT_TRUE(rsp->success());
    //将文件数据，存储到文件中
    ASSERT_TRUE(rsp->file_data().find(multi_file_id[0]) != rsp->file_data().end());
    ASSERT_TRUE(rsp->file_data().find(multi_file_id[1]) != rsp->file_data().end());
    auto map = rsp->file_data();
    auto file_data1 = map[multi_file_id[0]];
    XuChat::writeFile("base_download_file1",file_data1.file_content());
    auto file_data2 = map[multi_file_id[1]];
    XuChat::writeFile("file_download_file2", file_data2.file_content());
}


int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    google::ParseCommandLineFlags(&argc, &argv, true);


    //1. 先构造Rpc信道管理对象
    auto sm = std::make_shared<XuChat::ServiceManager>();
    sm->declared(FLAGS_file_service);
    auto put_cb = std::bind(&XuChat::ServiceManager::onServiceOnline, sm.get(), std::placeholders::_1, std::placeholders::_2);
    auto del_cb = std::bind(&XuChat::ServiceManager::onServiceOffline, sm.get(), std::placeholders::_1, std::placeholders::_2);
    //2. 构造服务发现对象
    XuChat::Discovery::ptr dclient = std::make_shared<XuChat::Discovery>(FLAGS_etcd_host, FLAGS_base_service, put_cb, del_cb);
    
    //3. 通过Rpc信道管理对象，获取提供Echo服务的信道
    channel = sm->choose(FLAGS_file_service);
    if (!channel) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return -1;
    }

    return RUN_ALL_TESTS();
}