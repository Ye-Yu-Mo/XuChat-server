# XuChat

C++20

微服务聊天系统的服务端

## 依赖的库或平台

1. [Xulog](https://github.com/Ye-Yu-Mo/LogSystem) 日志库
2. [gflags](https://github.com/gflags/gflags) 命令行参数解析框架
3. [gtest](https://github.com/google/gtest-parallel) 单元测试框架
4. [etcd](https://github.com/etcd-cpp-apiv3/etcd-cpp-apiv3) 分布式键值存储系统 用于服务注册与发现
5. [brpc](https://github.com/apache/brpc) rpc框架
6. [cinatra](https://github.com/qicosmos/cinatra?tab=readme-ov-file) 协程 http websocket
7. [elasticsearch](https://github.com/elastic/elasticsearch) 搜索引擎库
8. [redis](https://github.com/redis/redis) redis缓存库
9. [hiredis](https://github.com/redis/hiredis) C语言redis客户端
10. [redis++](https://github.com/sewenew/redis-plus-plus) C++redis客户端 
11. [odb](https://codesynthesis.com/products/odb/doc/install-build2.xhtml#linux) 内存与数据库映射库
12. [AMQP-CPP](https://github.com/CopernicaMarketingSoftware/AMQP-CPP) 用于RabbitMQ通信
13. [阿里验证码服务](https://github.com/aliyun/aliyun-openapi-cpp-sdk.git)
14. [百度语音转文字服务](https://ai.baidu.com/sdk#asr)

## TODO

1. 手机号验证码接收平台 从百度换个免费的
2. 增加邮箱注册登录
3. 接入谷歌gemini模型 考虑使用Go或者Python完成API调用 [gemini文档](https://ai.google.dev/gemini-api/docs/structured-output?hl=zh-cn&lang=python)
    
    1. 文字聊天（天气，计划生成待办）
    2. 语音聊天
    3. 图片 + 视频
