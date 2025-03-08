#include "mysql.hpp"
#include "user.hxx"
#include "user-odb.hxx"

namespace XuChat
{
    class UserTable
    {
    public:
        using ptr = std::shared_ptr<UserTable>;
        UserTable(const std::shared_ptr<odb::core::database> &db) : _db(db) {}
        bool insert(const std::shared_ptr<XuChat::User> &user)
        {
            try
            {
                odb::transaction trans(_db->begin());
                _db->persist(*user);
                trans.commit();
            }
            catch (std::exception &e)
            {
                log_error(logger, "新增用户失败 %s:%s！", user->nickname().c_str(), e.what());
                return false;
            }
            return true;
        }
        bool update(const std::shared_ptr<XuChat::User> &user)
        {
            try
            {
                odb::transaction trans(_db->begin());
                _db->update(*user);
                trans.commit();
            }
            catch (std::exception &e)
            {
                log_error(logger, "更新用户失败 %s:%s！", user->nickname().c_str(), e.what());
                return false;
            }
            return true;
        }
        std::shared_ptr<XuChat::User> select_by_nickname(const std::string &nickname)
        {
            std::shared_ptr<XuChat::User> res;
            try
            {
                odb::transaction trans(_db->begin());
                typedef odb::query<XuChat::User> query;
                typedef odb::result<XuChat::User> result;
                res.reset(_db->query_one<XuChat::User>(query::nickname == nickname));
                trans.commit();
            }
            catch (std::exception &e)
            {
                log_error(logger, "通过昵称查询用户失败 %s:%s！", nickname.c_str(), e.what());
            }
            return res;
        }
        std::shared_ptr<XuChat::User> select_by_phone(const std::string &phone)
        {
            std::shared_ptr<XuChat::User> res;
            try
            {
                odb::transaction trans(_db->begin());
                typedef odb::query<XuChat::User> query;
                typedef odb::result<XuChat::User> result;
                res.reset(_db->query_one<XuChat::User>(query::phone == phone));
                trans.commit();
            }
            catch (std::exception &e)
            {
                log_error(logger, "通过手机号查询用户失败 %s:%s！", phone.c_str(), e.what());
            }
            return res;
        }
        std::shared_ptr<XuChat::User> select_by_id(const std::string &user_id)
        {
            std::shared_ptr<XuChat::User> res;
            try
            {
                odb::transaction trans(_db->begin());
                typedef odb::query<XuChat::User> query;
                typedef odb::result<XuChat::User> result;
                res.reset(_db->query_one<XuChat::User>(query::user_id == user_id));
                trans.commit();
            }
            catch (std::exception &e)
            {
                log_error(logger, "通过用户ID查询用户失败 %s:%s！", user_id.c_str(), e.what());
            }
            return res;
        }
        std::vector<XuChat::User> select_multi_users(const std::vector<std::string> &id_list)
        {
            // select * from user where id in ('id1', 'id2', ...)
            if (id_list.empty())
            {
                return std::vector<XuChat::User>();
            }
            std::vector<XuChat::User> res;
            try
            {
                odb::transaction trans(_db->begin());
                typedef odb::query<XuChat::User> query;
                typedef odb::result<XuChat::User> result;
                std::stringstream ss;
                ss << "user_id in (";
                for (const auto &id : id_list)
                {
                    ss << "'" << id << "',";
                }
                std::string condition = ss.str();
                condition.pop_back();
                condition += ")";
                result r(_db->query<XuChat::User>(condition));
                for (result::iterator i(r.begin()); i != r.end(); ++i)
                {
                    res.push_back(*i);
                }
                trans.commit();
            }
            catch (std::exception &e)
            {
                log_error(logger, "通过用户ID批量查询用户失败:%s！", e.what());
            }
            return res;
        }

    private:
        std::shared_ptr<odb::core::database> _db;
    };
}