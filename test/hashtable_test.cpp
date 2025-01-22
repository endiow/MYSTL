#include <gtest/gtest.h>
#include "mystl/hashtable.hpp"
#include "mystl/functional.hpp"
#include "throw_on_copy.hpp"

// 基本类型测试
TEST(HashtableTest, BasicType) 
{
    mystl::hashtable<int, int, mystl::hash<int>, 
                    mystl::identity<int>, mystl::equal_to<int>> ht;
    
    // 空表测试
    EXPECT_TRUE(ht.empty());
    EXPECT_EQ(ht.size(), 0);
    EXPECT_GT(ht.bucket_count(), 0);

    // 插入测试
    auto p1 = ht.insert_unique(1);
    EXPECT_TRUE(p1.second);
    EXPECT_EQ(*p1.first, 1);
    EXPECT_EQ(ht.size(), 1);

    // 重复插入测试
    auto p2 = ht.insert_unique(1);
    EXPECT_FALSE(p2.second);
    EXPECT_EQ(ht.size(), 1);

    // 多个元素插入
    ht.insert_unique(2);
    ht.insert_unique(3);
    EXPECT_EQ(ht.size(), 3);

    // 查找测试
    auto it = ht.find(2);
    EXPECT_NE(it, ht.end());
    EXPECT_EQ(*it, 2);

    // 计数测试
    EXPECT_EQ(ht.count(1), 1);
    EXPECT_EQ(ht.count(4), 0);
}

// 允许重复值的测试
TEST(HashtableTest, DuplicateValues) 
{
    mystl::hashtable<int, int, mystl::hash<int>, 
                    mystl::identity<int>, mystl::equal_to<int>> ht;
    
    // 插入重复值
    auto it1 = ht.insert_equal(1);
    auto it2 = ht.insert_equal(1);
    EXPECT_EQ(ht.size(), 2);
    EXPECT_EQ(ht.count(1), 2);

    // 范围查找测试
    auto range = ht.equal_range(1);
    size_t count = 0;
    for (auto it = range.first; it != range.second; ++it)
    {
        ++count;
        EXPECT_EQ(*it, 1);
    }
    EXPECT_EQ(count, 2);
}

// 迭代器测试
TEST(HashtableTest, Iterator) 
{
    mystl::hashtable<int, int, mystl::hash<int>, 
                    mystl::identity<int>, mystl::equal_to<int>> ht;
    
    // 插入一些数据
    ht.insert_unique(1);
    ht.insert_unique(2);
    ht.insert_unique(3);

    // 正向遍历
    size_t count = 0;
    for (auto it = ht.begin(); it != ht.end(); ++it)
    {
        ++count;
        EXPECT_TRUE(*it >= 1 && *it <= 3);
    }
    EXPECT_EQ(count, 3);

    // const 迭代器测试
    const auto& cht = ht;
    count = 0;
    for (auto it = cht.begin(); it != cht.end(); ++it)
    {
        ++count;
        EXPECT_TRUE(*it >= 1 && *it <= 3);
    }
    EXPECT_EQ(count, 3);
}

// 异常安全性测试
TEST(HashtableTest, ExceptionSafety) 
{
    using HT = mystl::hashtable<ThrowOnCopy, ThrowOnCopy, mystl::hash<ThrowOnCopy>,
                               mystl::identity<ThrowOnCopy>, mystl::equal_to<ThrowOnCopy>>;
    
    HT ht;
    ThrowOnCopy::reset();

    // 正常插入
    ht.insert_unique(ThrowOnCopy(1));
    EXPECT_EQ(ht.size(), 1);

    // 插入时抛出异常
    ThrowOnCopy::should_throw = true;
    size_t old_size = ht.size();
    
    try 
    {
        ht.insert_unique(ThrowOnCopy(2));
        FAIL() << "Expected std::runtime_error";
    }
    catch (const std::runtime_error&) 
    {
        // 验证异常安全性：大小不变
        EXPECT_EQ(ht.size(), old_size);
        // 原有元素仍然可以访问
        auto it = ht.find(ThrowOnCopy(1));
        EXPECT_NE(it, ht.end());
        EXPECT_EQ(it->value, 1);
    }
}

// 哈希冲突测试
TEST(HashtableTest, HashCollision) 
{
    // 使用一个总是返回相同哈希值的哈希函数
    struct AlwaysCollideHash 
    {
        size_t operator()(int) const { return 1; }
    };

    mystl::hashtable<int, int, AlwaysCollideHash,
                    mystl::identity<int>, mystl::equal_to<int>> ht;

    // 插入多个元素（它们会在同一个桶中）
    for (int i = 0; i < 5; ++i)
    {
        ht.insert_unique(i);
    }

    EXPECT_EQ(ht.size(), 5);
    
    // 验证所有元素都能找到
    for (int i = 0; i < 5; ++i)
    {
        auto it = ht.find(i);
        EXPECT_NE(it, ht.end());
        EXPECT_EQ(*it, i);
    }
} 