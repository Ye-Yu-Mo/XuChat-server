// #include <ev.h>
// #include <amqpcpp.h>
// #include <amqpcpp/libev.h>
// #include <openssl/ssl.h>
// #include <openssl/opensslv.h>
#include "../../common/rabbitmq.hpp"

int main()
{
    // // 实例化底层网络通信框架的IO事件监控句柄
    // auto *loop = EV_DEFAULT;
    // // 实例化libEvHandler句柄 将AMQP框架和事件监控关联起来
    // AMQP::LibEvHandler handler(loop);
    // // 实例化网络连接对象
    // AMQP::Address address("amqp://root:123456@127.0.0.1:5672");
    // AMQP::TcpConnection connection(&handler, address);
    // // 信道对象
    // AMQP::TcpChannel channel(&connection);
    // // 声明交换机
    // channel.declareExchange("exchange1", AMQP::ExchangeType::direct)
    //     .onError([](const char *message)
    //              {
    //     std::cout << "声明交换机失败" << std::string(message) << std::endl;
    //     exit(0); })
    //     .onSuccess([]()
    //                {
    //     std::cout << "声明交换机成功" << std::endl;
    //     });
    // // 声明队列
    // channel.declareQueue("queue1")
    //     .onError([](const char *message)
    //              {
    //     std::cout << "声明queue1失败" << std::string(message) << std::endl;
    //     exit(0); })
    //     .onSuccess([]()
    //                {
    //     std::cout << "声明queue1成功" << std::endl;
    //     });
    // channel.declareQueue("queue2")
    //     .onError([](const char *message)
    //              {
    //     std::cout << "声明queue2失败" << std::string(message) << std::endl;
    //     exit(0); })
    //     .onSuccess([]()
    //                {
    //     std::cout << "声明queue2成功" << std::endl;
    //     });
    // // 绑定交换机和队列
    // channel.bindQueue("exchange1", "queue1", "queue1");
    // channel.bindQueue("exchange1", "queue2", "queue2");
    // // 向交换机发布消息
    // for (int i = 0; i < 10; i++)
    // {
    //     bool ret = channel.publish("exchange1", "queue1", std::to_string(i).c_str());
    //     if (!ret)
    //     {
    //         std::cout << "publish 失败" << std::endl;
    //     }
    // }
    // ev_run(loop, 0);

    Common::MQClient client("root", "123456", "127.0.0.1:5672");

    client.declareComponents("exchange1", "queue1");
    client.publish("exchange1", "hello world!");
    std::this_thread::sleep_for(std::chrono::seconds(60));

    return 0;
}