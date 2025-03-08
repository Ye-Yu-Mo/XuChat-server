#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include "logger.hpp"

namespace XuChat
{
    typedef websocketpp::server<websocketpp::config::asio> server_t;
    // 连接的类型： server_t::connection_ptr

    class Connection
    {
    public:
        struct Client
        {
            Client(const std::string &u, const std::string &s) : uid(u), ssid(s) {}
            std::string uid;
            std::string ssid;
        };
        using ptr = std::shared_ptr<Connection>;
        Connection() {}
        ~Connection() {}
        void insert(const server_t::connection_ptr &conn,
                    const std::string &uid, const std::string &ssid)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _uid_connections.insert(std::make_pair(uid, conn));
            _conn_clients.insert(std::make_pair(conn, Client(uid, ssid)));
            debug(logger, "新增长连接用户信息：%d-%s-%s", (size_t)conn.get(), uid.c_str(), ssid.c_str());
        }
        server_t::connection_ptr connection(const std::string &uid)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto it = _uid_connections.find(uid);
            if (it == _uid_connections.end())
            {
                log_error(logger, "未找到 %s 客户端的长连接！", uid.c_str());
                return server_t::connection_ptr();
            }
            debug(logger, "找到 %s 客户端的长连接！", uid.c_str());
            return it->second;
        }
        bool client(const server_t::connection_ptr &conn, std::string &uid, std::string &ssid)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto it = _conn_clients.find(conn);
            if (it == _conn_clients.end())
            {
                log_error(logger, "获取-未找到长连接 %d 对应的客户端信息！", (size_t)conn.get());
                return false;
            }
            uid = it->second.uid;
            ssid = it->second.ssid;
            debug(logger, "获取长连接客户端信息成功！");
            return true;
        }
        void remove(const server_t::connection_ptr &conn)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto it = _conn_clients.find(conn);
            if (it == _conn_clients.end())
            {
                log_error(logger, "删除-未找到长连接 %d 对应的客户端信息！", (size_t)conn.get());
                return;
            }
            _uid_connections.erase(it->second.uid);
            _conn_clients.erase(it);
            debug(logger, "删除长连接信息完毕！");
        }

    private:
        std::mutex _mutex;
        std::unordered_map<std::string, server_t::connection_ptr> _uid_connections;
        std::unordered_map<server_t::connection_ptr, Client> _conn_clients;
    };
}