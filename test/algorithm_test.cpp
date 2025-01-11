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