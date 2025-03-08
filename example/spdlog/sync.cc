#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <iostream>

int main()
{
    //设置全局的刷新策略
    //每秒刷新
    spdlog::flush_every(std::chrono::seconds(1));
    //遇到debug以上等级的日志立即刷新
    spdlog::flush_on(spdlog::level::level_enum::debug);
    //设置全局的日志输出等级 -- 无所谓 --每个日志器可以独立进行设置
    spdlog::set_level(spdlog::level::level_enum::debug);

    //创建同步日志器（标准输出/文件） -- 工厂接口默认创建的就是同步日志器
    //auto logger = spdlog::stdout_color_mt("default-logger");
    auto logger = spdlog::basic_logger_mt("file-logger", "sync.log");
    //设置日志器的刷新策略，以及设置日志器的输出等级
    // logger->flush_on(spdlog::level::level_enum::debug);
    // logger->set_level(spdlog::level::level_enum::debug);
    //设置日志输出格式
    logger->set_pattern("[%n][%H:%M:%S][%t][%-8l] %v");
    //进行简单的日志输出
    logger->trace("你好！{}", "小明");
    logger->debug("你好！{}", "小明");
    logger->info("你好！{}", "小明");
    logger->warn("你好！{}", "小明");
    logger->error("你好！{}", "小明");
    logger->critical("你好！{}", "小明");
    std::cout << "日志输出演示完毕！\n";
    return 0;
}