#include <gtest/gtest.h>
#include "mystl/rb_tree.hpp"
#include "mystl/functional.hpp"
#include "throw_on_copy.hpp"

// 基本功能测试
TEST(RBTreeTest, BasicOperations) 
{
    mystl::rb_tree<int, mystl::less<int>> tree;
    
    // 空树测试
    EXPECT_TRUE(tree.empty());
    EXPECT_EQ(tree.size(), 0);
    
    // 插入测试
    auto p1 = tree.insert_unique(10);
    EXPECT_TRUE(p1.second);
    EXPECT_EQ(*p1.first, 10);
    EXPECT_EQ(tree.size(), 1);
    
    // 重复插入测试
    auto p2 = tree.insert_unique(10);
    EXPECT_FALSE(p2.second);
    EXPECT_EQ(tree.size(), 1);
    
    // 多个元素插入
    tree.insert_unique(5);
    tree.insert_unique(15);
    tree.insert_unique(3);
    tree.insert_unique(7);
    EXPECT_EQ(tree.size(), 5);
    
    // 验证有序性
    auto it = tree.begin();
    EXPECT_EQ(*it++, 3);
    EXPECT_EQ(*it++, 5);
    EXPECT_EQ(*it++, 7);
    EXPECT_EQ(*it++, 10);
    EXPECT_EQ(*it++, 15);
    EXPECT_EQ(it, tree.end());
}

// 查找操作测试
TEST(RBTreeTest, SearchOperations) 
{
    mystl::rb_tree<int, mystl::less<int>> tree;
    tree.insert_unique(10);
    tree.insert_unique(5);
    tree.insert_unique(15);
    tree.insert_unique(3);
    tree.insert_unique(7);
    
    // find 测试
    auto it = tree.find(7);
    EXPECT_NE(it, tree.end());
    EXPECT_EQ(*it, 7);
    
    it = tree.find(100);
    EXPECT_EQ(it, tree.end());
    
    // lower_bound 测试
    it = tree.lower_bound(6);
    EXPECT_EQ(*it, 7);
    
    // upper_bound 测试
    it = tree.upper_bound(7);
    EXPECT_EQ(*it, 10);
    
    // equal_range 测试
    auto range = tree.equal_range(7);
    EXPECT_EQ(*range.first, 7);
    EXPECT_EQ(*range.second, 10);
}

// 删除操作测试
TEST(RBTreeTest, EraseOperations) 
{
    mystl::rb_tree<int, mystl::less<int>> tree;
    tree.insert_unique(10);
    tree.insert_unique(5);
    tree.insert_unique(15);
    tree.insert_unique(3);
    tree.insert_unique(7);
    
    // 删除叶子节点
    tree.erase(tree.find(3));
    EXPECT_EQ(tree.size(), 4);
    EXPECT_EQ(tree.find(3), tree.end());
    
    // 删除有一个子节点的节点
    tree.erase(tree.find(5));
    EXPECT_EQ(tree.size(), 3);
    EXPECT_EQ(tree.find(5), tree.end());
    
    // 删除有两个子节点的节点
    tree.erase(tree.find(10));
    EXPECT_EQ(tree.size(), 2);
    EXPECT_EQ(tree.find(10), tree.end());
    
    // 验证剩余节点
    auto it = tree.begin();
    EXPECT_EQ(*it++, 7);
    EXPECT_EQ(*it++, 15);
    EXPECT_EQ(it, tree.end());
}

// 异常安全性测试
TEST(RBTreeTest, ExceptionSafety) 
{
    mystl::rb_tree<ThrowOnCopy, mystl::less<ThrowOnCopy>> tree;
    
    // 插入测试
    ThrowOnCopy::reset();
    tree.insert_unique(ThrowOnCopy(1));
    tree.insert_unique(ThrowOnCopy(2));
    
    size_t old_size = tree.size();
    ThrowOnCopy::should_throw = true;
    
    try 
    {
        tree.insert_unique(ThrowOnCopy(3));
        FAIL() << "Expected std::runtime_error";
    }
    catch (const std::runtime_error&) 
    {
        // 验证树的状态没有改变
        EXPECT_EQ(tree.size(), old_size);
        auto it = tree.begin();
        EXPECT_EQ(it->value, 1);
        ++it;
        EXPECT_EQ(it->value, 2);
    }
}

// 性能测试
TEST(RBTreeTest, Performance) 
{
    mystl::rb_tree<int, mystl::less<int>> tree;
    const int N = 10000;
    
    // 插入性能
    for (int i = 0; i < N; ++i) 
    {
        tree.insert_unique(i);
    }
    EXPECT_EQ(tree.size(), N);
    
    // 查找性能
    for (int i = 0; i < N; ++i) 
    {
        auto it = tree.find(i);
        EXPECT_NE(it, tree.end());
        EXPECT_EQ(*it, i);
    }
    
    // 删除性能
    for (int i = 0; i < N; ++i) 
    {
        auto it = tree.find(i);
        tree.erase(it);
    }
    EXPECT_TRUE(tree.empty());
}

// 红黑树性质验证
TEST(RBTreeTest, RBProperties) 
{
    mystl::rb_tree<int, mystl::less<int>> tree;
    
    // 插入一些数据
    tree.insert_unique(10);
    tree.insert_unique(5);
    tree.insert_unique(15);
    tree.insert_unique(3);
    tree.insert_unique(7);
    
    // 验证根节点为黑色
    EXPECT_EQ(tree.get_root()->color, mystl::rb_tree_black);
    
    // 验证红节点的子节点为黑色
    // 注意：这需要访问内部节点，可能需要添加辅助函数
} 