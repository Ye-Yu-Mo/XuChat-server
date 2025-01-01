// #include <ev.h>
// #include <amqpcpp.h>
// #include <amqpcpp/libev.h>
// #include <openssl/ssl.h>
// #include <openssl/opensslv.h>
#include <functional>
#include "../../common/rabbitmq.hpp"

// 消息回调处理函数
// void CallBack(AMQP::TcpChannel *channel, const AMQP::Message &message, uint64_t deliveryTag, bool redelivered)
// {
//     std::string msg;
//     msg.assign(message.body(), message.bodySize());
//     std::cout << msg << std::endl;
//     channel->ack(deliveryTag); // 消息确认
// }

void callback(const char * msg, size_t size){
    std::string message;
    message.assign(msg, size);
    info(Common::logger, "message: %s, size: %d", message.c_str(), size);
}

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
    //    });
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
    // channel.consume("queue1", "consumer_tag")
    //     .onReceived(std::bind(&CallBack, &channel, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
    //     .onError([](const char *message)
    //              { std::cout << "订阅 queue1 失败\n"; });
    // ev_run(loop, 0);

    Common::MQClient client("root", "123456", "127.0.0.1:5672");
    client.declareComponents("exchange1", "queue1");
    client.consume("queue1", callback);
    std::this_thread::sleep_for(std::chrono::seconds(60));
    return 0;
}