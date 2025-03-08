#pragma once
#include <string>
#include <memory>  // std::auto_ptr
#include <cstdlib> // std::exit
#include <iostream>
#include <odb/mysql/mysql.hxx>  // 再包含 ODB 的 MySQL 适配头文件
#include <odb/database.hxx>
#include <odb/mysql/database.hxx>
#include "logger.hpp"

// 用户注册， 用户登录， 验证码获取， 手机号注册，手机号登录， 获取用户信息， 用户信息修改
// 用信息新增， 通过昵称获取用户信息，通过手机号获取用户信息， 通过用户ID获取用户信息， 通过多个用户ID获取多个用户信息，信息修改
namespace XuChat
{
    class ODBFactory
    {
    public:
        static std::shared_ptr<odb::core::database> create(
            const std::string &user,
            const std::string &pswd,
            const std::string &host,
            const std::string &db,
            const std::string &cset,
            int port,
            int conn_pool_count)
        {
            std::unique_ptr<odb::mysql::connection_pool_factory> cpf(
                new odb::mysql::connection_pool_factory(conn_pool_count, 0));
            auto res = std::make_shared<odb::mysql::database>(user, pswd,
                                                              db, host, port, "", cset, 0, std::move(cpf));
            return res;
        }
    };
}