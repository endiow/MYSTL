#include <gtest/gtest.h>
#include "mystl/algorithm.hpp"
#include <vector>
#include <string>

// 测试 find 算法
TEST(AlgorithmTest, Find) 
{
    // 测试基本类型
    std::vector<int> v1{1, 2, 3, 4, 5};
    auto it1 = mystl::find(v1.begin(), v1.end(), 3);
    EXPECT_EQ(*it1, 3);
    EXPECT_EQ(std::distance(v1.begin(), it1), 2);

    // 测试找不到的情况
    auto it2 = mystl::find(v1.begin(), v1.end(), 10);
    EXPECT_EQ(it2, v1.end());

    // 测试自定义类型
    std::vector<std::string> v2{"hello", "world", "mystl"};
    auto it3 = mystl::find(v2.begin(), v2.end(), "world");
    EXPECT_EQ(*it3, "world");
}

// 测试 find_if 算法
TEST(AlgorithmTest, FindIf) 
{
    std::vector<int> v{1, 2, 3, 4, 5};
    
    // 测试基本谓词
    auto it1 = mystl::find_if(v.begin(), v.end(), [](int n) { return n > 3; });
    EXPECT_EQ(*it1, 4);

    // 测试找不到的情况
    auto it2 = mystl::find_if(v.begin(), v.end(), [](int n) { return n > 10; });
    EXPECT_EQ(it2, v.end());

    // 测试复杂谓词
    struct Person 
    {
        std::string name;
        int age;
    };
    std::vector<Person> people{{"Alice", 20}, {"Bob", 25}, {"Charlie", 30}};
    auto it3 = mystl::find_if(people.begin(), people.end(), 
                             [](const Person& p) { return p.age > 22; });
    EXPECT_EQ(it3->name, "Bob");
}

// 测试 find_if_not 算法
TEST(AlgorithmTest, FindIfNot) 
{
    std::vector<int> v{1, 2, 3, 4, 5};
    
    // 测试基本谓词
    auto it1 = mystl::find_if_not(v.begin(), v.end(), [](int n) { return n < 4; });
    EXPECT_EQ(*it1, 4);

    // 测试找不到的情况
    auto it2 = mystl::find_if_not(v.begin(), v.end(), [](int n) { return n > 0; });
    EXPECT_EQ(it2, v.end());
}

// 测试 find_first_of 算法
TEST(AlgorithmTest, FindFirstOf) 
{
    std::vector<int> v1{1, 2, 3, 4, 5};
    std::vector<int> v2{10, 4, 20};
    
    // 测试基本查找
    auto it1 = mystl::find_first_of(v1.begin(), v1.end(), v2.begin(), v2.end());
    EXPECT_EQ(*it1, 4);

    // 测试找不到的情况
    std::vector<int> v3{10, 20, 30};
    auto it2 = mystl::find_first_of(v1.begin(), v1.end(), v3.begin(), v3.end());
    EXPECT_EQ(it2, v1.end());

    // 测试自定义比较器
    auto it3 = mystl::find_first_of(v1.begin(), v1.end(), v2.begin(), v2.end(),
                                   [](int a, int b) { return (a % 2) == (b % 2); });
    EXPECT_EQ(*it3, 2);  // 应该找到2，因为它是v1中第一个与v2中的数字（都是偶数）具有相同奇偶性的数
}

// 测试 adjacent_find 算法
TEST(AlgorithmTest, AdjacentFind) 
{
    // 测试基本查找
    std::vector<int> v1{1, 2, 2, 3, 4};
    auto it1 = mystl::adjacent_find(v1.begin(), v1.end());
    EXPECT_EQ(*it1, 2);
    EXPECT_EQ(*(it1 + 1), 2);

    // 测试找不到的情况
    std::vector<int> v2{1, 2, 3, 4, 5};
    auto it2 = mystl::adjacent_find(v2.begin(), v2.end());
    EXPECT_EQ(it2, v2.end());

    // 测试自定义比较器
    std::vector<int> v3{1, 3, 2, 4, 6};
    auto it3 = mystl::adjacent_find(v3.begin(), v3.end(),
                                   [](int a, int b) { return b == a + 1; });
    EXPECT_EQ(it3, v3.end());  // 序列中没有连续的数字
}

