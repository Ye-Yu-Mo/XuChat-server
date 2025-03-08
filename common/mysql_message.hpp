#include "mysql.hpp"
#include "message.hxx"
#include "message-odb.hxx"

namespace XuChat
{
    class MessageTable
    {
    public:
        using ptr = std::shared_ptr<MessageTable>;
        MessageTable(const std::shared_ptr<odb::core::database> &db) : _db(db) {}
        ~MessageTable() {}
        bool insert(XuChat::Message &msg)
        {
            try
            {
                odb::transaction trans(_db->begin());
                _db->persist(msg);
                trans.commit();
            }
            catch (std::exception &e)
            {
                log_error(logger, "新增消息失败 %s:%s！", msg.message_id().c_str(), e.what());
                return false;
            }
            return true;
        }
        bool remove(const std::string &ssid)
        {
            try
            {
                odb::transaction trans(_db->begin());
                typedef odb::query<XuChat::Message> query;
                typedef odb::result<XuChat::Message> result;
                _db->erase_query<XuChat::Message>(query::session_id == ssid);
                trans.commit();
            }
            catch (std::exception &e)
            {
                log_error(logger, "删除会话所有消息失败 %s:%s！", ssid.c_str(), e.what());
                return false;
            }
            return true;
        }
        std::vector<XuChat::Message> recent(const std::string &ssid, int count)
        {
            std::vector<XuChat::Message> res;
            try
            {
                odb::transaction trans(_db->begin());
                typedef odb::query<XuChat::Message> query;
                typedef odb::result<XuChat::Message> result;
                // 本次查询是以ssid作为过滤条件，然后进行以时间字段进行逆序，通过limit
                //  session_id='xx' order by create_time desc  limit count;
                std::stringstream cond;
                cond << "session_id='" << ssid << "' ";
                cond << "order by create_time desc limit " << count;
                result r(_db->query<XuChat::Message>(cond.str()));
                for (result::iterator i(r.begin()); i != r.end(); ++i)
                {
                    res.push_back(*i);
                }
                std::reverse(res.begin(), res.end());
                trans.commit();
            }
            catch (std::exception &e)
            {
                log_error(logger, "获取最近消息失败:%s-%d-%s！", ssid.c_str(), count, e.what());
            }
            return res;
        }
        std::vector<XuChat::Message> range(const std::string &ssid,
                                   boost::posix_time::ptime &stime,
                                   boost::posix_time::ptime &etime)
        {
            std::vector<XuChat::Message> res;
            try
            {
                odb::transaction trans(_db->begin());
                typedef odb::query<XuChat::Message> query;
                typedef odb::result<XuChat::Message> result;
                // 获取指定会话指定时间段的信息
                result r(_db->query<XuChat::Message>(query::session_id == ssid &&
                                             query::create_time >= stime &&
                                             query::create_time <= etime));
                for (result::iterator i(r.begin()); i != r.end(); ++i)
                {
                    res.push_back(*i);
                }
                trans.commit();
            }
            catch (std::exception &e)
            {
                log_error(logger, "获取区间消息失败:%s-%s:%s-%s！", ssid.c_str(),
                          boost::posix_time::to_simple_string(stime).c_str(),
                          boost::posix_time::to_simple_string(etime).c_str(), e.what());
            }
            return res;
        }

    private:
        std::shared_ptr<odb::core::database> _db;
    };
}