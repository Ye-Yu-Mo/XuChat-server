#pragma once
#include "mysql.hpp"
#include "chat_session_member.hxx"
#include "chat_session_member-odb.hxx"

namespace XuChat
{
    class ChatSessionMemeberTable
    {
    public:
        using ptr = std::shared_ptr<ChatSessionMemeberTable>;
        ChatSessionMemeberTable(const std::shared_ptr<odb::core::database> &db) : _db(db) {}
        // 单个会话成员的新增 --- ssid & uid
        bool append(XuChat::ChatSessionMember &csm)
        {
            try
            {
                odb::transaction trans(_db->begin());
                _db->persist(csm);
                trans.commit();
            }
            catch (std::exception &e)
            {
                log_error(logger, "新增单会话成员失败 %s-%s:%s！",
                          csm.session_id().c_str(), csm.user_id().c_str(), e.what());
                return false;
            }
            return true;
        }
        bool append(std::vector<XuChat::ChatSessionMember> &csm_lists)
        {
            try
            {
                odb::transaction trans(_db->begin());
                for (auto &csm : csm_lists)
                {
                    _db->persist(csm);
                }
                trans.commit();
            }
            catch (std::exception &e)
            {
                log_error(logger, "新增多会话成员失败 %s-%d:%s！",
                          csm_lists[0].session_id().c_str(), csm_lists.size(), e.what());
                return false;
            }
            return true;
        }
        // 删除指定会话中的指定成员 -- ssid & uid
        bool remove(XuChat::ChatSessionMember &csm)
        {
            try
            {
                odb::transaction trans(_db->begin());
                typedef odb::query<XuChat::ChatSessionMember> query;
                typedef odb::result<XuChat::ChatSessionMember> result;
                _db->erase_query<XuChat::ChatSessionMember>(query::session_id == csm.session_id() &&
                                                         query::user_id == csm.user_id());
                trans.commit();
            }
            catch (std::exception &e)
            {
                log_error(logger, "删除单会话成员失败 %s-%s:%s！",
                          csm.session_id().c_str(), csm.user_id().c_str(), e.what());
                return false;
            }
            return true;
        }
        // 删除会话的所有成员信息
        bool remove(const std::string &ssid)
        {
            try
            {
                odb::transaction trans(_db->begin());
                typedef odb::query<XuChat::ChatSessionMember> query;
                typedef odb::result<XuChat::ChatSessionMember> result;
                _db->erase_query<XuChat::ChatSessionMember>(query::session_id == ssid);
                trans.commit();
            }
            catch (std::exception &e)
            {
                log_error(logger, "删除会话所有成员失败 %s:%s！", ssid.c_str(), e.what());
                return false;
            }
            return true;
        }
        std::vector<std::string> members(const std::string &ssid)
        {
            std::vector<std::string> res;
            try
            {
                odb::transaction trans(_db->begin());
                typedef odb::query<XuChat::ChatSessionMember> query;
                typedef odb::result<XuChat::ChatSessionMember> result;
                result r(_db->query<XuChat::ChatSessionMember>(query::session_id == ssid));
                for (result::iterator i(r.begin()); i != r.end(); ++i)
                {
                    res.push_back(i->user_id());
                }
                trans.commit();
            }
            catch (std::exception &e)
            {
                log_error(logger, "获取会话成员失败:%s-%s！", ssid.c_str(), e.what());
            }
            return res;
        }

    private:
        std::shared_ptr<odb::core::database> _db;
    };
}