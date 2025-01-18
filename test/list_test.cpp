#include <gtest/gtest.h>
#include "mystl/list.hpp"
#include "throw_on_copy.hpp"
#include <stdexcept>

// 构造函数测试
TEST(ListTest, Constructor) 
{
    // 默认构造函数
    mystl::list<int> lst1;
    EXPECT_TRUE(lst1.empty());
    EXPECT_EQ(lst1.size(), 0);

    // 大小构造函数
    mystl::list<int> lst2(3);
    EXPECT_EQ(lst2.size(), 3);
    for(const auto& x : lst2) 
    {
        EXPECT_EQ(x, 0);
    }

    // 填充构造函数
    mystl::list<int> lst3(3, 5);
    EXPECT_EQ(lst3.size(), 3);
    for(const auto& x : lst3) 
    {
        EXPECT_EQ(x, 5);
    }

    // 范围构造函数
    int arr[] = {1, 2, 3};
    mystl::list<int> lst4(arr, arr + 3);
    EXPECT_EQ(lst4.size(), 3);
    auto it = lst4.begin();
    EXPECT_EQ(*it++, 1);
    EXPECT_EQ(*it++, 2);
    EXPECT_EQ(*it++, 3);

    // 初始化列表构造函数
    mystl::list<int> lst5{1, 2, 3};
    EXPECT_EQ(lst5.size(), 3);
    it = lst5.begin();
    EXPECT_EQ(*it++, 1);
    EXPECT_EQ(*it++, 2);
    EXPECT_EQ(*it++, 3);

    // 拷贝构造函数
    mystl::list<int> lst6(lst5);
    EXPECT_EQ(lst6, lst5);

    // 移动构造函数
    mystl::list<int> lst7(mystl::move(lst6));
    EXPECT_TRUE(lst6.empty());
    EXPECT_EQ(lst7, lst5);
}

// 赋值操作测试
TEST(ListTest, Assignment) 
{
    mystl::list<int> lst1{1, 2, 3};
    
    // 拷贝赋值
    mystl::list<int> lst2;
    lst2 = lst1;
    EXPECT_EQ(lst2, lst1);

    // 移动赋值
    mystl::list<int> lst3;
    lst3 = mystl::move(lst2);
    EXPECT_TRUE(lst2.empty());
    EXPECT_EQ(lst3, lst1);

    // assign - 填充
    lst3.assign(3, 5);
    EXPECT_EQ(lst3.size(), 3);
    for(const auto& x : lst3) 
    {
        EXPECT_EQ(x, 5);
    }

    // assign - 范围
    lst3.assign(lst1.begin(), lst1.end());
    EXPECT_EQ(lst3, lst1);

    // assign - 初始化列表
    lst3.assign({4, 5, 6});
    mystl::list<int> lst4{4, 5, 6};
    EXPECT_EQ(lst3, lst4);
}

// 元素访问测试
TEST(ListTest, ElementAccess) 
{
    mystl::list<int> lst{1, 2, 3};
    const mystl::list<int>& clst = lst;
    
    // front()
    EXPECT_EQ(lst.front(), 1);
    EXPECT_EQ(clst.front(), 1);

    // back()
    EXPECT_EQ(lst.back(), 3);
    EXPECT_EQ(clst.back(), 3);
}

// 迭代器测试
TEST(ListTest, Iterator) 
{
    mystl::list<int> lst{1, 2, 3};
    const mystl::list<int>& clst = lst;
    
    // 正向迭代
    auto it = lst.begin();
    EXPECT_EQ(*it++, 1);
    EXPECT_EQ(*it++, 2);
    EXPECT_EQ(*it++, 3);
    EXPECT_EQ(it, lst.end());

    // const迭代器
    auto cit = clst.begin();
    EXPECT_EQ(*cit++, 1);
    EXPECT_EQ(*cit++, 2);
    EXPECT_EQ(*cit++, 3);
    EXPECT_EQ(cit, clst.end());

    // 反向迭代
    auto rit = lst.rbegin();
    EXPECT_EQ(*rit++, 3);
    EXPECT_EQ(*rit++, 2);
    EXPECT_EQ(*rit++, 1);
    EXPECT_EQ(rit, lst.rend());

    // const反向迭代器
    auto crit = clst.rbegin();
    EXPECT_EQ(*crit++, 3);
    EXPECT_EQ(*crit++, 2);
    EXPECT_EQ(*crit++, 1);
    EXPECT_EQ(crit, clst.rend());
}

