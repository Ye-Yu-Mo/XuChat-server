// 实现项目中一些公共的工具类接口
// 1. 生成一个唯一ID的接口
// 2. 文件的读写操作接口

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <atomic>
#include <random>
#include <iomanip>
#include "logger.hpp"

namespace XuChat
{

    std::string uuid()
    {
        // 生成一个由16位随机字符组成的字符串作为唯一ID
        //  1. 生成6个0~255之间的随机数字(1字节-转换为16进制字符)--生成12位16进制字符
        std::random_device rd;                                   // 实例化设备随机数对象-用于生成设备随机数
        std::mt19937 generator(rd());                            // 以设备随机数为种子，实例化伪随机数对象
        std::uniform_int_distribution<int> distribution(0, 255); // 限定数据范围

        std::stringstream ss;
        for (int i = 0; i < 6; i++)
        {
            if (i == 2)
                ss << "-";
            ss << std::setw(2) << std::setfill('0') << std::hex << distribution(generator);
        }
        ss << "-";
        // 2. 通过一个静态变量生成一个2字节的编号数字--生成4位16进制数字字符
        static std::atomic<short> idx(0);
        short tmp = idx.fetch_add(1);
        ss << std::setw(4) << std::setfill('0') << std::hex << tmp;
        return ss.str();
    }

    std::string vcode()
    {
        std::random_device rd;                                 // 实例化设备随机数对象-用于生成设备随机数
        std::mt19937 generator(rd());                          // 以设备随机数为种子，实例化伪随机数对象
        std::uniform_int_distribution<int> distribution(0, 9); // 限定数据范围

        std::stringstream ss;
        for (int i = 0; i < 4; i++)
        {
            ss << distribution(generator);
        }
        return ss.str();
    }

    bool readFile(const std::string &filename, std::string &body)
    {
        // 实现读取一个文件的所有数据，放入body中
        std::ifstream ifs(filename, std::ios::binary | std::ios::in);
        if (ifs.is_open() == false)
        {
            log_error(logger, "打开文件 %s 失败！", filename.c_str());
            return false;
        }
        ifs.seekg(0, std::ios::end); // 跳转到文件末尾
        size_t flen = ifs.tellg();   // 获取当前偏移量-- 文件大小
        ifs.seekg(0, std::ios::beg); // 跳转到文件起始
        body.resize(flen);
        ifs.read(&body[0], flen);
        if (ifs.good() == false)
        {
            log_error(logger, "读取文件 %s 数据失败！", filename.c_str());
            ifs.close();
            return false;
        }
        ifs.close();
        return true;
    }
    bool writeFile(const std::string &filename, const std::string &body)
    {
        // 实现将body中的数据，写入filename对应的文件中
        std::ofstream ofs(filename, std::ios::out | std::ios::binary | std::ios::trunc);
        if (ofs.is_open() == false)
        {
            log_error(logger, "打开文件 %s 失败！", filename.c_str());
            return false;
        }
        ofs.write(body.c_str(), body.size());
        if (ofs.good() == false)
        {
            log_error(logger, "读取文件 %s 数据失败！", filename.c_str());
            ofs.close();
            return false;
        }
        ofs.close();
        return true;
    }

}