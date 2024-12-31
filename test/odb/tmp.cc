#include <odb/core.hxx>

#pragma db object
class MyClass
{
public:
    MyClass() = default;
    MyClass(int id) : id_(id) {}
    int get_id() const { return id_; }

private:
    int id_;
};

int main()
{
    // 这里可以做一些测试，比如创建对象等
    MyClass obj(10);
    return 0;
}

