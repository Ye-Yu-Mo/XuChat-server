#pragma once
#include "mysql.hpp"
#include "friend_apply.hxx"
#include "friend_apply-odb.hxx"

namespace XuChat
{
    class FriendApplyTable
    {
    public:
        using ptr = std::shared_ptr<FriendApplyTable>;
        FriendApplyTable(const std::shared_ptr<odb::core::database> &db) : _db(db) {}
        bool insert(XuChat::FriendApply &ev)
        {
            try
            {
                odb::transaction trans(_db->begin());
                _db->persist(ev);
                trans.commit();
            }
            catch (std::exception &e)
            {
                log_error(XuChat::logger, "新增好友申请事件失败 %s-%s:%s！", ev.user_id().c_str(), ev.peer_id().c_str(), e.what());
                return false;
            }
            return true;
        }
        bool exists(const std::string &uid, const std::string &pid)
        {
            bool flag = false;
            try
            {
                typedef odb::query<XuChat::FriendApply> query;
                typedef odb::result<XuChat::FriendApply> result;
                odb::transaction trans(_db->begin());
                result r(_db->query<XuChat::FriendApply>(query::user_id == uid && query::peer_id == pid));
                debug(XuChat::logger, "%s - %s 好友事件数量：%d", uid.c_str(), pid.c_str(), r.size());
                flag = !r.empty();
                trans.commit();
            }
            catch (std::exception &e)
            {
                log_error(XuChat::logger, "获取好友申请事件失败:%s - %s - %s！", uid.c_str(), pid.c_str(), e.what());
            }
            return flag;
        }
        bool remove(const std::string &uid, const std::string &pid)
        {
            try
            {
                odb::transaction trans(_db->begin());
                typedef odb::query<XuChat::FriendApply> query;
                typedef odb::result<XuChat::FriendApply> result;
                _db->erase_query<XuChat::FriendApply>(query::user_id == uid && query::peer_id == pid);
                trans.commit();
            }
            catch (std::exception &e)
            {
                log_error(logger, "删除好友申请事件失败 %s - %s : %s！", uid.c_str(), pid.c_str(), e.what());
                return false;
            }
            return true;
        }
        // 获取当前指定用户的 所有好友申请者ID
        std::vector<std::string> applyUsers(const std::string &uid)
        {
            std::vector<std::string> res;
            try
            {
                odb::transaction trans(_db->begin());
                typedef odb::query<XuChat::FriendApply> query;
                typedef odb::result<XuChat::FriendApply> result;
                // 当前的uid是被申请者的用户ID
                result r(_db->query<XuChat::FriendApply>(query::peer_id == uid));
                for (result::iterator i(r.begin()); i != r.end(); ++i)
                {
                    res.push_back(i->user_id());
                }
                trans.commit();
            }
            catch (std::exception &e)
            {
                log_error(XuChat::logger, "通过用户 %s 的好友申请者失败: %s！", uid.c_str(), e.what());
            }
            return res;
        }

    private:
        std::shared_ptr<odb::core::database> _db;
    };
}