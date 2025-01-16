#include <gtest/gtest.h>
#include "mystl/deque.hpp"

// 构造函数测试
TEST(DequeTest, Constructor) 
{
    // 默认构造
    mystl::deque<int> d1;
    EXPECT_TRUE(d1.empty());
    EXPECT_EQ(d1.size(), 0);

    // 大小构造
    mystl::deque<int> d2(5);
    EXPECT_EQ(d2.size(), 5);
    for (const auto& elem : d2) 
    {
        EXPECT_EQ(elem, 0);
    }

    // 填充构造
    mystl::deque<int> d3(5, 42);
    EXPECT_EQ(d3.size(), 5);
    for (const auto& elem : d3) 
    {
        EXPECT_EQ(elem, 42);
    }

    // 范围构造
    int arr[] = {1, 2, 3, 4, 5};
    mystl::deque<int> d4(arr, arr + 5);
    EXPECT_EQ(d4.size(), 5);
    for (size_t i = 0; i < d4.size(); ++i) 
    {
        EXPECT_EQ(d4[i], arr[i]);
    }

    // 拷贝构造
    mystl::deque<int> d5(d4);
    EXPECT_EQ(d5, d4);

    // 移动构造
    mystl::deque<int> d6(std::move(d5));
    EXPECT_TRUE(d5.empty());
    EXPECT_EQ(d6, d4);
}

// 赋值操作测试
TEST(DequeTest, Assignment) 
{
    mystl::deque<int> d1{1, 2, 3};

    // 拷贝赋值
    mystl::deque<int> d2;
    d2 = d1;
    EXPECT_EQ(d2, d1);

    // 移动赋值
    mystl::deque<int> d3;
    d3 = std::move(d2);
    EXPECT_TRUE(d2.empty());
    EXPECT_EQ(d3, d1);

    // assign - 范围
    d3.assign(d1.begin(), d1.end());
    EXPECT_EQ(d3, d1);

    // assign - 填充
    d3.assign(3, 42);
    EXPECT_EQ(d3, mystl::deque<int>(3, 42));
}

// 元素访问测试
TEST(DequeTest, ElementAccess) 
{
    mystl::deque<int> d{1, 2, 3};
    const mystl::deque<int>& cd = d;

    // operator[]
    EXPECT_EQ(d[0], 1);
    EXPECT_EQ(cd[0], 1);

    // at()
    EXPECT_EQ(d.at(1), 2);
    EXPECT_EQ(cd.at(1), 2);
    EXPECT_THROW(d.at(3), std::out_of_range);

    // front()
    EXPECT_EQ(d.front(), 1);
    EXPECT_EQ(cd.front(), 1);

    // back()
    EXPECT_EQ(d.back(), 3);
    EXPECT_EQ(cd.back(), 3);
}

// 迭代器测试
TEST(DequeTest, Iterators) 
{
    mystl::deque<int> d{1, 2, 3};
    const mystl::deque<int>& cd = d;

    // begin/end
    EXPECT_EQ(*d.begin(), 1);
    EXPECT_EQ(*(d.end() - 1), 3);

    // cbegin/cend
    EXPECT_EQ(*d.cbegin(), 1);
    EXPECT_EQ(*(d.cend() - 1), 3);

    // rbegin/rend
    EXPECT_EQ(*d.rbegin(), 3);
    EXPECT_EQ(*(d.rend() - 1), 1);

    // crbegin/crend
    EXPECT_EQ(*d.crbegin(), 3);
    EXPECT_EQ(*(d.crend() - 1), 1);
}

// 容量操作测试
TEST(DequeTest, Capacity) 
{
    mystl::deque<int> d;

    // empty/size
    EXPECT_TRUE(d.empty());
    EXPECT_EQ(d.size(), 0);

    // resize
    d.resize(5);
    EXPECT_EQ(d.size(), 5);
    for (size_t i = 0; i < d.size(); ++i) 
    {
        EXPECT_EQ(d[i], 0);
    }

    d.resize(2);
    EXPECT_EQ(d.size(), 2);
    EXPECT_EQ(d[0], 0);
    EXPECT_EQ(d[1], 0);

    d.resize(4, 42);
    EXPECT_EQ(d.size(), 4);
    EXPECT_EQ(d[2], 42);
    EXPECT_EQ(d[3], 42);
}

// 修改器测试
TEST(DequeTest, Modifiers) 
{
    mystl::deque<int> d;

    // push_back/push_front
    d.push_back(2);
    d.push_front(1);
    d.push_back(3);
    EXPECT_EQ(d.size(), 3);
    EXPECT_EQ(d.front(), 1);
    EXPECT_EQ(d.back(), 3);

    // pop_back/pop_front
    d.pop_back();
    d.pop_front();
    EXPECT_EQ(d.size(), 1);
    EXPECT_EQ(d.front(), 2);

    // insert
    auto it = d.insert(d.begin(), 1);
    EXPECT_EQ(*it, 1);
    d.insert(d.end(), 3);
    EXPECT_EQ(d.size(), 3);

    // erase
    it = d.erase(d.begin());
    EXPECT_EQ(*it, 2);

    // clear
    d.clear();
    EXPECT_TRUE(d.empty());
}

// 比较操作测试
TEST(DequeTest, Comparison) 
{
    mystl::deque<int> d1{1, 2, 3};
    mystl::deque<int> d2{1, 2, 3};
    mystl::deque<int> d3{1, 2, 4};
    mystl::deque<int> d4{1, 2};

    EXPECT_TRUE(d1 == d2);
    EXPECT_FALSE(d1 == d3);
    EXPECT_TRUE(d1 != d3);
    EXPECT_TRUE(d1 > d4);
    EXPECT_TRUE(d4 < d1);
    EXPECT_TRUE(d1 >= d2);
    EXPECT_TRUE(d1 <= d2);
} 