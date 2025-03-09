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

## 运行方法

请先查看并修改conf中的密钥配置文件 

### 1. 本机运行

请提前安装好依赖

编译
```bash
    mkdir build && cd build
    cmake ..
    make
```
对应开启build中的子服务即可

### 2. docker

#### docker 打包

在每一个子服务中分别编译形成可以执行文件

```bash
    cd ./***/
    mkdir build && cd build
    cmake ..
    make
```

```shell
./depends.sh
```
如果没有输出则说明依赖成功

```shell
docker compose up
```
测试是否能正常启动

#### docker 运行


## 解决问题

1. 运行`depends.sh`时提示没有`./****/depends`
    > 打开`depends.sh` 取消注释 `# mkdir $2`(或者手动创建)<br>
    > 如果已经存在则需要添加注释

## TODO

1. 手机号验证码接收平台 从百度换个免费的
2. 增加邮箱注册登录
3. 接入谷歌gemini模型 考虑使用Go或者Python完成API调用 [gemini文档](https://ai.google.dev/gemini-api/docs/structured-output?hl=zh-cn&lang=python)
    
    1. 文字聊天（天气，计划生成待办）
    2. 语音聊天
    3. 图片 + 视频