// 测试空容器的情况
TEST(AlgorithmTest, EmptyContainer) 
{
    std::vector<int> empty;
    
    EXPECT_EQ(mystl::find(empty.begin(), empty.end(), 1), empty.end());
    EXPECT_EQ(mystl::find_if(empty.begin(), empty.end(), [](int n) { return true; }), empty.end());
    EXPECT_EQ(mystl::find_if_not(empty.begin(), empty.end(), [](int n) { return true; }), empty.end());
    EXPECT_EQ(mystl::find_first_of(empty.begin(), empty.end(), empty.begin(), empty.end()), empty.end());
    EXPECT_EQ(mystl::adjacent_find(empty.begin(), empty.end()), empty.end());
}

// 测试 count 算法
TEST(AlgorithmTest, Count) 
{
    // 测试基本类型
    std::vector<int> v1{1, 2, 2, 3, 2, 4, 2, 5};
    auto count1 = mystl::count(v1.begin(), v1.end(), 2);
    EXPECT_EQ(count1, 4);  // 应该找到4个2

    // 测试找不到的情况
    auto count2 = mystl::count(v1.begin(), v1.end(), 0);
    EXPECT_EQ(count2, 0);  // 应该找到0个0

    // 测试自定义类型
    struct Person 
    {
        std::string name;
        int age;
        bool operator==(const Person& other) const 
        {
            return age == other.age;
        }
    };
    std::vector<Person> people{
        {"Alice", 20}, {"Bob", 25}, {"Charlie", 20}, {"David", 20}
    };
    auto count3 = mystl::count(people.begin(), people.end(), Person{"", 20});
    EXPECT_EQ(count3, 3);  // 应该找到3个年龄为20的人
}

// 测试 count_if 算法
TEST(AlgorithmTest, CountIf) 
{
    std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    // 测试基本谓词
    auto count1 = mystl::count_if(v.begin(), v.end(), 
                                 [](int n) { return n % 2 == 0; });
    EXPECT_EQ(count1, 5);  // 应该找到5个偶数

    // 测试找不到的情况
    auto count2 = mystl::count_if(v.begin(), v.end(), 
                                 [](int n) { return n > 100; });
    EXPECT_EQ(count2, 0);  // 应该找到0个大于100的数

    // 测试复杂谓词
    struct Person 
    {
        std::string name;
        int age;
    };
    std::vector<Person> people{
        {"Alice", 20}, {"Bob", 25}, {"Charlie", 30}, {"David", 20}
    };
    auto count3 = mystl::count_if(people.begin(), people.end(),
                                 [](const Person& p) { return p.age < 25; });
    EXPECT_EQ(count3, 2);  // 应该找到2个年龄小于25的人
}

// 测试 search 算法
TEST(AlgorithmTest, Search) 
{
    // 测试基本类型
    std::vector<int> v1{1, 2, 3, 4, 2, 3, 4, 5};
    std::vector<int> v2{2, 3, 4};
    
    auto it1 = mystl::search(v1.begin(), v1.end(), v2.begin(), v2.end());
    EXPECT_EQ(std::distance(v1.begin(), it1), 1);  // 子序列在位置1处首次出现

    // 测试找不到的情况
    std::vector<int> v3{2, 3, 5};
    auto it2 = mystl::search(v1.begin(), v1.end(), v3.begin(), v3.end());
    EXPECT_EQ(it2, v1.end());

    // 测试空序列
    std::vector<int> empty;
    auto it3 = mystl::search(v1.begin(), v1.end(), empty.begin(), empty.end());
    EXPECT_EQ(it3, v1.begin());  // 空序列应该返回第一个迭代器

    // 测试自定义比较器
    auto it4 = mystl::search(v1.begin(), v1.end(), v2.begin(), v2.end(),
                            [](int a, int b) { return (a % 2) == (b % 2); });
    EXPECT_EQ(std::distance(v1.begin(), it4), 1);  // 从开始位置就能找到奇偶性匹配的序列
}

