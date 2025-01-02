#include "../../third/aip-cpp-sdk/speech.h"
#include <cstdlib>

void asr(aip::Speech &client)
{
    // 无可选参数调用接口
    std::string file_content;
    // 读取文件数据
    aip::get_file_content("./assets/voice/16k_test.pcm", &file_content);
    // Json::Value result = client.recognize(file_content, "pcm", 16000, aip::null);

    // 极速版调用函数
    // Json::Value result = client.recognize_pro(file_content, "pcm", 16000, aip::null);

    // 如果需要覆盖或者加入参数
    std::map<std::string, std::string> options;
    options["dev_pid"] = "1537";
    Json::Value result = client.recognize(file_content, "pcm", 16000, options);
    if (result["err_no"].asInt() != 0)
    {
        std::cout << "失败, reason: " << result["err_message"].asString() << std::endl;
    }
    else
    {
        std::cout << "成功, message: " << result["result"][0].asString() << std::endl;
    }
}

int main()
{
    std::string app_id = getenv("BAIDU_APP_ID");
    std::string api_key = getenv("BAIDU_API_KEY");
    std::string secret_key = getenv("BAIDU_API_SECRET");

    aip::Speech client(app_id, api_key, secret_key);
    asr(client);
    return 0;
}