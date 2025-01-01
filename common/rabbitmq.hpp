#include <ev.h>
#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <openssl/ssl.h>
#include <openssl/opensslv.h>
#include "logger.hpp"
#define DEFAULT_ROUTING_KEY "default_routing"
namespace Common
{
    class MQClient
    {
    public:
        using ptr = std::shared_ptr<MQClient>;
        using MessageCallback = std::function<void(const char *, size_t)>;
        /// @brief 消息队列构造函数
        /// @param user 用户名
        /// @param passwd 密码
        /// @param host RabbitMQ 服务端主机 127.0.0.1:5672
        MQClient(const std::string &user,
                 const std::string &passwd,
                 const std::string &host)
            : _loop(EV_DEFAULT)
        {
            _handler = std::make_unique<AMQP::LibEvHandler>(_loop);
            std::string url = "amqp://" + user + ":" + passwd + "@" + host + "/";
            _connection = std::make_unique<AMQP::TcpConnection>(_handler.get(), AMQP::Address(url.c_str()));
            _channel = std::make_unique<AMQP::TcpChannel>(_connection.get());
            _loop_thread = std::thread([this]()
                                       { ev_run(_loop, 0); });
        }

        void declareComponents(const std::string &exchange,
                               const std::string &queue,
                               const std::string &routing_key = DEFAULT_ROUTING_KEY,
                               AMQP::ExchangeType exchange_type = AMQP::ExchangeType::direct)
        {
            _channel->declareExchange(exchange, exchange_type)
                .onError([&exchange](const char *message)
                         {
                            fatal(logger, "交换机 %s 声明失败! %s", exchange.c_str(), message);
                            exit(0); })
                .onSuccess([&exchange]()
                           { info(logger, "交换机 %s 声明成功!", exchange.c_str()); });
            _channel->declareQueue(queue)
                .onError([&queue](const char *message)
                         {
                            fatal(logger, "队列 %s 声明失败! %s", queue.c_str(), message);
                            exit(0); })
                .onSuccess([&queue]()
                           { info(logger, "队列 %s 声明成功!", queue.c_str()); });
            _channel->bindQueue(exchange, queue, routing_key)
                .onError([&exchange, &queue](const char *message)
                         {
                            fatal(logger, "%s-%s 绑定失败! %s", exchange.c_str(), queue.c_str(), message);
                            exit(0); })
                .onSuccess([&exchange, &queue]()
                           { info(logger, "%s-%s 绑定成功!", exchange.c_str(), queue.c_str()); });
        }

        bool publish(const std::string &exchange,
                     const std::string &msg,
                     const std::string &routing_key = DEFAULT_ROUTING_KEY)
        {
            bool ret = _channel->publish(exchange, routing_key, msg);
            if (!ret)
            {
                log_error(logger, "消息发送失败! exchange: %s, routing_key: %s, msg: %s", exchange.c_str(), routing_key.c_str(), msg.c_str());
                return false;
            }
            return true;
        }

        void consume(const std::string &queue,
                     const MessageCallback &cb)
        {
            _channel->consume(queue, "consumer_tag")
                .onReceived([this, &cb](const AMQP::Message &message, uint64_t deliveryTag, bool redelivered)
                            {
                    cb(message.body(), message.bodySize());
                    _channel->ack(deliveryTag); })
                .onError([&queue](const char *message)
                         { log_error(logger, "订阅消息失败! queue: %s, reason: %s", queue.c_str(), message); });
        }

        ~MQClient()
        {
            struct ev_async async_watcher;
            ev_async_init(&async_watcher, watcher_callback);
            ev_async_start(_loop, &async_watcher);
            ev_async_send(_loop, &async_watcher);
            _loop_thread.join();
        }

    private:
        static void watcher_callback(struct ev_loop *loop, ev_async *watcher, int32_t revents)
        {
            ev_break(loop, EVBREAK_ALL);
        }

    private:
        
        struct ev_loop *_loop;
        std::unique_ptr<AMQP::LibEvHandler> _handler;
        std::unique_ptr<AMQP::TcpConnection> _connection;
        std::unique_ptr<AMQP::TcpChannel> _channel;
        std::thread _loop_thread;
    };
}