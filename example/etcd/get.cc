#include <etcd/Client.hpp>
#include <etcd/KeepAlive.hpp>
#include <etcd/Response.hpp>
#include <etcd/Watcher.hpp>
#include <etcd/Value.hpp>
#include <thread>

void callback(const etcd::Response &resp) {
    if (resp.is_ok() == false) {
        std::cout << "收到一个错误的事件通知:" << resp.error_message() << std::endl;
        return;
    }
    for (auto const& ev : resp.events()) {
        if (ev.event_type() == etcd::Event::EventType::PUT) {
            std::cout << "服务信息发生了改变:\n" ;
            std::cout << "当前的值：" << ev.kv().key() << "-" << ev.kv().as_string() << std::endl;
            std::cout << "原来的值：" << ev.prev_kv().key() << "-" << ev.prev_kv().as_string() << std::endl;
        }else if (ev.event_type() == etcd::Event::EventType::DELETE_) {
            std::cout << "服务信息下线被删除:\n";
            std::cout << "当前的值：" << ev.kv().key() << "-" << ev.kv().as_string() << std::endl;
            std::cout << "原来的值：" << ev.prev_kv().key() << "-" << ev.prev_kv().as_string() << std::endl;
        }
    }
}

int main(int argc, char *argv[])
{
    std::string etcd_host = "http://127.0.0.1:2379";
    //实例化客户端对象
    etcd::Client client(etcd_host);
    //获取指定的键值对信息
    auto resp = client.ls("/service").get();
    if (resp.is_ok() == false) {
        std::cout << "获取键值对数据失败: " << resp.error_message() << std::endl;
        return -1;
    }
    int sz = resp.keys().size();
    for (int i = 0; i < sz; ++i) {
        std::cout << resp.value(i).as_string() << "可以提供" << resp.key(i) << "服务\n";
    }
    //实例化一个键值对事件监控对象
    auto watcher = etcd::Watcher(client, "/service", callback, true);
    watcher.Wait();
    return 0;
}