// 修改器测试
TEST(ListTest, Modifiers) 
{
    mystl::list<int> lst;

    // push_back/push_front
    lst.push_back(2);
    lst.push_front(1);
    lst.push_back(3);
    EXPECT_EQ(lst.size(), 3);
    auto it = lst.begin();
    EXPECT_EQ(*it++, 1);
    EXPECT_EQ(*it++, 2);
    EXPECT_EQ(*it++, 3);

    // pop_back/pop_front
    lst.pop_back();
    lst.pop_front();
    EXPECT_EQ(lst.size(), 1);
    EXPECT_EQ(lst.front(), 2);

    // insert
    it = lst.begin();
    auto it2 = lst.insert(it, 1);
    EXPECT_EQ(*it2, 1);
    lst.insert(lst.end(), 3);
    EXPECT_EQ(lst.size(), 3);
    
    // insert多个元素
    lst.insert(lst.begin(), 2, 0);
    EXPECT_EQ(lst.size(), 5);
    mystl::list<int> expected1{0, 0, 1, 2, 3};
    EXPECT_EQ(lst, expected1);

    // insert范围
    int arr[] = {4, 5};
    lst.insert(lst.end(), arr, arr + 2);
    mystl::list<int> expected2{0, 0, 1, 2, 3, 4, 5};
    EXPECT_EQ(lst, expected2);

    // erase
    it = lst.begin();
    ++it; // 指向第二个0
    it = lst.erase(it); // 删除第二个0
    EXPECT_EQ(*it, 1);
    
    // erase范围
    it = lst.begin();
    ++it; // 指向1
    auto it3 = it;
    mystl::advance(it3, 2); // 指向3
    it = lst.erase(it, it3); // 删除[1,2]
    EXPECT_EQ(*it, 3);
    mystl::list<int> expected3{0, 3, 4, 5};
    EXPECT_EQ(lst, expected3);

    // clear
    lst.clear();
    EXPECT_TRUE(lst.empty());
    EXPECT_EQ(lst.size(), 0);
}

// 链表操作测试
TEST(ListTest, Operations) 
{
    // splice 测试
    {
        // 整个链表的 splice
        {
            mystl::list<int> lst1{1, 2, 3};
            mystl::list<int> lst2{4, 5, 6};
            lst1.splice(lst1.end(), lst2);
            EXPECT_TRUE(lst2.empty());
            mystl::list<int> expected1{1, 2, 3, 4, 5, 6};
            EXPECT_EQ(lst1, expected1);
        }

        // 单个节点的 splice
        {
            mystl::list<int> lst1{1, 2, 3};
            mystl::list<int> lst2{4, 5, 6};
            lst1.splice(lst1.begin(), lst2, lst2.begin());
            mystl::list<int> expected1{4, 1, 2, 3};
            mystl::list<int> expected2{5, 6};
            EXPECT_EQ(lst1, expected1);
            EXPECT_EQ(lst2, expected2);
        }

        // 范围的 splice
        {
            mystl::list<int> lst1{1, 2, 3};
            mystl::list<int> lst2{4, 5, 6, 7, 8};
            auto first = ++lst2.begin();  // 指向5
            auto last = --lst2.end();     // 指向8
            lst1.splice(++lst1.begin(), lst2, first, last);
            mystl::list<int> expected1{1, 5, 6, 7, 2, 3};
            mystl::list<int> expected2{4, 8};
            EXPECT_EQ(lst1, expected1);
            EXPECT_EQ(lst2, expected2);
        }
    }

    // remove/remove_if 测试
    {
        mystl::list<int> lst{1, 2, 3, 2, 4, 2, 5};
        lst.remove(2);
        mystl::list<int> expected1{1, 3, 4, 5};
        EXPECT_EQ(lst, expected1);

        lst.remove_if([](int n) { return n > 3; });
        mystl::list<int> expected2{1, 3};
        EXPECT_EQ(lst, expected2);
    }

    // unique 测试
    {
        mystl::list<int> lst{1, 1, 2, 2, 2, 3, 3, 2, 2, 1};
        lst.unique();
        mystl::list<int> expected{1, 2, 3, 2, 1};
        EXPECT_EQ(lst, expected);
    }

    // merge 测试
    {
        mystl::list<int> lst1{1, 3, 5, 7};
        mystl::list<int> lst2{2, 4, 6, 8};
        lst1.merge(lst2);
        EXPECT_TRUE(lst2.empty());
        mystl::list<int> expected{1, 2, 3, 4, 5, 6, 7, 8};
        EXPECT_EQ(lst1, expected);
    }

    // sort 测试
    {
        mystl::list<int> lst{4, 1, 3, 2, 6, 5, 8, 7};
        lst.sort();
        mystl::list<int> expected{1, 2, 3, 4, 5, 6, 7, 8};
        EXPECT_EQ(lst, expected);

        // 自定义比较器
        lst.sort(std::greater<int>());
        mystl::list<int> expected2{8, 7, 6, 5, 4, 3, 2, 1};
        EXPECT_EQ(lst, expected2);
    }

    // reverse 测试
    {
        mystl::list<int> lst{1, 2, 3, 4, 5};
        lst.reverse();
        mystl::list<int> expected{5, 4, 3, 2, 1};
        EXPECT_EQ(lst, expected);
    }
}

