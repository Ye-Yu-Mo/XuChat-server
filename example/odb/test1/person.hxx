#pragma once
#include <string>
#include <cstddef> // std::size_t
#include <boost/date_time/posix_time/posix_time.hpp>
#include <odb/nullable.hxx>
/*
 在 C++ 中，要使用 ODB 将类声明为持久化类，需要包含 ODB 的核心头文
件，并使用 #pragma db object 指令
 #pragma db object 指示 ODB 编译器将 person 类视为一个持久化类。
*/
#include <odb/core.hxx>
typedef boost::posix_time::ptime ptime;

#pragma db object table("person")
class Person {
    public:
        Person(const std::string &name, int age, const ptime 
        &update):
        _name(name), _age(age), _update(update){}
        void age(int val) { _age = val; }
        int age() { return _age; }
        void name(const std::string& val) { _name = val; }
        std::string name() { return _name; }
        void update(const ptime &update) { _update = update; }
        std::string update() { return 
            boost::posix_time::to_simple_string(*_update); }
    private:
        //将 odb：：access 类作为 person 类的朋友。
        //这是使数据库支持代码可访问默认构造函数和数据成员所必需的。
        //如果类具有公共默认构造函数和公共数据成员或数据成员的公共访问器和修饰符，则不需要友元声明
        friend class odb::access;
        Person () {}
        //_id 成员前面的 pragma 告诉 ODB 编译器，以下成员是对象的标识符。auto 说明符指示它是数据库分配的 ID。
        #pragma db id auto // 表示 ID 字段将自动生成（通常是数据库中的主键）。
        unsigned long _id;
        #pragma db column("user_age") type("decimal") default(20)
        unsigned short _age;
        #pragma db unique
        std::string _name;
        #pragma db type("TIMESTAMP") index
        odb::nullable<boost::posix_time::ptime> _update;
};
//将 ODB 编译指示组合在一起，并放在类定义之后。它们也可以移动到一个单独的标头中，使原始类完全保持不变
// #pragma db object(person)
// #pragma db member(person::_name) id
//完成后，需要使用 odb 编译器将当前所写的代码生成数据库支持代码
//odb -d mysql --generate-query --generate-schema person.hxx
//如果用到了 boost 库中的接口，则需要使用选项 : --profile boost/datetime

//odb -d mysql --generate-query --generate-schema --profile boost/date-time person.hxx