#pragma once
#include "mysql.hpp"
#include "chat_session.hxx"
#include "chat_session-odb.hxx"
#include "mysql_chat_session_member.hpp"

namespace XuChat
{
    class ChatSessionTable
    {
    public:
        using ptr = std::shared_ptr<ChatSessionTable>;
        ChatSessionTable(const std::shared_ptr<odb::core::database> &db) : _db(db) {}
        bool insert(XuChat::ChatSession &cs)
        {
            try
            {
                odb::transaction trans(_db->begin());
                _db->persist(cs);
                trans.commit();
            }
            catch (std::exception &e)
            {
                log_error(logger, "新增会话失败 %s:%s！", cs.chat_session_name().c_str(), e.what());
                return false;
            }
            return true;
        }
        bool remove(const std::string &ssid)
        {
            try
            {
                odb::transaction trans(_db->begin());
                typedef odb::query<XuChat::ChatSession> query;
                typedef odb::result<XuChat::ChatSession> result;
                _db->erase_query<XuChat::ChatSession>(query::chat_session_id == ssid);

                typedef odb::query<XuChat::ChatSessionMember> mquery;
                _db->erase_query<XuChat::ChatSessionMember>(mquery::session_id == ssid);
                trans.commit();
            }
            catch (std::exception &e)
            {
                log_error(logger, "删除会话失败 %s:%s！", ssid.c_str(), e.what());
                return false;
            }
            return true;
        }
        bool remove(const std::string &uid, const std::string &pid)
        {
            // 单聊会话的删除，-- 根据单聊会话的两个成员
            try
            {
                odb::transaction trans(_db->begin());
                typedef odb::query<XuChat::SingleChatSession> query;
                typedef odb::result<XuChat::SingleChatSession> result;
                auto res = _db->query_one<XuChat::SingleChatSession>(
                    query::csm1::user_id == uid &&
                    query::csm2::user_id == pid &&
                    query::css::chat_session_type == XuChat::ChatSessionType::SINGLE);

                std::string cssid = res->chat_session_id;
                typedef odb::query<XuChat::ChatSession> cquery;
                _db->erase_query<XuChat::ChatSession>(cquery::chat_session_id == cssid);

                typedef odb::query<XuChat::ChatSessionMember> mquery;
                _db->erase_query<XuChat::ChatSessionMember>(mquery::session_id == cssid);
                trans.commit();
            }
            catch (std::exception &e)
            {
                log_error(logger, "删除会话失败 %s-%s:%s！", uid.c_str(), pid.c_str(), e.what());
                return false;
            }
            return true;
        }
        std::shared_ptr<XuChat::ChatSession> select(const std::string &ssid)
        {
            std::shared_ptr<XuChat::ChatSession> res;
            try
            {
                odb::transaction trans(_db->begin());
                typedef odb::query<XuChat::ChatSession> query;
                typedef odb::result<XuChat::ChatSession> result;
                res.reset(_db->query_one<XuChat::ChatSession>(query::chat_session_id == ssid));
                trans.commit();
            }
            catch (std::exception &e)
            {
                log_error(logger, "通过会话ID获取会话信息失败 %s:%s！", ssid.c_str(), e.what());
            }
            return res;
        }
        std::vector<XuChat::SingleChatSession> singleChatSession(const std::string &uid)
        {
            std::vector<XuChat::SingleChatSession> res;
            try
            {
                odb::transaction trans(_db->begin());
                typedef odb::query<XuChat::SingleChatSession> query;
                typedef odb::result<XuChat::SingleChatSession> result;
                // 当前的uid是被申请者的用户ID
                result r(_db->query<XuChat::SingleChatSession>(
                    query::css::chat_session_type == XuChat::ChatSessionType::SINGLE &&
                    query::csm1::user_id == uid &&
                    query::csm2::user_id != query::csm1::user_id));
                for (result::iterator i(r.begin()); i != r.end(); ++i)
                {
                    res.push_back(*i);
                }
                trans.commit();
            }
            catch (std::exception &e)
            {
                log_error(logger, "获取用户 %s 的单聊会话失败:%s！", uid.c_str(), e.what());
            }
            return res;
        }
        std::vector<XuChat::GroupChatSession> groupChatSession(const std::string &uid)
        {
            std::vector<XuChat::GroupChatSession> res;
            try
            {
                odb::transaction trans(_db->begin());
                typedef odb::query<XuChat::GroupChatSession> query;
                typedef odb::result<XuChat::GroupChatSession> result;
                // 当前的uid是被申请者的用户ID
                result r(_db->query<XuChat::GroupChatSession>(
                    query::css::chat_session_type == XuChat::ChatSessionType::GROUP &&
                    query::csm::user_id == uid));
                for (result::iterator i(r.begin()); i != r.end(); ++i)
                {
                    res.push_back(*i);
                }
                trans.commit();
            }
            catch (std::exception &e)
            {
                log_error(logger, "获取用户 %s 的群聊会话失败:%s！", uid.c_str(), e.what());
            }
            return res;
        }

    private:
        std::shared_ptr<odb::core::database> _db;
    };
}