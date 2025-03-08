#include <ev.h>
#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <openssl/ssl.h>
#include <openssl/opensslv.h>

//消息回调处理函数的实现
void MessageCb(AMQP::TcpChannel *channel, const AMQP::Message &message, uint64_t deliveryTag, bool redelivered)
{
    std::string msg;
    msg.assign(message.body(), message.bodySize());
    std::cout << msg << std::endl;
    channel->ack(deliveryTag); // 对消息进行确认
}

int main()
{
    //1. 实例化底层网络通信框架的I/O事件监控句柄
    auto *loop = EV_DEFAULT;
    //2. 实例化libEvHandler句柄 --- 将AMQP框架与事件监控关联起来
    AMQP::LibEvHandler handler(loop);
    //2.5. 实例化连接对象
    AMQP::Address address("amqp://root:123456@127.0.0.1:5672/");
    AMQP::TcpConnection connection(&handler, address);
    //3. 实例化信道对象
    AMQP::TcpChannel channel(&connection);
    //4. 声明交换机
    channel.declareExchange("test-exchange", AMQP::ExchangeType::direct)
        .onError([](const char *message) {
            std::cout << "声明交换机失败：" << message << std::endl;
            exit(0);
        })
        .onSuccess([](){
            std::cout << "test-exchange 交换机创建成功！" << std::endl;
        });
    //5. 声明队列
    channel.declareQueue("test-queue")
        .onError([](const char *message) {
            std::cout << "声明队列失败：" << message << std::endl;
            exit(0);
        })
        .onSuccess([](){
            std::cout << "test-queue 队列创建成功！" << std::endl;
        });
    //6. 针对交换机和队列进行绑定
    channel.bindQueue("test-exchange", "test-queue", "test-queue-key")
        .onError([](const char *message) {
            std::cout << "test-exchange - test-queue 绑定失败：" << message << std::endl;
            exit(0);
        })
        .onSuccess([](){
            std::cout << "test-exchange - test-queue 绑定成功！" << std::endl;
        });
    //7. 订阅队列消息 -- 设置消息处理回调函数
    auto callback = std::bind(MessageCb, &channel, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    channel.consume("test-queue", "consume-tag")  //返回值 DeferredConsumer
        .onReceived(callback)
        .onError([](const char *message){
            std::cout << "订阅 test-queue 队列消息失败:" << message << std::endl;
            exit(0);
        }); // 返回值是 AMQP::Deferred
    
    
    //8. 启动底层网络通信框架--开启I/O
    ev_run(loop, 0);
    return 0;
}