// 异常安全性测试
TEST(ListTest, ExceptionSafety) 
{
    // emplace 的异常安全性
    {
        mystl::list<ThrowOnCopy> lst;
        lst.push_back(ThrowOnCopy(1));
        lst.push_back(ThrowOnCopy(2));
        EXPECT_EQ(lst.size(), 2);

        ThrowOnCopy::reset();
        ThrowOnCopy::should_throw = true;
        size_t old_size = lst.size();
        
        try 
        {
            // 尝试在开头emplace一个新元素，通过拷贝构造触发异常
            ThrowOnCopy value(3);
            lst.emplace(lst.begin(), value);  // 这里会触发拷贝构造
            FAIL() << "Expected std::runtime_error";
        }
        catch (const std::runtime_error&) 
        {
            // 验证链表状态保持不变
            EXPECT_EQ(lst.size(), old_size);
            auto it = lst.begin();
            EXPECT_EQ(it->value, 1);
            ++it;
            EXPECT_EQ(it->value, 2);
        }

        // 测试直接构造的情况（不应该抛出异常）
        ThrowOnCopy::reset();
        lst.emplace(lst.begin(), 3);  // 直接构造，不会触发拷贝
        EXPECT_EQ(lst.size(), 3);
        EXPECT_EQ(lst.front().value, 3);
    }

    // insert 单个元素的异常安全性
    {
        mystl::list<ThrowOnCopy> lst;
        lst.push_back(ThrowOnCopy(1));
        lst.push_back(ThrowOnCopy(2));
        
        ThrowOnCopy::reset();
        ThrowOnCopy value(3);
        ThrowOnCopy::should_throw = true;
        
        try 
        {
            lst.insert(lst.begin(), value);
            FAIL() << "Expected std::runtime_error";
        }
        catch (const std::runtime_error&) 
        {
            EXPECT_EQ(lst.size(), 2);
            auto it = lst.begin();
            EXPECT_EQ(it->value, 1);
            ++it;
            EXPECT_EQ(it->value, 2);
        }
    }

    // insert 多个相同元素的异常安全性
    {
        mystl::list<ThrowOnCopy> lst;
        lst.push_back(ThrowOnCopy(1));
        lst.push_back(ThrowOnCopy(2));
        EXPECT_EQ(lst.size(), 2);

        ThrowOnCopy::reset();
        ThrowOnCopy value(3);
        ThrowOnCopy::should_throw = true;
        size_t old_size = lst.size();
        
        try 
        {
            // 尝试在开头插入3个相同的元素
            lst.insert(lst.begin(), 3, value);
            FAIL() << "Expected std::runtime_error";
        }
        catch (const std::runtime_error&) 
        {
            // 验证链表状态保持不变
            EXPECT_EQ(lst.size(), old_size);
            auto it = lst.begin();
            EXPECT_EQ(it->value, 1);
            ++it;
            EXPECT_EQ(it->value, 2);
        }
    }

    // insert 范围的异常安全性
    {
        mystl::list<ThrowOnCopy> lst;
        lst.push_back(ThrowOnCopy(1));
        
        mystl::list<ThrowOnCopy> other;
        other.push_back(ThrowOnCopy(2));
        other.push_back(ThrowOnCopy(3));
        
        ThrowOnCopy::reset();
        ThrowOnCopy::should_throw = true;
        
        try 
        {
            lst.insert(lst.begin(), other.begin(), other.end());
            FAIL() << "Expected std::runtime_error";
        }
        catch (const std::runtime_error&) 
        {
            EXPECT_EQ(lst.size(), 1);
            EXPECT_EQ(lst.front().value, 1);
        }
    }

    // insert 初始化列表的异常安全性
    {
        mystl::list<ThrowOnCopy> lst;
        lst.push_back(ThrowOnCopy(1));
        
        ThrowOnCopy::reset();
        ThrowOnCopy::should_throw = true;
        
        try 
        {
            lst.insert(lst.begin(), {ThrowOnCopy(2), ThrowOnCopy(3)});
            FAIL() << "Expected std::runtime_error";
        }
        catch (const std::runtime_error&) 
        {
            EXPECT_EQ(lst.size(), 1);
            EXPECT_EQ(lst.front().value, 1);
        }
    }

    // resize 的异常安全性
    {
        mystl::list<ThrowOnCopy> lst;
        lst.push_back(ThrowOnCopy(1));
        lst.push_back(ThrowOnCopy(2));
        EXPECT_EQ(lst.size(), 2);

        // 测试缩小大小（不应该抛出异常）
        lst.resize(1);
        EXPECT_EQ(lst.size(), 1);
        EXPECT_EQ(lst.front().value, 1);

        // 测试使用值扩大大小时的异常
        ThrowOnCopy::reset();
        ThrowOnCopy value(3);
        ThrowOnCopy::should_throw = true;
        size_t old_size = lst.size();
        
        try 
        {
            lst.resize(3, value);  // 尝试用value扩大到3个元素
            FAIL() << "Expected std::runtime_error";
        }
        catch (const std::runtime_error&) 
        {
            // 验证链表状态保持不变
            EXPECT_EQ(lst.size(), old_size);
            EXPECT_EQ(lst.front().value, 1);
        }

        // 测试默认构造扩大大小（不应该抛出异常）
        ThrowOnCopy::reset();
        lst.resize(3);  // 使用默认构造函数扩大到3个元素
        EXPECT_EQ(lst.size(), 3);
        auto it = lst.begin();
        EXPECT_EQ(it->value, 1);
        ++it;
        EXPECT_EQ(it->value, 0);  // 默认构造的值
        ++it;
        EXPECT_EQ(it->value, 0);
    }
}

// 边界情况测试
TEST(ListTest, EdgeCases) 
{
    mystl::list<int> lst;
    
    // 空链表操作
    EXPECT_TRUE(lst.empty());
    EXPECT_EQ(lst.size(), 0);
    EXPECT_EQ(lst.begin(), lst.end());
    
    // 单元素操作
    lst.push_back(1);
    EXPECT_EQ(lst.front(), lst.back());
    EXPECT_EQ(lst.size(), 1);
    
    // 大量元素操作
    for(int i = 0; i < 1000; ++i) 
    {
        lst.push_back(i);
    }
    EXPECT_EQ(lst.size(), 1001);
    
    // 清空后再操作
    lst.clear();
    EXPECT_TRUE(lst.empty());
    lst.push_back(1);
    EXPECT_EQ(lst.size(), 1);
    EXPECT_EQ(lst.front(), 1);
}

