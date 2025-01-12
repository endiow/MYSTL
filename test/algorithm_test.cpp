#include <gtest/gtest.h>
#include "mystl/algorithm.hpp"
#include "mystl/vector.hpp"
#include "mystl/functional.hpp"
#include <string>


// 测试 find 算法
TEST(AlgorithmTest, Find) 
{
    // 测试基本类型
    mystl::vector<int> v1{1, 2, 3, 4, 5};
    auto it1 = mystl::find(v1.begin(), v1.end(), 3);
    EXPECT_EQ(*it1, 3);
    EXPECT_EQ(mystl::distance(v1.begin(), it1), 2);

    // 测试找不到的情况
    auto it2 = mystl::find(v1.begin(), v1.end(), 10);
    EXPECT_EQ(it2, v1.end());

    // 测试自定义类型
    mystl::vector<std::string> v2{"hello", "world", "mystl"};
    auto it3 = mystl::find(v2.begin(), v2.end(), "world");
    EXPECT_EQ(*it3, "world");
}

// 测试 find_if 算法
TEST(AlgorithmTest, FindIf) 
{
    mystl::vector<int> v{1, 2, 3, 4, 5};
    
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
    mystl::vector<Person> people{{"Alice", 20}, {"Bob", 25}, {"Charlie", 30}};
    auto it3 = mystl::find_if(people.begin(), people.end(), 
                             [](const Person& p) { return p.age > 22; });
    EXPECT_EQ(it3->name, "Bob");
}

// 测试 find_if_not 算法
TEST(AlgorithmTest, FindIfNot) 
{
    mystl::vector<int> v{1, 2, 3, 4, 5};
    
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
    mystl::vector<int> v1{1, 2, 3, 4, 5};
    mystl::vector<int> v2{10, 4, 20};
    
    // 测试基本查找
    auto it1 = mystl::find_first_of(v1.begin(), v1.end(), v2.begin(), v2.end());
    EXPECT_EQ(*it1, 4);

    // 测试找不到的情况
    mystl::vector<int> v3{10, 20, 30};
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
    mystl::vector<int> v1{1, 2, 2, 3, 4};
    auto it1 = mystl::adjacent_find(v1.begin(), v1.end());
    EXPECT_EQ(*it1, 2);
    EXPECT_EQ(*(it1 + 1), 2);

    // 测试找不到的情况
    mystl::vector<int> v2{1, 2, 3, 4, 5};
    auto it2 = mystl::adjacent_find(v2.begin(), v2.end());
    EXPECT_EQ(it2, v2.end());

    // 测试自定义比较器
    mystl::vector<int> v3{1, 3, 2, 4, 6};
    auto it3 = mystl::adjacent_find(v3.begin(), v3.end(),
                                   [](int a, int b) { return b == a + 1; });
    EXPECT_EQ(it3, v3.end());  // 序列中没有连续的数字
}

// 测试空容器的情况
TEST(AlgorithmTest, EmptyContainer) 
{
    mystl::vector<int> empty;
    
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
    mystl::vector<int> v1{1, 2, 2, 3, 2, 4, 2, 5};
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
    mystl::vector<Person> people{
        {"Alice", 20}, {"Bob", 25}, {"Charlie", 20}, {"David", 20}
    };
    auto count3 = mystl::count(people.begin(), people.end(), Person{"", 20});
    EXPECT_EQ(count3, 3);  // 应该找到3个年龄为20的人
}

