#include <cinatra.hpp>
#include <iostream>

// 协程函数的启动方式

// 1. co_await 启动
// 2. 通过协程函数的start(callback)方法启动
// 例如: Lazy<> task2()
// task().start([](auto&&){});
// 调用之后会立即开始执行协程函数 执行完成之后会将结果传入callback方法中进行处理
// 3. 通过协程函数的directlyStart(callback, executor)方法启动
// 和start类似，但是提供了调度器接口，用于在启动协程的时候绑定调度器
// 需要注意的是 directlyStart 不会在启动时立即调度协程
// 参考 https://alibaba.github.io/async_simple/docs.cn/Lazy.html

async_simple::coro::Lazy<void> test_websocket()
{
    std::cout << "test_websocket start" << std::endl;
    cinatra::coro_http_server server(1, 8888); // 核心线程数和监听端口

    server.set_http_handler<cinatra::GET>(
        "/ws_echo",
        [](cinatra::coro_http_request &req, cinatra::coro_http_response &resp)
            -> async_simple::coro::Lazy<void>
        {
            cinatra::websocket_result result{};
            while (true)
            {
                result = co_await req.get_conn()->read_websocket();
                if (result.ec)
                {
                    std::cout << "从websocket客户端读取信息失败" << std::endl;
                    break;
                }
                if (result.type == cinatra::ws_frame_type::WS_TEXT_FRAME ||
                    result.type == cinatra::ws_frame_type::WS_BINARY_FRAME)
                {
                    std::cout << "读取到文本或者二进制" << std::endl;
                    std::cout << result.data << std::endl;
                }
                else if (result.type == cinatra::ws_frame_type::WS_PING_FRAME ||
                         result.type == cinatra::ws_frame_type::WS_PONG_FRAME)
                {
                    std::cout << "读取到心跳包" << std::endl;
                    continue;
                }
                else
                {
                    std::cout << "读取到未知消息" << std::endl;
                    break;
                }

                auto ec = co_await req.get_conn()->write_websocket(result.data);
                if (ec)
                {
                    std::cout << "向websocket写入消息失败" << std::endl;
                    break;
                }
            }
        });
    server.async_start();
    std::this_thread::sleep_for(std::chrono::seconds(10000));
}

int main()
{
    async_simple::coro::syncAwait(test_websocket());
    return 0;
}