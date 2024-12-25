#include <sw/redis++/redis.h>
#include <iostream>
void add_string(sw::redis::Redis &client)
{
    client.set("key", "value");
    client.set("会话1", "用户1");
    client.set("会话2", "用户2");
}

void delete_string(sw::redis::Redis &client)
{
    client.del("key");
}

void get_string(sw::redis::Redis &client)
{
    auto res = client.get("会话1");
    if (res)
        std::cout << *res << std::endl;
    res = client.get("会话2");
    if (res)
        std::cout << *res << std::endl;
    res = client.get("key");
    if (res)
        std::cout << *res << std::endl;
}

int main()
{
    // 实例化redis对象 构造连接选项 连接服务器
    sw::redis::ConnectionOptions opts;
    opts.host = "127.0.0.1"; // redis服务器ip地址
    opts.port = 6379;        // redis服务器端口号
    opts.db = 0;             // 数据库编号
    opts.keep_alive = true;  // 是否保活

    sw::redis::Redis client(opts);
    // 添加键值对字符串
    add_string(client);
    get_string(client);
    delete_string(client);
    get_string(client);
    // 控制数据有效时间
    // 列表操作
    return 0;
}