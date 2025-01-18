#include <gtest/gtest.h>
#include "mystl/deque.hpp"
#include "throw_on_copy.hpp"
#include "mystl/vector.hpp"
#include <stdexcept>

// 构造函数测试
TEST(DequeTest, Constructor) 
{
    // 默认构造函数
    mystl::deque<int> dq1;
    EXPECT_TRUE(dq1.empty());
    EXPECT_EQ(dq1.size(), 0);

    // 大小构造函数
    mystl::deque<int> dq2(3);
    EXPECT_EQ(dq2.size(), 3);
    for(const auto& x : dq2) 
    {
        EXPECT_EQ(x, 0);
    }

    // 填充构造函数
    mystl::deque<int> dq3(3, 5);
    EXPECT_EQ(dq3.size(), 3);
    for(const auto& x : dq3) 
    {
        EXPECT_EQ(x, 5);
    }

    // 范围构造函数
    int arr[] = {1, 2, 3};
    mystl::deque<int> dq4(arr, arr + 3);
    EXPECT_EQ(dq4.size(), 3);
    EXPECT_EQ(dq4[0], 1);
    EXPECT_EQ(dq4[1], 2);
    EXPECT_EQ(dq4[2], 3);

    // 初始化列表构造函数
    mystl::deque<int> dq5{1, 2, 3};
    EXPECT_EQ(dq5.size(), 3);
    EXPECT_EQ(dq5[0], 1);
    EXPECT_EQ(dq5[1], 2);
    EXPECT_EQ(dq5[2], 3);

    // 拷贝构造函数
    mystl::deque<int> dq6(dq5);
    EXPECT_EQ(dq6, dq5);

    // 移动构造函数
    mystl::deque<int> dq7(mystl::move(dq6));
    EXPECT_TRUE(dq6.empty());
    EXPECT_EQ(dq7, dq5);
}

// 赋值操作测试
TEST(DequeTest, Assignment) 
{
    mystl::deque<int> dq1{1, 2, 3};
    
    // 拷贝赋值
    mystl::deque<int> dq2;
    dq2 = dq1;
    EXPECT_EQ(dq2, dq1);

    // 移动赋值
    mystl::deque<int> dq3;
    dq3 = mystl::move(dq2);
    EXPECT_TRUE(dq2.empty());
    EXPECT_EQ(dq3, dq1);

    // assign - 填充
    dq3.assign(3, 5);
    EXPECT_EQ(dq3.size(), 3);
    for(const auto& x : dq3) 
    {
        EXPECT_EQ(x, 5);
    }

    // assign - 范围
    dq3.assign(dq1.begin(), dq1.end());
    EXPECT_EQ(dq3, dq1);

    // assign - 初始化列表
    dq3.assign({4, 5, 6});
    mystl::deque<int> dq4{4, 5, 6};
    EXPECT_EQ(dq3, dq4);
}

// 元素访问测试
TEST(DequeTest, ElementAccess) 
{
    mystl::deque<int> dq{1, 2, 3};
    const mystl::deque<int>& cdq = dq;
    
    // operator[]
    EXPECT_EQ(dq[0], 1);
    EXPECT_EQ(dq[1], 2);
    EXPECT_EQ(dq[2], 3);
    
    // at()
    EXPECT_EQ(dq.at(0), 1);
    EXPECT_EQ(dq.at(1), 2);
    EXPECT_EQ(dq.at(2), 3);
    EXPECT_THROW(dq.at(3), std::out_of_range);
    
    // front()
    EXPECT_EQ(dq.front(), 1);
    EXPECT_EQ(cdq.front(), 1);

    // back()
    EXPECT_EQ(dq.back(), 3);
    EXPECT_EQ(cdq.back(), 3);
}

// 迭代器测试
TEST(DequeTest, Iterator) 
{
    mystl::deque<int> dq{1, 2, 3};
    const mystl::deque<int>& cdq = dq;
    
    // 正向迭代
    auto it = dq.begin();
    EXPECT_EQ(*it++, 1);
    EXPECT_EQ(*it++, 2);
    EXPECT_EQ(*it++, 3);
    EXPECT_EQ(it, dq.end());

    // const迭代器
    auto cit = cdq.begin();
    EXPECT_EQ(*cit++, 1);
    EXPECT_EQ(*cit++, 2);
    EXPECT_EQ(*cit++, 3);
    EXPECT_EQ(cit, cdq.end());

    // 反向迭代
    auto rit = dq.rbegin();
    EXPECT_EQ(*rit++, 3);
    EXPECT_EQ(*rit++, 2);
    EXPECT_EQ(*rit++, 1);
    EXPECT_EQ(rit, dq.rend());

    // const反向迭代器
    auto crit = cdq.rbegin();
    EXPECT_EQ(*crit++, 3);
    EXPECT_EQ(*crit++, 2);
    EXPECT_EQ(*crit++, 1);
    EXPECT_EQ(crit, cdq.rend());
}

