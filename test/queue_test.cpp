#include <gtest/gtest.h>
#include "mystl/queue.hpp"
#include "throw_on_copy.hpp"

// 普通队列测试
TEST(QueueTest, Basic) 
{
    mystl::queue<int> q;
    
    // 基本操作
    EXPECT_TRUE(q.empty());
    EXPECT_EQ(q.size(), 0);
    
    // push和front/back
    q.push(1);
    EXPECT_EQ(q.front(), 1);
    EXPECT_EQ(q.back(), 1);
    
    q.push(2);
    EXPECT_EQ(q.front(), 1);
    EXPECT_EQ(q.back(), 2);
    
    // pop
    q.pop();
    EXPECT_EQ(q.front(), 2);
    EXPECT_EQ(q.size(), 1);
    
    q.pop();
    EXPECT_TRUE(q.empty());
}

// 优先队列测试
TEST(PriorityQueueTest, Basic) 
{
    mystl::priority_queue<int> pq;
    
    // 基本操作
    EXPECT_TRUE(pq.empty());
    EXPECT_EQ(pq.size(), 0);
    
    // push和top
    pq.push(1);
    EXPECT_EQ(pq.top(), 1);
    
    pq.push(3);
    EXPECT_EQ(pq.top(), 3);
    
    pq.push(2);
    EXPECT_EQ(pq.top(), 3);
    
    // pop
    pq.pop();
    EXPECT_EQ(pq.top(), 2);
    
    pq.pop();
    EXPECT_EQ(pq.top(), 1);
    
    pq.pop();
    EXPECT_TRUE(pq.empty());
}

// 异常安全性测试
TEST(QueueTest, ExceptionSafety) 
{
    // 普通队列的异常安全性
    {
        mystl::queue<ThrowOnCopy> q;
        ThrowOnCopy::reset();
        
        q.push(ThrowOnCopy(1));
        q.push(ThrowOnCopy(2));
        
        ThrowOnCopy value(3);
        size_t old_size = q.size();
        
        try 
        {
            ThrowOnCopy::should_throw = true;
            q.push(value);
            FAIL() << "Expected std::runtime_error";
        }
        catch (const std::runtime_error&) 
        {
            EXPECT_EQ(q.size(), old_size);
            EXPECT_EQ(q.front().value, 1);
            EXPECT_EQ(q.back().value, 2);
        }
    }
    
    // 优先队列的异常安全性
    {
        mystl::priority_queue<ThrowOnCopy> pq;
        ThrowOnCopy::reset();
        
        pq.push(ThrowOnCopy(1));
        pq.push(ThrowOnCopy(2));
        
        ThrowOnCopy value(3);
        size_t old_size = pq.size();
        
        try 
        {
            ThrowOnCopy::should_throw = true;
            pq.push(value);
            FAIL() << "Expected std::runtime_error";
        }
        catch (const std::runtime_error&) 
        {
            EXPECT_EQ(pq.size(), old_size);
            EXPECT_EQ(pq.top().value, 2);
        }
    }
} 