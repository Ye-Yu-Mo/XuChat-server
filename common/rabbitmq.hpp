#pragma once
#include <ev.h>
#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <openssl/ssl.h>
#include <openssl/opensslv.h>
#include <iostream>
#include <functional>
#include "logger.hpp"

namespace XuChat
{
    class MQClient
    {
    public:
        using MessageCallback = std::function<void(const char *, size_t)>;
        using ptr = std::shared_ptr<MQClient>;
        MQClient(const std::string &user,
                 const std::string passwd,
                 const std::string host)
        {
            _loop = EV_DEFAULT;
            _handler = std::make_unique<AMQP::LibEvHandler>(_loop);
            // amqp://root:123456@127.0.0.1:5672/
            std::string url = "amqp://" + user + ":" + passwd + "@" + host + "/";
            AMQP::Address address(url);
            _connection = std::make_unique<AMQP::TcpConnection>(_handler.get(), address);
            _channel = std::make_unique<AMQP::TcpChannel>(_connection.get());

            _loop_thread = std::thread([this]()
                                       { ev_run(_loop, 0); });
        }
        ~MQClient()
        {
            ev_async_init(&_async_watcher, watcher_callback);
            ev_async_start(_loop, &_async_watcher);
            ev_async_send(_loop, &_async_watcher);
            _loop_thread.join();
            _loop = nullptr;
        }
        void declareComponents(const std::string &exchange,
                               const std::string &queue,
                               const std::string &routing_key = "routing_key",
                               AMQP::ExchangeType echange_type = AMQP::ExchangeType::direct)
        {
            _channel->declareExchange(exchange, echange_type)
                .onError([](const char *message)
                         {
                    log_error(logger, "声明交换机失败：%s", message);
                    exit(0); })
                .onSuccess([exchange]()
                           { log_error(logger, "%s 交换机创建成功！", exchange.c_str()); });
            _channel->declareQueue(queue)
                .onError([](const char *message)
                         {
                    log_error(logger, "声明队列失败：%s", message);
                    exit(0); })
                .onSuccess([queue]()
                           { log_error(logger, "%s 队列创建成功！", queue.c_str()); });
            // 6. 针对交换机和队列进行绑定
            _channel->bindQueue(exchange, queue, routing_key)
                .onError([exchange, queue](const char *message)
                         {
                    log_error(logger, "%s - %s 绑定失败：", exchange.c_str(), queue.c_str());
                    exit(0); })
                .onSuccess([exchange, queue, routing_key]()
                           { log_error(logger, "%s - %s - %s 绑定成功！", exchange.c_str(), queue.c_str(), routing_key.c_str()); });
        }
        bool publish(const std::string &exchange,
                     const std::string &msg,
                     const std::string &routing_key = "routing_key")
        {
            debug(logger, "向交换机 %s-%s 发布消息！", exchange.c_str(), routing_key.c_str());
            bool ret = _channel->publish(exchange, routing_key, msg);
            if (ret == false)
            {
                log_error(logger, "%s 发布消息失败：", exchange.c_str());
                return false;
            }
            return true;
        }
        void consume(const std::string &queue, const MessageCallback &cb)
        {
            debug(logger, "开始订阅 %s 队列消息！", queue.c_str());
            _channel->consume(queue, "consume-tag") // 返回值 DeferredConsumer
                .onReceived([this, cb](const AMQP::Message &message,
                                       uint64_t deliveryTag,
                                       bool redelivered)
                            {
                    cb(message.body(), message.bodySize());
                    _channel->ack(deliveryTag); })
                .onError([queue](const char *message)
                         {
                    log_error(logger, "订阅 %s 队列消息失败: %s", queue.c_str(), message);
                    exit(0); });
        }

    private:
        static void watcher_callback(struct ev_loop *loop, ev_async *watcher, int32_t revents)
        {
            ev_break(loop, EVBREAK_ALL);
        }

    private:
        struct ev_async _async_watcher;
        struct ev_loop *_loop;
        std::unique_ptr<AMQP::LibEvHandler> _handler;
        std::unique_ptr<AMQP::TcpConnection> _connection;
        std::unique_ptr<AMQP::TcpChannel> _channel;
        std::thread _loop_thread;
    };
}