// 测试 search_n 算法
TEST(AlgorithmTest, SearchN) 
{
    std::vector<int> v{1, 2, 2, 2, 3, 4, 2, 2, 5};
    
    // 测试基本查找
    auto it1 = mystl::search_n(v.begin(), v.end(), 3, 2);
    EXPECT_EQ(std::distance(v.begin(), it1), 1);  // 从位置1开始有3个连续的2

    // 测试找不到的情况
    auto it2 = mystl::search_n(v.begin(), v.end(), 4, 2);
    EXPECT_EQ(it2, v.end());  // 没有4个连续的2

    // 测试count为0的情况
    auto it3 = mystl::search_n(v.begin(), v.end(), 0, 2);
    EXPECT_EQ(it3, v.begin());  // count为0应该返回第一个迭代器

    // 测试自定义比较器
    auto it4 = mystl::search_n(v.begin(), v.end(), 2, 4,
                              [](int a, int b) { return (a % 2) == (b % 2); });
    EXPECT_EQ(std::distance(v.begin(), it4), 1);  // 从位置1开始有2个连续的偶数
}

// 测试 replace 算法
TEST(AlgorithmTest, Replace) 
{
    // 测试基本类型
    std::vector<int> v1{1, 2, 2, 3, 2, 4, 2, 5};
    mystl::replace(v1.begin(), v1.end(), 2, 0);
    std::vector<int> expected1{1, 0, 0, 3, 0, 4, 0, 5};
    EXPECT_EQ(v1, expected1);

    // 测试自定义类型
    struct Person 
    {
        std::string name;
        int age;
        bool operator==(const Person& other) const 
        {
            return age == other.age;
        }
    };
    std::vector<Person> people
    {
        {"Alice", 20}, {"Bob", 25}, {"Charlie", 20}, {"David", 20}
    };
    mystl::replace(people.begin(), people.end(), 
                  Person{"", 20}, Person{"Unknown", 0});
    EXPECT_EQ(people[0].age, 0);
    EXPECT_EQ(people[0].name, "Unknown");
}

// 测试 replace_if 算法
TEST(AlgorithmTest, ReplaceIf) 
{
    // 测试基本类型
    std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    mystl::replace_if(v.begin(), v.end(),
                     [](int n) { return n % 2 == 0; }, 0);
    std::vector<int> expected{1, 0, 3, 0, 5, 0, 7, 0, 9, 0};
    EXPECT_EQ(v, expected);

    // 测试自定义类型
    struct Person 
    {
        std::string name;
        int age;
    };
    std::vector<Person> people
    {
        {"Alice", 20}, {"Bob", 25}, {"Charlie", 30}, {"David", 20}
    };
    mystl::replace_if(people.begin(), people.end(),
                     [](const Person& p) { return p.age < 25; },
                     Person{"Young", 0});
    EXPECT_EQ(people[0].name, "Young");
    EXPECT_EQ(people[0].age, 0);
}

// 测试 replace_copy 算法
TEST(AlgorithmTest, ReplaceCopy) 
{
    // 测试基本类型
    std::vector<int> v1{1, 2, 2, 3, 2, 4, 2, 5};
    std::vector<int> result1(v1.size());
    mystl::replace_copy(v1.begin(), v1.end(), result1.begin(), 2, 0);
    std::vector<int> expected1{1, 0, 0, 3, 0, 4, 0, 5};
    EXPECT_EQ(result1, expected1);
    EXPECT_EQ(v1[1], 2);  // 原序列不应被修改

    // 测试空序列
    std::vector<int> empty, result2;
    mystl::replace_copy(empty.begin(), empty.end(), 
                       std::back_inserter(result2), 1, 0);
    EXPECT_TRUE(result2.empty());
}

