#include <gtest/gtest.h>
#include "../include/mystl/list.hpp"

// 构造函数测试
TEST(ListTest, Constructor) 
{
    // 默认构造函数
    mystl::list<int> lst1;
    EXPECT_TRUE(lst1.empty());
    EXPECT_EQ(lst1.size(), 0);

    // 填充构造函数
    mystl::list<int> lst2(3, 2);
    EXPECT_EQ(lst2.size(), 3);
    for (const auto& x : lst2) 
    {
        EXPECT_EQ(x, 2);
    }
}

// 元素访问测试
TEST(ListTest, ElementAccess) 
{
    mystl::list<int> lst;
    lst.push_back(1);
    lst.push_back(2);
    lst.push_back(3);

    EXPECT_EQ(lst.front(), 1);
    EXPECT_EQ(lst.back(), 3);
}

// 迭代器测试
TEST(ListTest, Iterator) 
{
    mystl::list<int> lst{1, 2, 3};
    auto it = lst.begin();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 2);
    --it;
    EXPECT_EQ(*it, 1);
}

// 容量操作测试
TEST(ListTest, Capacity) 
{
    mystl::list<int> lst;
    EXPECT_TRUE(lst.empty());
    EXPECT_EQ(lst.size(), 0);

    lst.push_back(1);
    EXPECT_FALSE(lst.empty());
    EXPECT_EQ(lst.size(), 1);
}

// 修改操作测试
TEST(ListTest, Modifiers) 
{
    mystl::list<int> lst;
    
    // push_back
    lst.push_back(1);
    EXPECT_EQ(lst.back(), 1);
    
    // push_front
    lst.push_front(0);
    EXPECT_EQ(lst.front(), 0);
    
    // insert
    auto it = lst.begin();
    ++it;
    lst.insert(it, 5);
    EXPECT_EQ(lst.size(), 3);
    
    // erase
    it = lst.begin();
    ++it;
    lst.erase(it);
    EXPECT_EQ(lst.size(), 2);
    
    // clear
    lst.clear();
    EXPECT_TRUE(lst.empty());
}

// 链表操作测试
TEST(ListTest, Operations) 
{
    // merge测试
    mystl::list<int> lst1{1, 3, 5};
    mystl::list<int> lst2{2, 4, 6};
    lst1.merge(lst2);
    
    std::vector<int> expected{1, 2, 3, 4, 5, 6};
    auto it = expected.begin();
    for (const auto& x : lst1) 
    {
        EXPECT_EQ(x, *it++);
    }
    EXPECT_TRUE(lst2.empty());

    // reverse测试
    mystl::list<int> lst3{1, 2, 3};
    lst3.reverse();
    std::vector<int> expected_rev{3, 2, 1};
    it = expected_rev.begin();
    for (const auto& x : lst3) 
    {
        EXPECT_EQ(x, *it++);
    }

    // unique测试
    mystl::list<int> lst4{1, 1, 2, 2, 3, 3};
    lst4.unique();
    std::vector<int> expected_unq{1, 2, 3};
    it = expected_unq.begin();
    for (const auto& x : lst4) 
    {
        EXPECT_EQ(x, *it++);
    }

    // remove测试
    mystl::list<int> lst5{1, 2, 2, 3};
    lst5.remove(2);
    std::vector<int> expected_rem{1, 3};
    it = expected_rem.begin();
    for (const auto& x : lst5) 
    {
        EXPECT_EQ(x, *it++);
    }
}

// 异常安全测试
class ListThrowOnCopy
{
public:
    static bool should_throw;
    ListThrowOnCopy() = default;
    ListThrowOnCopy(const ListThrowOnCopy&) 
    {
        if (should_throw) 
        {
            throw std::runtime_error("Copy error");
        }
    }
};

bool ListThrowOnCopy::should_throw = false;

TEST(ListTest, ListExceptionSafety) 
{
    mystl::list<ListThrowOnCopy> lst;
    lst.push_back(ListThrowOnCopy());
    
    ListThrowOnCopy::should_throw = true;
    EXPECT_THROW(lst.push_back(ListThrowOnCopy()), std::runtime_error);
    EXPECT_EQ(lst.size(), 1);
    
    ListThrowOnCopy::should_throw = false;
} 