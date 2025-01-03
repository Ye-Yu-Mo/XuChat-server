#include "speech_server.hpp"
#include <cstdlib>
DEFINE_string(registry_host, "http://127.0.0.1:2379", "服务注册中心Host");
DEFINE_string(base_service, "/service", "服务监控根目录");
DEFINE_string(instance_name, "/speech/instance_id ", "当前服务实例");

DEFINE_string(access_host, "127.0.0.1:7070", "当前服务实例的外部访问地址");
DEFINE_int32(listen_port, 7070, "RPC服务器监听端口");
DEFINE_int32(rpc_timeout, -1, "RPC服务器超时时间");
DEFINE_int32(rpc_threads, 1, "RPC服务器IO线程数量");

DEFINE_string(app_id,  getenv("BAIDU_APP_ID"), "应用ID");
DEFINE_string(api_key, getenv("BAIDU_API_KEY"), "API密钥");
DEFINE_string(secret_key, getenv("BAIDU_API_SECRET"), "私钥");

int main(int argc, char* argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    
    XuChat::SpeechServerBuilder builder;
    builder.makeAsrClient(FLAGS_app_id, FLAGS_api_key, FLAGS_secret_key);
    builder.makeRpcServer(FLAGS_listen_port, FLAGS_rpc_timeout, FLAGS_rpc_threads);
    builder.makeRegClient(FLAGS_registry_host, FLAGS_base_service+FLAGS_instance_name, FLAGS_access_host);
    XuChat::SpeechServer::ptr server = builder.build();
    server->start();
    return 0;
}