// 测试 count_if 算法
TEST(AlgorithmTest, CountIf) 
{
    mystl::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
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
    mystl::vector<Person> people{
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
    mystl::vector<int> v1{1, 2, 3, 4, 2, 3, 4, 5};
    mystl::vector<int> v2{2, 3, 4};
    
    auto it1 = mystl::search(v1.begin(), v1.end(), v2.begin(), v2.end());
    EXPECT_EQ(mystl::distance(v1.begin(), it1), 1);  // 子序列在位置1处首次出现

    // 测试找不到的情况
    mystl::vector<int> v3{2, 3, 5};
    auto it2 = mystl::search(v1.begin(), v1.end(), v3.begin(), v3.end());
    EXPECT_EQ(it2, v1.end());

    // 测试空序列
    mystl::vector<int> empty;
    auto it3 = mystl::search(v1.begin(), v1.end(), empty.begin(), empty.end());
    EXPECT_EQ(it3, v1.begin());  // 空序列应该返回第一个迭代器

    // 测试自定义比较器
    auto it4 = mystl::search(v1.begin(), v1.end(), v2.begin(), v2.end(),
                            [](int a, int b) { return (a % 2) == (b % 2); });
    EXPECT_EQ(mystl::distance(v1.begin(), it4), 1);  // 从开始位置就能找到奇偶性匹配的序列
}

// 测试 search_n 算法
TEST(AlgorithmTest, SearchN) 
{
    mystl::vector<int> v{1, 2, 2, 2, 3, 4, 2, 2, 5};
    
    // 测试基本查找
    auto it1 = mystl::search_n(v.begin(), v.end(), 3, 2);
    EXPECT_EQ(mystl::distance(v.begin(), it1), 1);  // 从位置1开始有3个连续的2

    // 测试找不到的情况
    auto it2 = mystl::search_n(v.begin(), v.end(), 4, 2);
    EXPECT_EQ(it2, v.end());  // 没有4个连续的2

    // 测试count为0的情况
    auto it3 = mystl::search_n(v.begin(), v.end(), 0, 2);
    EXPECT_EQ(it3, v.begin());  // count为0应该返回第一个迭代器

    // 测试自定义比较器
    auto it4 = mystl::search_n(v.begin(), v.end(), 2, 4,
                              [](int a, int b) { return (a % 2) == (b % 2); });
    EXPECT_EQ(mystl::distance(v.begin(), it4), 1);  // 从位置1开始有2个连续的偶数
}

// 测试 replace 算法
TEST(AlgorithmTest, Replace) 
{
    // 测试基本类型
    mystl::vector<int> v1{1, 2, 2, 3, 2, 4, 2, 5};
    mystl::replace(v1.begin(), v1.end(), 2, 0);
    mystl::vector<int> expected1{1, 0, 0, 3, 0, 4, 0, 5};
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
    mystl::vector<Person> people
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
    mystl::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    mystl::replace_if(v.begin(), v.end(),
                     [](int n) { return n % 2 == 0; }, 0);
    mystl::vector<int> expected{1, 0, 3, 0, 5, 0, 7, 0, 9, 0};
    EXPECT_EQ(v, expected);

    // 测试自定义类型
    struct Person 
    {
        std::string name;
        int age;
    };
    mystl::vector<Person> people
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
    mystl::vector<int> v1{1, 2, 2, 3, 2, 4, 2, 5};
    mystl::vector<int> result1(v1.size());
    mystl::replace_copy(v1.begin(), v1.end(), result1.begin(), 2, 0);
    mystl::vector<int> expected1{1, 0, 0, 3, 0, 4, 0, 5};
    EXPECT_EQ(result1, expected1);
    EXPECT_EQ(v1[1], 2);  // 原序列不应被修改

    // 测试空序列
    mystl::vector<int> empty, result2;
    mystl::replace_copy(empty.begin(), empty.end(), 
                       mystl::back_inserter(result2), 1, 0);
    EXPECT_TRUE(result2.empty());
}

// 测试 replace_copy_if 算法
TEST(AlgorithmTest, ReplaceCopyIf) 
{
    // 测试基本类型
    mystl::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    mystl::vector<int> result(v.size());
    mystl::replace_copy_if(v.begin(), v.end(), result.begin(),
                          [](int n) { return n % 2 == 0; }, 0);
    mystl::vector<int> expected{1, 0, 3, 0, 5, 0, 7, 0, 9, 0};
    EXPECT_EQ(result, expected);
    EXPECT_EQ(v[1], 2);  // 原序列不应被修改

    // 测试空序列
    mystl::vector<int> empty, result2;
    mystl::replace_copy_if(empty.begin(), empty.end(), mystl::back_inserter(result2),
                          [](int n) { return n % 2 == 0; }, 0);
    EXPECT_TRUE(result2.empty());
}

// 测试 remove 算法
TEST(AlgorithmTest, Remove) 
{
    // 测试基本类型
    mystl::vector<int> v1{1, 2, 2, 3, 2, 4, 2, 5};
    auto new_end = mystl::remove(v1.begin(), v1.end(), 2);
    v1.erase(new_end, v1.end());  // 真正删除元素
    mystl::vector<int> expected1{1, 3, 4, 5};
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
    mystl::vector<Person> people
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
    mystl::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto new_end = mystl::remove_if(v.begin(), v.end(),
                                   [](int n) { return n % 2 == 0; });
    v.erase(new_end, v.end());
    mystl::vector<int> expected{1, 3, 5, 7, 9};
    EXPECT_EQ(v, expected);

    // 测试自定义类型
    struct Person 
    {
        std::string name;
        int age;
    };
    mystl::vector<Person> people
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
    mystl::vector<int> v1{1, 2, 2, 3, 2, 4, 2, 5};
    mystl::vector<int> result1;
    mystl::remove_copy(v1.begin(), v1.end(), mystl::back_inserter(result1), 2);
    mystl::vector<int> expected1{1, 3, 4, 5};
    EXPECT_EQ(result1, expected1);
    EXPECT_EQ(v1[1], 2);  // 原序列不应被修改

    // 测试空序列
    mystl::vector<int> empty, result2;
    mystl::remove_copy(empty.begin(), empty.end(), 
                      mystl::back_inserter(result2), 1);
    EXPECT_TRUE(result2.empty());
}

// 测试 remove_copy_if 算法
TEST(AlgorithmTest, RemoveCopyIf) 
{
    // 测试基本类型
    mystl::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    mystl::vector<int> result;
    mystl::remove_copy_if(v.begin(), v.end(), mystl::back_inserter(result),
                         [](int n) { return n % 2 == 0; });
    mystl::vector<int> expected{1, 3, 5, 7, 9};
    EXPECT_EQ(result, expected);
    EXPECT_EQ(v[1], 2);  // 原序列不应被修改

    // 测试空序列
    mystl::vector<int> empty, result2;
    mystl::remove_copy_if(empty.begin(), empty.end(), mystl::back_inserter(result2),
                         [](int n) { return n % 2 == 0; });
    EXPECT_TRUE(result2.empty());
}

// 测试 unique 算法
TEST(AlgorithmTest, Unique) 
{
    // 测试基本类型
    mystl::vector<int> v1{1, 1, 2, 2, 2, 3, 3, 4, 5, 5};
    auto new_end = mystl::unique(v1.begin(), v1.end());
    v1.erase(new_end, v1.end());  // 真正删除重复元素
    mystl::vector<int> expected1{1, 2, 3, 4, 5};
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
    mystl::vector<Person> people{
        {"Alice", 20}, {"Bob", 20}, {"Charlie", 25}, 
        {"David", 25}, {"Eve", 30}
    };
    auto new_end2 = mystl::unique(people.begin(), people.end());
    people.erase(new_end2, people.end());
    EXPECT_EQ(people.size(), 3);  // 应该剩下三个不同年龄的人

    // 测试自定义谓词
    mystl::vector<int> v2{1, 2, 4, 7, 11, 12, 14, 15};
    auto new_end3 = mystl::unique(v2.begin(), v2.end(),
                                 [](int a, int b) { return (b - a) < 3; });
    v2.erase(new_end3, v2.end());
    mystl::vector<int> expected2{1, 4, 7, 11, 14};
    EXPECT_EQ(v2, expected2);  // 移除相差小于3的连续元素
}

// 测试 unique_copy 算法
TEST(AlgorithmTest, UniqueCopy) 
{
    // 测试基本类型
    mystl::vector<int> v1{1, 1, 2, 2, 2, 3, 3, 4, 5, 5};
    mystl::vector<int> result1;
    mystl::unique_copy(v1.begin(), v1.end(), mystl::back_inserter(result1));
    mystl::vector<int> expected1{1, 2, 3, 4, 5};
    EXPECT_EQ(result1, expected1);
    EXPECT_EQ(v1[0], 1);  // 原序列不应被修改

    // 测试空序列
    mystl::vector<int> empty, result2;
    mystl::unique_copy(empty.begin(), empty.end(), mystl::back_inserter(result2));
    EXPECT_TRUE(result2.empty());

    // 测试自定义谓词
    mystl::vector<int> v2{1, 2, 4, 7, 11, 12, 14, 15};
    mystl::vector<int> result3;
    mystl::unique_copy(v2.begin(), v2.end(), mystl::back_inserter(result3),
                      [](int a, int b) { return (b - a) < 3; });
    mystl::vector<int> expected2{1, 4, 7, 11, 14};
    EXPECT_EQ(result3, expected2);  // 复制时移除相差小于3的连续元素
}

// 测试 reverse 算法
TEST(AlgorithmTest, Reverse) 
{
    // 测试基本类型
    mystl::vector<int> v1{1, 2, 3, 4, 5};
    mystl::reverse(v1.begin(), v1.end());
    mystl::vector<int> expected1{5, 4, 3, 2, 1};
    EXPECT_EQ(v1, expected1);

    // 测试单个元素
    mystl::vector<int> v2{1};
    mystl::reverse(v2.begin(), v2.end());
    EXPECT_EQ(v2[0], 1);

    // 测试空序列
    mystl::vector<int> empty;
    mystl::reverse(empty.begin(), empty.end());
    EXPECT_TRUE(empty.empty());

    // 测试自定义类型
    struct Person 
    {
        std::string name;
        bool operator==(const Person& other) const 
        {
            return name == other.name;
        }
    };
    mystl::vector<Person> people{{"Alice"}, {"Bob"}, {"Charlie"}};
    mystl::reverse(people.begin(), people.end());
    EXPECT_EQ(people[0].name, "Charlie");
    EXPECT_EQ(people[2].name, "Alice");
}

// 测试 reverse_copy 算法
TEST(AlgorithmTest, ReverseCopy) 
{
    // 测试基本类型
    mystl::vector<int> v1{1, 2, 3, 4, 5};
    mystl::vector<int> result1(v1.size());
    mystl::reverse_copy(v1.begin(), v1.end(), result1.begin());
    mystl::vector<int> expected1{5, 4, 3, 2, 1};
    EXPECT_EQ(result1, expected1);
    EXPECT_EQ(v1[0], 1);  // 原序列不应被修改

    // 测试空序列
    mystl::vector<int> empty, result2;
    mystl::reverse_copy(empty.begin(), empty.end(), mystl::back_inserter(result2));
    EXPECT_TRUE(result2.empty());

    // 测试自定义类型
    struct Person 
    {
        std::string name;
        bool operator==(const Person& other) const 
        {
            return name == other.name;
        }
    };
    mystl::vector<Person> people{{"Alice"}, {"Bob"}, {"Charlie"}};
    mystl::vector<Person> result3;
    mystl::reverse_copy(people.begin(), people.end(), mystl::back_inserter(result3));
    EXPECT_EQ(result3[0].name, "Charlie");
    EXPECT_EQ(result3[2].name, "Alice");
    EXPECT_EQ(people[0].name, "Alice");  // 原序列不应被修改
} 

// 测试 rotate 算法
TEST(AlgorithmTest, Rotate) 
{
    // 测试基本类型
    mystl::vector<int> v1{1, 2, 3, 4, 5};
    auto new_begin = mystl::rotate(v1.begin(), v1.begin() + 2, v1.end());
    mystl::vector<int> expected1{3, 4, 5, 1, 2};
    EXPECT_EQ(v1, expected1);
    EXPECT_EQ(*new_begin, 1);  // 返回原来middle位置的元素的新位置

    // 测试边界情况：middle为begin
    mystl::vector<int> v2{1, 2, 3};
    auto it2 = mystl::rotate(v2.begin(), v2.begin(), v2.end());
    EXPECT_EQ(it2, v2.end());
    EXPECT_EQ(v2, (mystl::vector<int>{1, 2, 3}));

    // 测试边界情况：middle为end
    mystl::vector<int> v3{1, 2, 3};
    auto it3 = mystl::rotate(v3.begin(), v3.end(), v3.end());
    EXPECT_EQ(it3, v3.begin());
    EXPECT_EQ(v3, (mystl::vector<int>{1, 2, 3}));

    // 测试自定义类型
    struct Person 
    {
        std::string name;
        bool operator==(const Person& other) const 
        {
            return name == other.name;
        }
    };
    mystl::vector<Person> people{{"Alice"}, {"Bob"}, {"Charlie"}, {"David"}};
    mystl::rotate(people.begin(), people.begin() + 1, people.end());
    EXPECT_EQ(people[0].name, "Bob");
    EXPECT_EQ(people.back().name, "Alice");
}

// 测试 rotate_copy 算法
TEST(AlgorithmTest, RotateCopy) 
{
    // 测试基本类型
    mystl::vector<int> v1{1, 2, 3, 4, 5};
    mystl::vector<int> result1(v1.size());
    mystl::rotate_copy(v1.begin(), v1.begin() + 2, v1.end(), result1.begin());
    mystl::vector<int> expected1{3, 4, 5, 1, 2};
    EXPECT_EQ(result1, expected1);
    EXPECT_EQ(v1[0], 1);  // 原序列不应被修改

    // 测试空序列
    mystl::vector<int> empty, result2;
    mystl::rotate_copy(empty.begin(), empty.begin(), empty.end(), mystl::back_inserter(result2));
    EXPECT_TRUE(result2.empty());

    // 测试自定义类型
    struct Person 
    {
        std::string name;
        bool operator==(const Person& other) const 
        {
            return name == other.name;
        }
    };
    mystl::vector<Person> people{{"Alice"}, {"Bob"}, {"Charlie"}, {"David"}};
    mystl::vector<Person> result3;
    mystl::rotate_copy(people.begin(), people.begin() + 2, people.end(), mystl::back_inserter(result3));
    EXPECT_EQ(result3[0].name, "Charlie");
    EXPECT_EQ(result3.back().name, "Bob");
    EXPECT_EQ(people[0].name, "Alice");  // 原序列不应被修改
}

// 测试 random_shuffle 算法
TEST(AlgorithmTest, RandomShuffle) 
{
    // 测试基本类型
    mystl::vector<int> v1{1, 2, 3, 4, 5};
    mystl::vector<int> original_int = v1;
    mystl::random_shuffle(v1.begin(), v1.end());
    
    // 检查元素是否仍然存在
    mystl::vector<int> sorted = v1;
    std::sort(sorted.begin(), sorted.end());
    EXPECT_EQ(sorted, original_int);
    
    // 检查是否真的打乱
    EXPECT_NE(v1, original_int);

    // 测试自定义随机数生成器
    mystl::vector<int> v2{1, 2, 3, 4, 5};
    original_int = v2;
    int seed = 42;
    auto rand_func = [&seed](int n) -> int 
    { 
        seed = (seed * 1103515245 + 12345) & 0x7fffffff;
        return seed % n;
    };
    mystl::random_shuffle(v2.begin(), v2.end(), rand_func);
    
    // 检查元素是否仍然存在
    sorted = v2;
    std::sort(sorted.begin(), sorted.end());
    EXPECT_EQ(sorted, original_int);
    
    // 检查是否真的打乱
    EXPECT_NE(v2, original_int);

    // 测试单个元素
    mystl::vector<int> v3{1};
    mystl::random_shuffle(v3.begin(), v3.end());
    EXPECT_EQ(v3[0], 1);

    // 测试空序列
    mystl::vector<int> empty;
    mystl::random_shuffle(empty.begin(), empty.end());
    EXPECT_TRUE(empty.empty());

    // 测试自定义类型
    struct Person 
    {
        std::string name;
        bool operator==(const Person& other) const 
        {
            return name == other.name;
        }
    };
    mystl::vector<Person> people{{"Alice"}, {"Bob"}, {"Charlie"}, {"David"}};
    mystl::vector<Person> original_people = people;

    mystl::random_shuffle(people.begin(), people.end());
    
    // 检查所有元素是否仍然存在
    auto name_less = [](const Person& a, const Person& b) { return a.name < b.name; };
    std::sort(people.begin(), people.end(), name_less);
    std::sort(original_people.begin(), original_people.end(), name_less);
    EXPECT_EQ(people, original_people);
}

// 测试排序算法
TEST(AlgorithmTest, Sort) 
{
    // 测试基本类型
    mystl::vector<int> v1{5, 4, 3, 2, 1};
    mystl::sort(v1.begin(), v1.end());
    EXPECT_TRUE(mystl::is_sorted(v1.begin(), v1.end()));
    
    // 测试自定义比较器
    mystl::vector<int> v2{1, 2, 3, 4, 5};
    mystl::sort(v2.begin(), v2.end(), mystl::greater<int>());
    EXPECT_TRUE(mystl::is_sorted(v2.begin(), v2.end(), mystl::greater<int>()));

    // 测试自定义类型
    struct Person 
    {
        std::string name;
        int age;
        bool operator<(const Person& other) const { return age < other.age; }
    };
    mystl::vector<Person> people
    {
        {"Alice", 30}, {"Bob", 20}, {"Charlie", 25}
    };
    mystl::sort(people.begin(), people.end());
    EXPECT_EQ(people[0].name, "Bob");
    EXPECT_EQ(people[2].name, "Alice");

    // 测试空序列
    mystl::vector<int> empty;
    mystl::sort(empty.begin(), empty.end());
    EXPECT_TRUE(empty.empty());

    // 测试单个元素
    mystl::vector<int> single{1};
    mystl::sort(single.begin(), single.end());
    EXPECT_EQ(single[0], 1);

    // 测试重复元素
    mystl::vector<int> duplicates{3, 1, 4, 1, 5, 9, 2, 6, 5, 3};
    mystl::sort(duplicates.begin(), duplicates.end());
    EXPECT_TRUE(mystl::is_sorted(duplicates.begin(), duplicates.end()));
}

// 测试各种排序算法
TEST(AlgorithmTest, SortingAlgorithms) 
{
    // 准备测试数据
    mystl::vector<int> original{3, 1, 4, 1, 5, 9, 2, 6, 5, 3};
    mystl::vector<int> v;

    // 测试插入排序
    v = original;
    mystl::insertion_sort(v.begin(), v.end());
    EXPECT_TRUE(mystl::is_sorted(v.begin(), v.end()));

    // 测试快速排序
    v = original;
    mystl::quick_sort(v.begin(), v.end());
    EXPECT_TRUE(mystl::is_sorted(v.begin(), v.end()));

    // 测试堆排序
    v = original;
    mystl::heap_sort(v.begin(), v.end());
    EXPECT_TRUE(mystl::is_sorted(v.begin(), v.end()));

    // 测试归并排序
    v = original;
    mystl::merge_sort(v.begin(), v.end());
    EXPECT_TRUE(mystl::is_sorted(v.begin(), v.end()));

    // 测试选择排序
    v = original;
    mystl::selection_sort(v.begin(), v.end());
    EXPECT_TRUE(mystl::is_sorted(v.begin(), v.end()));

    // 测试冒泡排序
    v = original;
    mystl::bubble_sort(v.begin(), v.end());
    EXPECT_TRUE(mystl::is_sorted(v.begin(), v.end()));

    // 测试希尔排序
    v = original;
    mystl::shell_sort(v.begin(), v.end());
    EXPECT_TRUE(mystl::is_sorted(v.begin(), v.end()));

    // 测试内省排序
    v = original;
    mystl::introsort(v.begin(), v.end());
    EXPECT_TRUE(mystl::is_sorted(v.begin(), v.end()));
}

// 测试排序算法的稳定性
TEST(AlgorithmTest, SortStability) 
{
    struct Record 
    {
        int key;
        int index;  // 原始位置
        bool operator<(const Record& other) const { return key < other.key; }
        bool operator==(const Record& other) const { return key == other.key; }
    };

    // 准备测试数据
    mystl::vector<Record> original
    {
        {1, 0}, {3, 1}, {1, 2}, {2, 3}, {2, 4}, {1, 5}
    };
    mystl::vector<Record> v;

    // 测试稳定排序算法
    // 插入排序
    v = original;
    mystl::insertion_sort(v.begin(), v.end());
    for (size_t i = 1; i < v.size(); ++i) 
    {
        if (v[i].key == v[i-1].key) 
        {
            EXPECT_LT(v[i-1].index, v[i].index);  // 检查相等元素的相对顺序是否保持
        }
    }

    // 归并排序
    v = original;
    mystl::merge_sort(v.begin(), v.end());
    for (size_t i = 1; i < v.size(); ++i) 
    {
        if (v[i].key == v[i-1].key) 
        {
            EXPECT_LT(v[i-1].index, v[i].index);
        }
    }

    // 冒泡排序
    v = original;
    mystl::bubble_sort(v.begin(), v.end());
    for (size_t i = 1; i < v.size(); ++i) 
    {
        if (v[i].key == v[i-1].key) 
        {
            EXPECT_LT(v[i-1].index, v[i].index);
        }
    }
}

// 测试 is_sorted 和 is_sorted_until 算法
TEST(AlgorithmTest, IsSorted) 
{
    // 测试 is_sorted - 基本类型
    mystl::vector<int> v1{1, 2, 3, 4, 5};
    EXPECT_TRUE(mystl::is_sorted(v1.begin(), v1.end()));

    mystl::vector<int> v2{5, 4, 3, 2, 1};
    EXPECT_FALSE(mystl::is_sorted(v2.begin(), v2.end()));
    EXPECT_TRUE(mystl::is_sorted(v2.begin(), v2.end(), mystl::greater<int>()));

    // 测试部分有序
    mystl::vector<int> v3{1, 2, 4, 3, 5};
    EXPECT_FALSE(mystl::is_sorted(v3.begin(), v3.end()));

    // 测试边界情况
    mystl::vector<int> empty;
    EXPECT_TRUE(mystl::is_sorted(empty.begin(), empty.end()));

    mystl::vector<int> single{1};
    EXPECT_TRUE(mystl::is_sorted(single.begin(), single.end()));

    mystl::vector<int> same{2, 2, 2, 2};
    EXPECT_TRUE(mystl::is_sorted(same.begin(), same.end()));

    // 测试自定义类型
    struct Person 
    {
        std::string name;
        int age;
        bool operator<(const Person& other) const { return age < other.age; }
    };

    mystl::vector<Person> people
    {
        {"Alice", 20}, {"Bob", 25}, {"Charlie", 30}
    };
    EXPECT_TRUE(mystl::is_sorted(people.begin(), people.end()));

    mystl::vector<Person> unsorted_people
    {
        {"Alice", 30}, {"Bob", 20}, {"Charlie", 25}
    };
    EXPECT_FALSE(mystl::is_sorted(unsorted_people.begin(), unsorted_people.end()));

    // 测试 is_sorted_until - 基本类型
    mystl::vector<int> v4{1, 2, 3, 2, 4, 5};
    auto it1 = mystl::is_sorted_until(v4.begin(), v4.end());
    EXPECT_EQ(mystl::distance(v4.begin(), it1), 3);

    // 完全有序的情况
    auto it2 = mystl::is_sorted_until(v1.begin(), v1.end());
    EXPECT_EQ(it2, v1.end());

    // 完全逆序的情况
    auto it3 = mystl::is_sorted_until(v2.begin(), v2.end(), mystl::greater<int>());
    EXPECT_EQ(it3, v2.end());

    // 边界情况
    auto it4 = mystl::is_sorted_until(empty.begin(), empty.end());
    EXPECT_EQ(it4, empty.end());

    auto it5 = mystl::is_sorted_until(single.begin(), single.end());
    EXPECT_EQ(it5, single.end());

    // 测试自定义类型的 is_sorted_until
    mystl::vector<Person> partially_sorted_people
    {
        {"Alice", 20}, {"Bob", 25}, {"David", 22}, {"Charlie", 30}
    };
    auto it6 = mystl::is_sorted_until(partially_sorted_people.begin(), partially_sorted_people.end());
    EXPECT_EQ(mystl::distance(partially_sorted_people.begin(), it6), 2);

    // 测试自定义比较器
    auto age_greater = [](const Person& a, const Person& b) { return a.age > b.age; };
    mystl::vector<Person> reverse_sorted_people
    {
        {"Charlie", 30}, {"Bob", 25}, {"Alice", 20}
    };
    EXPECT_TRUE(mystl::is_sorted(reverse_sorted_people.begin(), reverse_sorted_people.end(), age_greater));

    // 测试相等元素
    mystl::vector<Person> equal_age_people
    {
        {"Alice", 25}, {"Bob", 25}, {"Charlie", 25}
    };
    EXPECT_TRUE(mystl::is_sorted(equal_age_people.begin(), equal_age_people.end()));
}

// 测试二分查找算法
TEST(AlgorithmTest, BinarySearch) 
{
    // 测试基本类型
    mystl::vector<int> v{1, 2, 2, 2, 3, 4, 5};

    // 测试 lower_bound
    auto it1 = mystl::lower_bound(v.begin(), v.end(), 2);
    EXPECT_EQ(*it1, 2);
    EXPECT_EQ(mystl::distance(v.begin(), it1), 1);  // 第一个大于等于2的位置

    auto it2 = mystl::lower_bound(v.begin(), v.end(), 6);
    EXPECT_EQ(it2, v.end());  // 找不到大于等于6的元素

    // 测试 upper_bound
    auto it3 = mystl::upper_bound(v.begin(), v.end(), 2);
    EXPECT_EQ(*it3, 3);
    EXPECT_EQ(mystl::distance(v.begin(), it3), 4);  // 第一个大于2的位置

    auto it4 = mystl::upper_bound(v.begin(), v.end(), 6);
    EXPECT_EQ(it4, v.end());  // 找不到大于6的元素

    // 测试 binary_search
    EXPECT_TRUE(mystl::binary_search(v.begin(), v.end(), 3));
    EXPECT_FALSE(mystl::binary_search(v.begin(), v.end(), 6));

    // 测试 equal_range
    auto p1 = mystl::equal_range(v.begin(), v.end(), 2);
    EXPECT_EQ(mystl::distance(v.begin(), p1.first), 1);   // 第一个2的位置
    EXPECT_EQ(mystl::distance(v.begin(), p1.second), 4);  // 最后一个2的下一个位置
    EXPECT_EQ(mystl::distance(p1.first, p1.second), 3);   // 2的个数

    auto p2 = mystl::equal_range(v.begin(), v.end(), 6);
    EXPECT_EQ(p2.first, v.end());
    EXPECT_EQ(p2.second, v.end());

    // 测试自定义类型
    struct Person 
    {
        std::string name;
        int age;
        bool operator<(const Person& other) const { return age < other.age; }
    };

    mystl::vector<Person> people{
        {"Alice", 20}, {"Bob", 25}, {"Charlie", 25}, {"David", 30}
    };

    // 测试 lower_bound
    auto it5 = mystl::lower_bound(people.begin(), people.end(), Person{"", 25});
    EXPECT_EQ(it5->name, "Bob");  // 第一个年龄大于等于25的人

    // 测试 upper_bound
    auto it6 = mystl::upper_bound(people.begin(), people.end(), Person{"", 25});
    EXPECT_EQ(it6->name, "David");  // 第一个年龄大于25的人

    // 测试 binary_search
    EXPECT_TRUE(mystl::binary_search(people.begin(), people.end(), Person{"", 25}));
    EXPECT_FALSE(mystl::binary_search(people.begin(), people.end(), Person{"", 26}));

    // 测试 equal_range
    auto p3 = mystl::equal_range(people.begin(), people.end(), Person{"", 25});
    EXPECT_EQ(p3.first->name, "Bob");      // 第一个年龄为25的人
    EXPECT_EQ(p3.second->name, "David");    // 第一个年龄大于25的人
    EXPECT_EQ(mystl::distance(p3.first, p3.second), 2);  // 年龄为25的人数

    // 测试自定义比较器
    auto comp = [](const Person& a, const Person& b) { return a.age < b.age; };
    auto it7 = mystl::lower_bound(people.begin(), people.end(), Person{"", 25}, comp);
    EXPECT_EQ(it7->name, "Bob");

    // 测试空序列
    mystl::vector<int> empty;
    EXPECT_EQ(mystl::lower_bound(empty.begin(), empty.end(), 1), empty.end());
    EXPECT_EQ(mystl::upper_bound(empty.begin(), empty.end(), 1), empty.end());
    EXPECT_FALSE(mystl::binary_search(empty.begin(), empty.end(), 1));
    auto p4 = mystl::equal_range(empty.begin(), empty.end(), 1);
    EXPECT_EQ(p4.first, empty.end());
    EXPECT_EQ(p4.second, empty.end());

    // 测试单个元素
    mystl::vector<int> single{1};
    EXPECT_EQ(*mystl::lower_bound(single.begin(), single.end(), 1), 1);
    EXPECT_EQ(mystl::upper_bound(single.begin(), single.end(), 1), single.end());
    EXPECT_TRUE(mystl::binary_search(single.begin(), single.end(), 1));
    auto p5 = mystl::equal_range(single.begin(), single.end(), 1);
    EXPECT_EQ(*p5.first, 1);
    EXPECT_EQ(p5.second, single.end());
}

// 测试划分算法
TEST(AlgorithmTest, Partition) 
{
    // 测试基本类型
    mystl::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // 测试 partition
    auto is_even = [](int n) { return n % 2 == 0; };
    auto it1 = mystl::partition(v.begin(), v.end(), is_even);
    EXPECT_TRUE(mystl::all_of(v.begin(), it1, is_even));  // 前半部分都是偶数
    EXPECT_TRUE(mystl::none_of(it1, v.end(), is_even));   // 后半部分没有偶数

    // 测试 stable_partition
    mystl::vector<int> v2{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto it2 = mystl::stable_partition(v2.begin(), v2.end(), is_even);
    EXPECT_TRUE(mystl::all_of(v2.begin(), it2, is_even));  // 前半部分都是偶数
    EXPECT_TRUE(mystl::none_of(it2, v2.end(), is_even));   // 后半部分没有偶数
    EXPECT_EQ(v2, (mystl::vector<int>{2, 4, 6, 8, 10, 1, 3, 5, 7, 9}));  // 保持相对顺序

    // 测试 partition_copy
    mystl::vector<int> v3{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    mystl::vector<int> evens, odds;
    mystl::partition_copy(v3.begin(), v3.end(), mystl::back_inserter(evens), mystl::back_inserter(odds), is_even);
    EXPECT_EQ(evens, (mystl::vector<int>{2, 4, 6, 8, 10}));
    EXPECT_EQ(odds, (mystl::vector<int>{1, 3, 5, 7, 9}));

    // 测试自定义类型
    struct Person 
    {
        std::string name;
        int age;
    };

    mystl::vector<Person> people
    {
        {"Alice", 30}, {"Bob", 20}, {"Charlie", 25}, {"David", 35}
    };

    auto is_adult = [](const Person& p) { return p.age >= 30; };
    auto it3 = mystl::partition(people.begin(), people.end(), is_adult);
    EXPECT_TRUE(mystl::all_of(people.begin(), it3, is_adult));  // 前半部分都是成年人
    EXPECT_TRUE(mystl::none_of(it3, people.end(), is_adult));   // 后半部分没有成年人

    // 测试空序列
    mystl::vector<int> empty;
    auto it4 = mystl::partition(empty.begin(), empty.end(), is_even);
    EXPECT_EQ(it4, empty.end());

    // 测试单个元素
    mystl::vector<int> single{1};
    auto it5 = mystl::partition(single.begin(), single.end(), is_even);
    EXPECT_EQ(it5, single.begin());
}

// 测试排列组合算法
TEST(AlgorithmTest, Permutation) 
{
    // 测试 next_permutation
    {
        mystl::vector<int> v{1, 2, 3};
        std::vector<mystl::vector<int>> permutations;
        
        do 
        {
            permutations.push_back(v);
        } while (mystl::next_permutation(v.begin(), v.end()));
        
        // 验证所有可能的排列
        EXPECT_EQ(permutations.size(), 6);  // 3! = 6
        EXPECT_EQ(permutations[0], (mystl::vector<int>{1, 2, 3}));
        EXPECT_EQ(permutations[1], (mystl::vector<int>{1, 3, 2}));
        EXPECT_EQ(permutations[2], (mystl::vector<int>{2, 1, 3}));
        EXPECT_EQ(permutations[3], (mystl::vector<int>{2, 3, 1}));
        EXPECT_EQ(permutations[4], (mystl::vector<int>{3, 1, 2}));
        EXPECT_EQ(permutations[5], (mystl::vector<int>{3, 2, 1}));
    }
    
    // 测试 prev_permutation
    {
        mystl::vector<int> v{3, 2, 1};
        std::vector<mystl::vector<int>> permutations;
        
        do 
        {
            permutations.push_back(v);
        } while (mystl::prev_permutation(v.begin(), v.end()));
        
        // 验证所有可能的排列（逆序）
        EXPECT_EQ(permutations.size(), 6);  // 3! = 6
        EXPECT_EQ(permutations[0], (mystl::vector<int>{3, 2, 1}));
        EXPECT_EQ(permutations[1], (mystl::vector<int>{3, 1, 2}));
        EXPECT_EQ(permutations[2], (mystl::vector<int>{2, 3, 1}));
        EXPECT_EQ(permutations[3], (mystl::vector<int>{2, 1, 3}));
        EXPECT_EQ(permutations[4], (mystl::vector<int>{1, 3, 2}));
        EXPECT_EQ(permutations[5], (mystl::vector<int>{1, 2, 3}));
    }
    
    // 测试自定义比较器
    {
        mystl::vector<int> v{3, 2, 1};  // 从大到小开始
        std::vector<mystl::vector<int>> permutations;
        
        do 
        {
            permutations.push_back(v);
        } while (mystl::next_permutation(v.begin(), v.end(), mystl::greater<int>()));
        
        // 验证使用 greater 比较器的排列（从大到小开始）
        EXPECT_EQ(permutations.size(), 6);
        EXPECT_EQ(permutations[0], (mystl::vector<int>{3, 2, 1}));
        EXPECT_EQ(permutations[1], (mystl::vector<int>{3, 1, 2}));
        EXPECT_EQ(permutations[2], (mystl::vector<int>{2, 3, 1}));
        EXPECT_EQ(permutations[3], (mystl::vector<int>{2, 1, 3}));
        EXPECT_EQ(permutations[4], (mystl::vector<int>{1, 3, 2}));
        EXPECT_EQ(permutations[5], (mystl::vector<int>{1, 2, 3}));
    }
    
    // 测试边界情况
    {
        // 空序列
        mystl::vector<int> empty;
        EXPECT_FALSE(mystl::next_permutation(empty.begin(), empty.end()));
        EXPECT_FALSE(mystl::prev_permutation(empty.begin(), empty.end()));
        
        // 单个元素
        mystl::vector<int> single{1};
        EXPECT_FALSE(mystl::next_permutation(single.begin(), single.end()));
        EXPECT_FALSE(mystl::prev_permutation(single.begin(), single.end()));
        
        // 相等元素
        mystl::vector<int> equal{1, 1, 1};
        EXPECT_FALSE(mystl::next_permutation(equal.begin(), equal.end()));
        EXPECT_FALSE(mystl::prev_permutation(equal.begin(), equal.end()));
        
        // 部分相等元素
        mystl::vector<int> partial{1, 1, 2};
        int count = 0;
        do 
        {
            ++count;
        } while (mystl::next_permutation(partial.begin(), partial.end()));
        EXPECT_EQ(count, 3);  // 1,1,2 -> 1,2,1 -> 2,1,1
    }
    
    // 测试自定义类型
    {
        struct Point 
        {
            int x, y;
            Point(int x_ = 0, int y_ = 0) : x(x_), y(y_) {}
            bool operator<(const Point& other) const 
            {
                return x < other.x || (x == other.x && y < other.y);
            }
            bool operator==(const Point& other) const 
            {
                return x == other.x && y == other.y;
            }
        };
        
        mystl::vector<Point> points{{1,1}, {1,2}, {2,1}};
        int count = 0;
        do 
        {
            ++count;
        } while (mystl::next_permutation(points.begin(), points.end()));
        EXPECT_EQ(count, 6);  // 3! = 6
    }
}

// 测试集合操作算法
TEST(AlgorithmTest, SetOperations) 
{
    // 测试 merge
    {
        mystl::vector<int> v1{9, 7, 5, 3, 1};  // 降序排列
        mystl::vector<int> v2{10, 8, 6, 4, 2}; // 降序排列
        mystl::vector<int> dest(10);
        
        auto end = mystl::merge(v1.begin(), v1.end(), v2.begin(), v2.end(), dest.begin(), mystl::greater<int>());
        EXPECT_EQ(end, dest.end());
        EXPECT_EQ(dest, (mystl::vector<int>{10, 9, 8, 7, 6, 5, 4, 3, 2, 1}));
    }
    
    // 测试 inplace_merge
    {
        mystl::vector<int> v{9, 7, 5, 3, 1, 10, 8, 6, 4, 2};  // 两段都是降序
        mystl::inplace_merge(v.begin(), v.begin() + 5, v.end(), mystl::greater<int>());
        EXPECT_EQ(v, (mystl::vector<int>{10, 9, 8, 7, 6, 5, 4, 3, 2, 1}));
    }
    
    // 测试 set_union
    {
        mystl::vector<int> v1{1, 2, 3, 4, 5};
        mystl::vector<int> v2{4, 5, 6, 7, 8};
        mystl::vector<int> dest;
        
        mystl::set_union(v1.begin(), v1.end(), v2.begin(), v2.end(), 
                        mystl::back_inserter(dest));
        EXPECT_EQ(dest, (mystl::vector<int>{1, 2, 3, 4, 5, 6, 7, 8}));
        
        // 测试重复元素
        mystl::vector<int> v3{1, 1, 2, 2, 3};
        mystl::vector<int> v4{2, 2, 3, 3, 4};
        mystl::vector<int> dest2;
        mystl::set_union(v3.begin(), v3.end(), v4.begin(), v4.end(), mystl::back_inserter(dest2));
        EXPECT_EQ(dest2, (mystl::vector<int>{1, 1, 2, 2, 3, 3, 4}));
    }
    
    // 测试 set_intersection
    {
        mystl::vector<int> v1{1, 2, 3, 4, 5};
        mystl::vector<int> v2{4, 5, 6, 7, 8};
        mystl::vector<int> dest;
        
        mystl::set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), mystl::back_inserter(dest));
        EXPECT_EQ(dest, (mystl::vector<int>{4, 5}));
        
        // 测试重复元素
        mystl::vector<int> v3{1, 1, 2, 2, 3};
        mystl::vector<int> v4{2, 2, 3, 3, 4};
        mystl::vector<int> dest2;
        mystl::set_intersection(v3.begin(), v3.end(), v4.begin(), v4.end(), mystl::back_inserter(dest2));
        EXPECT_EQ(dest2, (mystl::vector<int>{2, 2, 3}));
    }
    
    // 测试 set_difference
    {
        mystl::vector<int> v1{1, 2, 3, 4, 5};
        mystl::vector<int> v2{4, 5, 6, 7, 8};
        mystl::vector<int> dest;
        
        mystl::set_difference(v1.begin(), v1.end(), v2.begin(), v2.end(), mystl::back_inserter(dest));
        EXPECT_EQ(dest, (mystl::vector<int>{1, 2, 3}));
        
        // 测试重复元素
        mystl::vector<int> v3{1, 1, 2, 2, 3};
        mystl::vector<int> v4{2, 2, 3, 3, 4};
        mystl::vector<int> dest2;
        mystl::set_difference(v3.begin(), v3.end(), v4.begin(), v4.end(), mystl::back_inserter(dest2));
        EXPECT_EQ(dest2, (mystl::vector<int>{1, 1}));
    }
    
    // 测试 set_symmetric_difference
    {
        mystl::vector<int> v1{1, 2, 3, 4, 5};
        mystl::vector<int> v2{4, 5, 6, 7, 8};
        mystl::vector<int> dest;
        
        mystl::set_symmetric_difference(v1.begin(), v1.end(), v2.begin(), v2.end(), mystl::back_inserter(dest));
        EXPECT_EQ(dest, (mystl::vector<int>{1, 2, 3, 6, 7, 8}));
        
        // 测试重复元素
        mystl::vector<int> v3{1, 1, 2, 2, 3};
        mystl::vector<int> v4{2, 2, 3, 3, 4};
        mystl::vector<int> dest2;
        mystl::set_symmetric_difference(v3.begin(), v3.end(), v4.begin(), v4.end(), mystl::back_inserter(dest2));
        EXPECT_EQ(dest2, (mystl::vector<int>{1, 1, 3, 4}));
    }
    
    // 测试边界情况
    {
        mystl::vector<int> empty;
        mystl::vector<int> v{1, 2, 3};
        mystl::vector<int> dest;
        
        // 空序列
        mystl::merge(empty.begin(), empty.end(), empty.begin(), empty.end(), mystl::back_inserter(dest));
        EXPECT_TRUE(dest.empty());
        
        // 一个序列为空
        dest.clear();
        mystl::merge(v.begin(), v.end(), empty.begin(), empty.end(), mystl::back_inserter(dest));
        EXPECT_EQ(dest, v);
        
        // 单个元素
        mystl::vector<int> single1{1};
        mystl::vector<int> single2{2};
        dest.clear();
        mystl::merge(single1.begin(), single1.end(), single2.begin(), single2.end(), mystl::back_inserter(dest));
        EXPECT_EQ(dest, (mystl::vector<int>{1, 2}));
    }
}

// 测试数值算法
TEST(AlgorithmTest, NumericOperations) 
{
    // 测试 accumulate
    {
        mystl::vector<int> v{1, 2, 3, 4, 5};
        
        // 测试默认加法
        int sum = mystl::accumulate(v.begin(), v.end(), 0);
        EXPECT_EQ(sum, 15);  // 1 + 2 + 3 + 4 + 5 = 15
        
        // 测试自定义操作（乘法）
        int product = mystl::accumulate(v.begin(), v.end(), 1, 
            [](int a, int b) { return a * b; });
        EXPECT_EQ(product, 120);  // 1 * 2 * 3 * 4 * 5 = 120
    }
    
    // 测试 inner_product
    {
        mystl::vector<int> v1{1, 2, 3};
        mystl::vector<int> v2{4, 5, 6};
        
        // 测试默认操作（乘加）
        int result1 = mystl::inner_product(v1.begin(), v1.end(), v2.begin(), 0);
        EXPECT_EQ(result1, 32);  // 1*4 + 2*5 + 3*6 = 4 + 10 + 18 = 32
        
        // 测试自定义操作（加乘）
        int result2 = mystl::inner_product(v1.begin(), v1.end(), v2.begin(), 0, mystl::plus<int>(), mystl::multiplies<int>());
        EXPECT_EQ(result2, 32);  // (0 + 1*4) + 2*5 + 3*6 = 32
    }
    
    // 测试 partial_sum
    {
        mystl::vector<int> v{1, 2, 3, 4, 5};
        mystl::vector<int> result1(v.size());
        
        // 测试默认加法
        mystl::partial_sum(v.begin(), v.end(), result1.begin());
        EXPECT_EQ(result1, (mystl::vector<int>{1, 3, 6, 10, 15}));
        
        // 测试自定义操作（乘法）
        mystl::vector<int> result2(v.size());
        mystl::partial_sum(v.begin(), v.end(), result2.begin(), std::multiplies<int>());
        EXPECT_EQ(result2, (mystl::vector<int>{1, 2, 6, 24, 120}));
    }
    
    // 测试 adjacent_difference
    {
        mystl::vector<int> v{1, 3, 6, 10, 15};
        mystl::vector<int> result1(v.size());
        
        // 测试默认减法
        mystl::adjacent_difference(v.begin(), v.end(), result1.begin());
        EXPECT_EQ(result1, (mystl::vector<int>{1, 2, 3, 4, 5}));
        
        // 测试自定义操作（加法）
        mystl::vector<int> result2(v.size());
        mystl::adjacent_difference(v.begin(), v.end(), result2.begin(), std::plus<int>());
        EXPECT_EQ(result2, (mystl::vector<int>{1, 4, 9, 16, 25}));
    }
    
    // 测试边界情况
    {
        // 空序列
        mystl::vector<int> empty;
        mystl::vector<int> result;
        
        int sum = mystl::accumulate(empty.begin(), empty.end(), 0);
        EXPECT_EQ(sum, 0);
        
        mystl::partial_sum(empty.begin(), empty.end(), mystl::back_inserter(result));
        EXPECT_TRUE(result.empty());
        
        mystl::adjacent_difference(empty.begin(), empty.end(), mystl::back_inserter(result));
        EXPECT_TRUE(result.empty());
        
        // 单个元素
        mystl::vector<int> single{1};
        mystl::vector<int> result_single(1);
        
        mystl::partial_sum(single.begin(), single.end(), result_single.begin());
        EXPECT_EQ(result_single, single);
        
        mystl::adjacent_difference(single.begin(), single.end(), result_single.begin());
        EXPECT_EQ(result_single, single);
    }
    
    // 测试自定义类型
    {
        struct Point 
        {
            int x, y;
            Point(int x_ = 0, int y_ = 0) : x(x_), y(y_) {}
            Point operator+(const Point& other) const 
            {
                return Point(x + other.x, y + other.y);
            }
            Point operator-(const Point& other) const 
            {
                return Point(x - other.x, y - other.y);
            }
            bool operator==(const Point& other) const 
            {
                return x == other.x && y == other.y;
            }
        };
        
        mystl::vector<Point> points{{1,1}, {2,2}, {3,3}};
        mystl::vector<Point> result(points.size());
        
        // 测试 partial_sum
        mystl::partial_sum(points.begin(), points.end(), result.begin());
        EXPECT_EQ(result[0], (Point{1,1}));
        EXPECT_EQ(result[1], (Point{3,3}));
        EXPECT_EQ(result[2], (Point{6,6}));
        
        // 测试 adjacent_difference
        mystl::adjacent_difference(result.begin(), result.end(), result.begin());
        EXPECT_EQ(result[0], (Point{1,1}));
        EXPECT_EQ(result[1], (Point{2,2}));
        EXPECT_EQ(result[2], (Point{3,3}));
    }
}