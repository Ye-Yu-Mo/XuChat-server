#include <elasticlient/client.h>
#include <cpr/cpr.h>
#include <iostream>

int main()
{
    //1. 构造ES客户端
    elasticlient::Client client({"http://127.0.0.1:9200/"});
    //2. 发起搜索请求
    try {
        auto rsp = client.search("user", "_doc", "{\"query\": { \"match_all\":{} }}");
        std::cout << rsp.status_code << std::endl;
        std::cout << rsp.text << std::endl;
    } catch(std::exception &e) {
        std::cout << "请求失败：" << e.what() << std::endl;
        return -1;
    }
    //3. 打印响应状态码和响应正文
    return 0;
}