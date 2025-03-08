#include <gtest/gtest.h>

int Add(int num1, int num2)
{
    return num1 + num2;
}

TEST(测试名称, 加法用例名称1)
{
    ASSERT_EQ(Add(10, 20), 30);
    ASSERT_LT(Add(20, 20), 50);
}

TEST(测试名称, 字符串比较测试)
{
    std::string str = "Hello";
    EXPECT_EQ(str, "hello");
    printf("断言失败后的打印\n");
    EXPECT_EQ(str, "Hello");
}

int main(int argc, char *argv[])
{
    //单元测试框架的初始化
    testing::InitGoogleTest(&argc, argv);
    //开始所有的单元测试
    return RUN_ALL_TESTS();
}