// 测试 replace_copy_if 算法
TEST(AlgorithmTest, ReplaceCopyIf) 
{
    // 测试基本类型
    std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::vector<int> result(v.size());
    mystl::replace_copy_if(v.begin(), v.end(), result.begin(),
                          [](int n) { return n % 2 == 0; }, 0);
    std::vector<int> expected{1, 0, 3, 0, 5, 0, 7, 0, 9, 0};
    EXPECT_EQ(result, expected);
    EXPECT_EQ(v[1], 2);  // 原序列不应被修改

    // 测试空序列
    std::vector<int> empty, result2;
    mystl::replace_copy_if(empty.begin(), empty.end(), 
                          std::back_inserter(result2),
                          [](int n) { return n % 2 == 0; }, 0);
    EXPECT_TRUE(result2.empty());
}

// 测试 remove 算法
TEST(AlgorithmTest, Remove) 
{
    // 测试基本类型
    std::vector<int> v1{1, 2, 2, 3, 2, 4, 2, 5};
    auto new_end = mystl::remove(v1.begin(), v1.end(), 2);
    v1.erase(new_end, v1.end());  // 真正删除元素
    std::vector<int> expected1{1, 3, 4, 5};
    EXPECT_EQ(v1, expected1);

    // 测试自定义类型
    struct Person 
    {
        std::string name;
        int age;
        bool operator==(const Person& other) const 
        {
            return age == other.age;
        }
    };
    std::vector<Person> people
    {
        {"Alice", 20}, {"Bob", 25}, {"Charlie", 20}, {"David", 20}
    };
    auto new_end2 = mystl::remove(people.begin(), people.end(), Person{"", 20});
    people.erase(new_end2, people.end());
    EXPECT_EQ(people.size(), 1);  // 应该只剩下Bob
    EXPECT_EQ(people[0].name, "Bob");
}

// 测试 remove_if 算法
TEST(AlgorithmTest, RemoveIf) 
{
    // 测试基本类型
    std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto new_end = mystl::remove_if(v.begin(), v.end(),
                                   [](int n) { return n % 2 == 0; });
    v.erase(new_end, v.end());
    std::vector<int> expected{1, 3, 5, 7, 9};
    EXPECT_EQ(v, expected);

    // 测试自定义类型
    struct Person 
    {
        std::string name;
        int age;
    };
    std::vector<Person> people
    {
        {"Alice", 20}, {"Bob", 25}, {"Charlie", 30}, {"David", 20}
    };
    auto new_end2 = mystl::remove_if(people.begin(), people.end(),
                                    [](const Person& p) { return p.age < 25; });
    people.erase(new_end2, people.end());
    EXPECT_EQ(people.size(), 2);  // 应该剩下Bob和Charlie
    EXPECT_EQ(people[0].name, "Bob");
    EXPECT_EQ(people[1].name, "Charlie");
}

// 测试 remove_copy 算法
TEST(AlgorithmTest, RemoveCopy) 
{
    // 测试基本类型
    std::vector<int> v1{1, 2, 2, 3, 2, 4, 2, 5};
    std::vector<int> result1;
    mystl::remove_copy(v1.begin(), v1.end(), std::back_inserter(result1), 2);
    std::vector<int> expected1{1, 3, 4, 5};
    EXPECT_EQ(result1, expected1);
    EXPECT_EQ(v1[1], 2);  // 原序列不应被修改

    // 测试空序列
    std::vector<int> empty, result2;
    mystl::remove_copy(empty.begin(), empty.end(), 
                      std::back_inserter(result2), 1);
    EXPECT_TRUE(result2.empty());
}

// 测试 remove_copy_if 算法
TEST(AlgorithmTest, RemoveCopyIf) 
{
    // 测试基本类型
    std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::vector<int> result;
    mystl::remove_copy_if(v.begin(), v.end(), std::back_inserter(result),
                         [](int n) { return n % 2 == 0; });
    std::vector<int> expected{1, 3, 5, 7, 9};
    EXPECT_EQ(result, expected);
    EXPECT_EQ(v[1], 2);  // 原序列不应被修改

    // 测试空序列
    std::vector<int> empty, result2;
    mystl::remove_copy_if(empty.begin(), empty.end(), 
                         std::back_inserter(result2),
                         [](int n) { return n % 2 == 0; });
    EXPECT_TRUE(result2.empty());
} 