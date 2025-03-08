#include <json/json.h>
#include <iostream>
#include <sstream>
#include <memory>

bool Serialize(const Json::Value &val, std::string &dst)
{
    //先定义Json::StreamWriter 工厂类 Json::StreamWriterBuilder 
    Json::StreamWriterBuilder swb;
    swb.settings_["emitUTF8"] = true;
    std::unique_ptr<Json::StreamWriter> sw(swb.newStreamWriter());
    //通过Json::StreamWriter中的write接口进行序列化
    std::stringstream ss;
    int ret = sw->write(val, &ss);
    if (ret != 0) {
        std::cout << "Json反序列化失败！\n";
        return false;
    }
    dst = ss.str();
    return true;
}
bool UnSerialize(const std::string &src, Json::Value &val)
{
    Json::CharReaderBuilder crb;
    std::unique_ptr<Json::CharReader> cr(crb.newCharReader());
    std::string err;
    bool ret = cr->parse(src.c_str(), src.c_str() + src.size(), &val, &err);
    if (ret == false) {
        std::cout << "json反序列化失败: " << err << std::endl;
        return false;
    }
    return true;
}

int main()
{
    char name[] = "张三";
    int age = 18;
    float score[3] = {88, 89.5, 99};

    Json::Value stu;
    stu["姓名"] = name;
    stu["年龄"] = age;
    stu["成绩"].append(score[0]);
    stu["成绩"].append(score[1]);
    stu["成绩"].append(score[2]);

    std::string stu_str;
    bool ret = Serialize(stu, stu_str);
    if (ret == false ) return -1;
    std::cout << stu_str << std::endl;

    Json::Value val;
    ret = UnSerialize(stu_str, val);
    if (ret == false) return -1;
    std::cout << val["姓名"].asString() << std::endl;
    std::cout << val["年龄"].asInt() << std::endl;
    int sz = val["成绩"].size();
    for (int i = 0; i < sz; i++) {
        std::cout << val["成绩"][i].asFloat() << std::endl;
    }
    return 0;
}