// 修改器测试
TEST(DequeTest, Modifiers) 
{
    mystl::deque<int> dq;
    
    // push_back
    dq.push_back(2);
    mystl::deque<int> expected1{2};
    EXPECT_EQ(dq, expected1);
    
    // push_front
    dq.push_front(1);
    mystl::deque<int> expected2{1, 2};
    EXPECT_EQ(dq, expected2);
    
    // push_back again
    dq.push_back(3);
    mystl::deque<int> expected3{1, 2, 3};
    EXPECT_EQ(dq, expected3);
    
    // insert 单个元素
    auto it = dq.begin();
    ++it;  // 指向2
    it = dq.insert(it, 4);
    EXPECT_EQ(*it, 4);
    mystl::deque<int> expected4{1, 4, 2, 3};
    EXPECT_EQ(dq, expected4);
    
    // insert 多个相同元素
    it = dq.begin();
    ++it;  // 指向4
    it = dq.insert(it, 2, 5);
    EXPECT_EQ(*it, 5);
    mystl::deque<int> expected5{1, 5, 5, 4, 2, 3};
    EXPECT_EQ(dq, expected5);
    
    // insert 范围元素
    mystl::vector<int> v{7, 8, 9};
    it = dq.begin();
    ++it;  // 指向5
    it = dq.insert(it, v.begin(), v.end());
    EXPECT_EQ(*it, 7);
    mystl::deque<int> expected6{1, 7, 8, 9, 5, 5, 4, 2, 3};
    EXPECT_EQ(dq, expected6);
    
    // insert 初始化列表
    it = dq.begin();
    ++it;  // 指向7
    it = dq.insert(it, {10, 11});
    EXPECT_EQ(*it, 10);
    mystl::deque<int> expected7{1, 10, 11, 7, 8, 9, 5, 5, 4, 2, 3};
    EXPECT_EQ(dq, expected7);
    
    // insert 在开头
    it = dq.insert(dq.begin(), 0);
    EXPECT_EQ(*it, 0);
    mystl::deque<int> expected8{0, 1, 10, 11, 7, 8, 9, 5, 5, 4, 2, 3};
    EXPECT_EQ(dq, expected8);
    
    // insert 在末尾
    it = dq.insert(dq.end(), 12);
    EXPECT_EQ(*it, 12);
    mystl::deque<int> expected9{0, 1, 10, 11, 7, 8, 9, 5, 5, 4, 2, 3, 12};
    EXPECT_EQ(dq, expected9);

    // erase
    it = dq.begin();
    ++it;  // 指向1
    it = dq.erase(it);
    EXPECT_EQ(*it, 10);
    mystl::deque<int> expected10{0, 10, 11, 7, 8, 9, 5, 5, 4, 2, 3, 12};
    EXPECT_EQ(dq, expected10);

    // erase范围
    it = dq.begin();
    it++;
    it=dq.erase(it,it+3);
    EXPECT_EQ(*it, 8);
    mystl::deque<int> expected11{0, 8, 9, 5, 5, 4, 2, 3, 12};
    EXPECT_EQ(dq, expected11);  
    
    // pop_back
    dq.pop_back();
    mystl::deque<int> expected12{0, 8, 9, 5, 5, 4, 2, 3};
    EXPECT_EQ(dq, expected12);
    
    // pop_front
    dq.pop_front();
    mystl::deque<int> expected13{8, 9, 5, 5, 4, 2, 3};
    EXPECT_EQ(dq, expected13);
    
    // clear
    dq.clear();
    mystl::deque<int> expected15;
    EXPECT_EQ(dq, expected15);
    EXPECT_TRUE(dq.empty());
}

