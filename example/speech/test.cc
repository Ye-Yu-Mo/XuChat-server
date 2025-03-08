#include "aip-cpp-sdk/speech.h"

void asr(aip::Speech &client)
{
    std::string file_content;
    aip::get_file_content("16k.pcm", &file_content);

    Json::Value result = client.recognize(file_content, "pcm", 16000, aip::null);
    if (result["err_no"].asInt() != 0) {
        std::cout << result["err_msg"].asString() << std::endl;
        return;
    }

    std::cout << result["result"][0].asString() << std::endl;
}

int main()
{
    // 设置APPID/AK/SK
    std::string app_id = "60694095";
    std::string api_key = "PWn6zlsxym8VwpBW8Or4PPGe";
    std::string secret_key = "Bl0mn74iyAkr3FzCo5TZV7lBq7NYoms9";

    aip::Speech client(app_id, api_key, secret_key);

    asr(client);
    return 0;
}