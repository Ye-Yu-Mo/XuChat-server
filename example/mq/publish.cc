#include <ev.h>
#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <openssl/ssl.h>
#include <openssl/opensslv.h>

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
    //7. 向交换机发布消息
    for (int i = 0; i < 10; i++) {
        std::string msg = "Hello Bite-" + std::to_string(i);
        bool ret = channel.publish("test-exchange", "test-queue-key", msg);
        if (ret == false) {
            std::cout << "publish 失败！\n";
        }
    }
    //启动底层网络通信框架--开启I/O
    ev_run(loop, 0);
    return 0;
}