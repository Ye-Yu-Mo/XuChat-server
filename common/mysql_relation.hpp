#pragma once
#include "mysql.hpp"
#include "relation.hxx"
#include "relation-odb.hxx"

namespace XuChat
{
    class RelationTable
    {
    public:
        using ptr = std::shared_ptr<RelationTable>;
        RelationTable(const std::shared_ptr<odb::core::database> &db) : _db(db) {}
        // 新增关系信息
        bool insert(const std::string &uid, const std::string &pid)
        {
            //{1,2} {2,1}
            try
            {
                XuChat::Relation r1(uid, pid);
                XuChat::Relation r2(pid, uid);
                odb::transaction trans(_db->begin());
                _db->persist(r1);
                _db->persist(r2);
                trans.commit();
            }
            catch (std::exception &e)
            {
                log_error(logger, "新增用户好友关系信息失败 %s-%s:%s！", uid.c_str(), pid.c_str(), e.what());
                return false;
            }
            return true;
        }
        // 移除关系信息
        bool remove(const std::string &uid, const std::string &pid)
        {
            try
            {
                odb::transaction trans(_db->begin());
                typedef odb::query<XuChat::Relation> query;
                typedef odb::result<XuChat::Relation> result;
                _db->erase_query<XuChat::Relation>(query::user_id == uid && query::peer_id == pid);
                _db->erase_query<XuChat::Relation>(query::user_id == pid && query::peer_id == uid);
                trans.commit();
            }
            catch (std::exception &e)
            {
                log_error(logger, "删除好友关系信息失败 %s-%s:%s！", uid.c_str(), pid.c_str(), e.what());
                return false;
            }
            return true;
        }
        // 判断关系是否存在
        bool exists(const std::string &uid, const std::string &pid)
        {
            typedef odb::query<XuChat::Relation> query;
            typedef odb::result<XuChat::Relation> result;
            result r;
            bool flag = false;
            try
            {
                odb::transaction trans(_db->begin());
                r = _db->query<XuChat::Relation>(query::user_id == uid && query::peer_id == pid);
                flag = !r.empty();
                trans.commit();
            }
            catch (std::exception &e)
            {
                log_error(logger, "获取用户好友关系失败:%s-%s-%s！", uid.c_str(), pid.c_str(), e.what());
            }
            return flag;
        }
        // 获取指定用户的好友ID
        std::vector<std::string> friends(const std::string &uid)
        {
            std::vector<std::string> res;
            try
            {
                odb::transaction trans(_db->begin());
                typedef odb::query<XuChat::Relation> query;
                typedef odb::result<XuChat::Relation> result;
                result r(_db->query<XuChat::Relation>(query::user_id == uid));
                for (result::iterator i(r.begin()); i != r.end(); ++i)
                {
                    res.push_back(i->peer_id());
                }
                trans.commit();
            }
            catch (std::exception &e)
            {
                log_error(logger, "通过用户-%s的所有好友ID失败:%s！", uid.c_str(), e.what());
            }
            return res;
        }

    private:
        std::shared_ptr<odb::core::database> _db;
    };
}