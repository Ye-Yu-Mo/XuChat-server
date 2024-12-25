#include <cinatra.hpp>
#include <iostream>
async_simple::coro::Lazy<void> test()
{
    cinatra::coro_http_client client{};
    auto r = co_await client.connect("ws://127.0.0.1:8888");
    if (r.net_err)
    {
        std::cout << "连接websocket服务器失败" << std::endl;
        co_return;
    }

    auto result = co_await client.write_websocket("这里是websocket客户端连接");
    assert(!result.net_err);
    auto data = co_await client.read_websocket();
    std::cout << "收到响应: " << data.resp_body << std::endl;
    result = co_await client.write_websocket("第二次测试");
    assert(!result.net_err);
    data = co_await client.read_websocket();
    std::cout << "收到响应: " << data.resp_body << std::endl;
}

int main()
{
    async_simple::coro::syncAwait(test());
    // test();
    return 0;
}