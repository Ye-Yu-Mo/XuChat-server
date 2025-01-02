#include <cstdlib>
#include <iostream>
#include <alibabacloud/core/AlibabaCloud.h>
#include <alibabacloud/core/CommonRequest.h>
#include <alibabacloud/core/CommonClient.h>
#include <alibabacloud/core/CommonResponse.h>
#include "logger.hpp"
namespace Common
{
    class DMSClient
    {
    public:
        DMSClient(const std::string &accesskey_id,
                  const std::string &access_key_secret)
        {
            AlibabaCloud::InitializeSdk();
            AlibabaCloud::ClientConfiguration configuration("cn-qingdao");
            configuration.setConnectTimeout(1500);
            configuration.setReadTimeout(4000);
            AlibabaCloud::Credentials credential(accesskey_id, access_key_secret);
            _client = std::make_unique<AlibabaCloud::CommonClient>(credential, configuration);
        }

        ~DMSClient()
        {
            AlibabaCloud::ShutdownSdk();
        }
        bool send(const std::string &phone, const std::string &code)
        {
            AlibabaCloud::CommonRequest request(AlibabaCloud::CommonRequest::RequestPattern::RpcPattern);
            request.setHttpMethod(AlibabaCloud::HttpRequest::Method::Post);
            request.setDomain("dysmsapi.aliyuncs.com");
            request.setVersion("2017-05-25");
            request.setQueryParameter("Action", "SendSms");
            request.setQueryParameter("SignName", "小许学习");
            request.setQueryParameter("TemplateCode", "SMS_476770738");
            request.setQueryParameter("PhoneNumbers", phone);
            request.setQueryParameter("TemplateParam", "{\"code\":\"" + code + "\"}");

            auto response = _client->commonResponse(request);
            if (!response.isSuccess())
            {
                log_error(logger, "验证码发送失败! reason: %s", response.error().errorMessage().c_str());
                return false;
            }
            info(logger, "验证码发送成功!");
            return true;
        }

    private:
        std::unique_ptr<AlibabaCloud::CommonClient> _client;
    };
}
