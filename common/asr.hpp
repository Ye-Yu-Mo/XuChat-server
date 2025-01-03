#include "../third/aip-cpp-sdk/speech.h"
#include "logger.hpp"

namespace Common
{
    class ASRClient
    {
    public:
        using ptr = std::shared_ptr<ASRClient>;
        ASRClient(const std::string &appid,
                  const std::string &api_key,
                  const std::string &secret_key)
            : _client(appid, api_key, secret_key)
        {
        }
        std::string recognize(const std::string &speech_data, std::string& errmsg)
        {
            Json::Value result = _client.recognize(speech_data, "pcm", 16000, aip::null);
            if (result["err_no"].asInt() != 0)
            {
                log_error(logger, "失败, reason: %s", result["err_message"].asString().c_str());
                return std::string();
            }
            info(logger, "语音识别成功!");
            return result["result"][0].asString();
        }

    private:    
        aip::Speech _client;
    };
}