// 异常安全性测试
TEST(DequeTest, ExceptionSafety) 
{
    // emplace 的异常安全性
    {
        mystl::deque<ThrowOnCopy> dq;
        dq.push_back(ThrowOnCopy(1));
        dq.push_back(ThrowOnCopy(2));
        EXPECT_EQ(dq.size(), 2);

        ThrowOnCopy value(3);  // 先创建value
        size_t old_size = dq.size();
        
        try 
        {
            ThrowOnCopy::reset();
            ThrowOnCopy::should_throw = true;  // 在emplace之前设置
            dq.emplace(dq.begin(), value);  // 这里会触发拷贝构造
            FAIL() << "Expected std::runtime_error";
        }
        catch (const std::runtime_error&) 
        {
            EXPECT_EQ(dq.size(), old_size);
            auto it = dq.begin();
            EXPECT_EQ(it->value, 1);
            ++it;
            EXPECT_EQ(it->value, 2);
        }
    }

    // 单个元素插入的异常安全性
    {
        mystl::deque<ThrowOnCopy> dq;
        dq.push_back(ThrowOnCopy(1));
        dq.push_back(ThrowOnCopy(3));
        EXPECT_EQ(dq.size(), 2);

        ThrowOnCopy value(2);  // 先创建value
        size_t old_size = dq.size();
        
        try 
        {
            ThrowOnCopy::reset();
            ThrowOnCopy::should_throw = true;  // 在insert之前设置
            dq.insert(dq.begin() + 1, value);
            FAIL() << "Expected std::runtime_error";
        }
        catch (const std::runtime_error&) 
        {
            // 验证容器状态保持不变
            EXPECT_EQ(dq.size(), old_size);
            EXPECT_EQ(dq.front().value, 1);
            EXPECT_EQ(dq.back().value, 3);
        }
    }

    // 多个相同元素插入的异常安全性
    {
        mystl::deque<ThrowOnCopy> dq;
        dq.push_back(ThrowOnCopy(1));
        dq.push_back(ThrowOnCopy(4));
        EXPECT_EQ(dq.size(), 2);

        ThrowOnCopy value(2);  // 先创建value
        size_t old_size = dq.size();
        
        try 
        {
            ThrowOnCopy::reset();
            ThrowOnCopy::should_throw = true;  // 在insert之前设置
            dq.insert(dq.begin() + 1, 3, value);
            FAIL() << "Expected std::runtime_error";
        }
        catch (const std::runtime_error&) 
        {
            EXPECT_EQ(dq.size(), old_size);
            EXPECT_EQ(dq.front().value, 1);
            EXPECT_EQ(dq.back().value, 4);
        }
    }

    // 范围插入的异常安全性
    {
        mystl::deque<ThrowOnCopy> dq;
        dq.push_back(ThrowOnCopy(1));
        dq.push_back(ThrowOnCopy(4));
        EXPECT_EQ(dq.size(), 2);

        mystl::deque<ThrowOnCopy> other;  // 先创建源数据
        other.push_back(ThrowOnCopy(2));
        other.push_back(ThrowOnCopy(3));
        size_t old_size = dq.size();
        
        try 
        {
            ThrowOnCopy::reset();
            ThrowOnCopy::should_throw = true;  // 在insert之前设置
            dq.insert(dq.begin() + 1, other.begin(), other.end());
            FAIL() << "Expected std::runtime_error";
        }
        catch (const std::runtime_error&) 
        {
            EXPECT_EQ(dq.size(), old_size);
            EXPECT_EQ(dq.front().value, 1);
            EXPECT_EQ(dq.back().value, 4);
        }
    }

    // 初始化列表插入的异常安全性
    {
        mystl::deque<ThrowOnCopy> dq;
        dq.push_back(ThrowOnCopy(1));
        dq.push_back(ThrowOnCopy(4));
        EXPECT_EQ(dq.size(), 2);

        size_t old_size = dq.size();
        
        try 
        {
            ThrowOnCopy::reset();
            ThrowOnCopy::should_throw = true;  // 在insert之前设置
            dq.insert(dq.begin() + 1, {ThrowOnCopy(2), ThrowOnCopy(3)});
            FAIL() << "Expected std::runtime_error";
        }
        catch (const std::runtime_error&) 
        {
            // 验证容器状态保持不变
            EXPECT_EQ(dq.size(), old_size);
            EXPECT_EQ(dq.front().value, 1);
            EXPECT_EQ(dq.back().value, 4);
        }
    }

    // 在不同位置插入时的异常安全性
    {
        mystl::deque<ThrowOnCopy> dq;
        dq.push_back(ThrowOnCopy(1));
        dq.push_back(ThrowOnCopy(2));
        dq.push_back(ThrowOnCopy(3));
        EXPECT_EQ(dq.size(), 3);

        ThrowOnCopy value(4);
        size_t old_size = dq.size();

        // 在开头插入
        try 
        {
            ThrowOnCopy::reset();
            ThrowOnCopy::should_throw = true;  // 在insert之前设置
            dq.insert(dq.begin(), value);
            FAIL() << "Expected std::runtime_error";
        }
        catch (const std::runtime_error&) 
        {
            EXPECT_EQ(dq.size(), old_size);
            auto it = dq.begin();
            EXPECT_EQ(it->value, 1);
            ++it;
            EXPECT_EQ(it->value, 2);
            ++it;
            EXPECT_EQ(it->value, 3);
        }

        // 在中间插入
        try 
        {
            ThrowOnCopy::reset();
            ThrowOnCopy::should_throw = true;  // 在insert之前设置
            dq.insert(dq.begin() + 1, value);
            FAIL() << "Expected std::runtime_error";
        }
        catch (const std::runtime_error&) 
        {
            EXPECT_EQ(dq.size(), old_size);
            auto it = dq.begin();
            EXPECT_EQ(it->value, 1);
            ++it;
            EXPECT_EQ(it->value, 2);
            ++it;
            EXPECT_EQ(it->value, 3);
        }

        // 在末尾插入
        try 
        {
            ThrowOnCopy::reset();
            ThrowOnCopy::should_throw = true;  // 在insert之前设置
            dq.insert(dq.end(), value);
            FAIL() << "Expected std::runtime_error";
        }
        catch (const std::runtime_error&) 
        {
            EXPECT_EQ(dq.size(), old_size);
            auto it = dq.begin();
            EXPECT_EQ(it->value, 1);
            ++it;
            EXPECT_EQ(it->value, 2);
            ++it;
            EXPECT_EQ(it->value, 3);
        }
    }

    // resize 的异常安全性测试
    {
        mystl::deque<ThrowOnCopy> dq;
        dq.push_back(ThrowOnCopy(1));
        dq.push_back(ThrowOnCopy(2));
        EXPECT_EQ(dq.size(), 2);

        // 测试缩小大小（不应该抛出异常）
        dq.resize(1);
        EXPECT_EQ(dq.size(), 1);
        EXPECT_EQ(dq.front().value, 1);

        ThrowOnCopy value(3);  // 先创建value
        size_t old_size = dq.size();
        
        try 
        {
            ThrowOnCopy::reset();
            ThrowOnCopy::should_throw = true;  // 在resize之前设置
            dq.resize(3, value);
            FAIL() << "Expected std::runtime_error";
        }
        catch (const std::runtime_error&) 
        {
            EXPECT_EQ(dq.size(), old_size);
            EXPECT_EQ(dq.front().value, 1);
        }

        // 测试默认构造扩大大小（不应该抛出异常）
        ThrowOnCopy::reset();
        dq.resize(3);
        EXPECT_EQ(dq.size(), 3);
        auto it = dq.begin();
        EXPECT_EQ(it->value, 1);
        ++it;
        EXPECT_EQ(it->value, 0);
        ++it;
        EXPECT_EQ(it->value, 0);
    }
}

// 边界情况测试
TEST(DequeTest, EdgeCases) 
{
    mystl::deque<int> dq;
    
    // 空deque操作
    EXPECT_TRUE(dq.empty());
    EXPECT_EQ(dq.size(), 0);
    EXPECT_EQ(dq.begin(), dq.end());
    
    // 单元素操作
    dq.push_back(1);
    EXPECT_EQ(dq.front(), dq.back());
    EXPECT_EQ(dq.size(), 1);
    
    // 大量元素操作
    for(int i = 0; i < 1000; ++i) 
    {
        dq.push_back(i);
    }
    EXPECT_EQ(dq.size(), 1001);
    
    // 清空后再操作
    dq.clear();
    EXPECT_TRUE(dq.empty());
    dq.push_back(1);
    EXPECT_EQ(dq.size(), 1);
    EXPECT_EQ(dq.front(), 1);
}

