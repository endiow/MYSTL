#include <gtest/gtest.h>
#include "mystl/vector.hpp"
#include <memory>
#include <ostream>
#include <stdexcept>

// 构造函数测试
TEST(VectorTest, Constructor) 
{
    // 默认构造
    mystl::vector<int> v1;
    EXPECT_TRUE(v1.empty());
    EXPECT_EQ(v1.size(), 0);
    EXPECT_EQ(v1.capacity(), 0);

    // 大小构造
    mystl::vector<int> v2(3);
    EXPECT_EQ(v2.size(), 3);
    for(size_t i = 0; i < v2.size(); ++i) 
    {
        EXPECT_EQ(v2[i], 0);
    }

    // 填充构造
    mystl::vector<int> v3(3, 42);
    EXPECT_EQ(v3.size(), 3);
    for(size_t i = 0; i < v3.size(); ++i) 
    {
        EXPECT_EQ(v3[i], 42);
    }

    // 范围构造
    int arr[] = {1, 2, 3};
    mystl::vector<int> v4(arr, arr + 3);
    EXPECT_EQ(v4.size(), 3);
    for(size_t i = 0; i < v4.size(); ++i) 
    {
        EXPECT_EQ(v4[i], arr[i]);
    }

    // 初始化列表构造
    mystl::vector<int> v5{1, 2, 3};
    EXPECT_EQ(v5.size(), 3);
    for(size_t i = 0; i < v5.size(); ++i) 
    {
        EXPECT_EQ(v5[i], arr[i]);
    }

    // 拷贝构造
    mystl::vector<int> v6(v5);
    EXPECT_EQ(v6, v5);

    // 移动构造
    mystl::vector<int> v7(std::move(v6));
    EXPECT_TRUE(v6.empty());
    EXPECT_EQ(v7, v5);
}

// 赋值操作测试
TEST(VectorTest, Assignment) 
{
    mystl::vector<int> v1{1, 2, 3};
    
    // 拷贝赋值
    mystl::vector<int> v2;
    v2 = v1;
    EXPECT_EQ(v2, v1);

    // 移动赋值
    mystl::vector<int> v3;
    v3 = std::move(v2);
    EXPECT_TRUE(v2.empty());
    EXPECT_EQ(v3, v1);

    // 初始化列表赋值
    v3 = {4, 5, 6};
    mystl::vector<int> v4{4, 5, 6};
    EXPECT_EQ(v3, v4);

    // assign - 范围
    v3.assign(v1.begin(), v1.end());
    EXPECT_EQ(v3, v1);

    // assign - 填充
    v3.assign(3, 42);
    EXPECT_EQ(v3, mystl::vector<int>(3, 42));

    // assign - 初始化列表
    v3.assign({1, 2, 3});
    EXPECT_EQ(v3, v1);
}

// 元素访问测试
TEST(VectorTest, ElementAccess) 
{
    mystl::vector<int> v{1, 2, 3};
    const mystl::vector<int>& cv = v;
    
    // operator[]
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(cv[0], 1);

    // at()
    EXPECT_EQ(v.at(1), 2);
    EXPECT_EQ(cv.at(1), 2);
    EXPECT_THROW(v.at(3), std::out_of_range);

    // front()
    EXPECT_EQ(v.front(), 1);
    EXPECT_EQ(cv.front(), 1);

    // back()
    EXPECT_EQ(v.back(), 3);
    EXPECT_EQ(cv.back(), 3);

    // data()
    EXPECT_EQ(*v.data(), 1);
    EXPECT_EQ(*cv.data(), 1);
}

// 迭代器测试
TEST(VectorTest, Iterators) 
{
    mystl::vector<int> v{1, 2, 3};
    const mystl::vector<int>& cv = v;
    
    // begin/end
    EXPECT_EQ(*v.begin(), 1);
    EXPECT_EQ(*(v.end() - 1), 3);
    
    // cbegin/cend
    EXPECT_EQ(*v.cbegin(), 1);
    EXPECT_EQ(*(v.cend() - 1), 3);
    
    // const begin/end
    EXPECT_EQ(*cv.begin(), 1);
    EXPECT_EQ(*(cv.end() - 1), 3);
    
    // rbegin/rend
    EXPECT_EQ(*v.rbegin(), 3);
    EXPECT_EQ(*(v.rend() - 1), 1);
    
    // crbegin/crend
    EXPECT_EQ(*v.crbegin(), 3);
    EXPECT_EQ(*(v.crend() - 1), 1);
    
    // const rbegin/rend
    EXPECT_EQ(*cv.rbegin(), 3);
    EXPECT_EQ(*(cv.rend() - 1), 1);

    // 迭代器运算
    auto it = v.begin();
    EXPECT_EQ(*(it + 1), 2);
    EXPECT_EQ(*(1 + it), 2);
    EXPECT_EQ(*(it - 0), 1);
    EXPECT_EQ(v.end() - v.begin(), 3);
    ++it;
    EXPECT_EQ(*it, 2);
    --it;
    EXPECT_EQ(*it, 1);
    it += 2;
    EXPECT_EQ(*it, 3);
    it -= 1;
    EXPECT_EQ(*it, 2);
}

// 容量操作测试
TEST(VectorTest, Capacity) 
{
    mystl::vector<int> v;
    
    // empty/size/capacity
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.size(), 0);
    EXPECT_EQ(v.capacity(), 0);

    // reserve
    v.reserve(10);
    EXPECT_GE(v.capacity(), 10);
    size_t old_cap = v.capacity();

    // 自动扩容
    for(int i = 0; i < 20; ++i) 
    {
        v.push_back(i);
        if(v.size() > old_cap) 
        {
            EXPECT_GT(v.capacity(), old_cap);
            old_cap = v.capacity();
        }
    }

    // shrink_to_fit
    v.clear();
    v.shrink_to_fit();
    EXPECT_EQ(v.capacity(), 0);

    // resize
    v = {1, 2, 3};
    v.resize(5);
    EXPECT_EQ(v.size(), 5);
    for(size_t i = 3; i < v.size(); ++i) 
    {
        EXPECT_EQ(v[i], 0);
    }

    v.resize(2);
    EXPECT_EQ(v.size(), 2);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);

    v.resize(4, 42);
    EXPECT_EQ(v.size(), 4);
    EXPECT_EQ(v[2], 42);
    EXPECT_EQ(v[3], 42);
}

// 修改器测试
TEST(VectorTest, Modifiers) 
{
    mystl::vector<int> v{1, 2, 3};
    
    // clear
    v.clear();
    EXPECT_TRUE(v.empty());

    // insert - 单个元素
    v = {1, 2, 3};
    auto it = v.insert(v.begin() + 1, 42);
    EXPECT_EQ(*it, 42);
    mystl::vector<int> v1{1, 42, 2, 3};
    EXPECT_EQ(v, v1);

    // insert - 填充
    it = v.insert(v.begin() + 2, 2, 10);
    EXPECT_EQ(*it, 10);
    mystl::vector<int> v2{1, 42, 10, 10, 2, 3};
    EXPECT_EQ(v, v2);

    // insert - 范围
    int arr[] = {7, 8};
    it = v.insert(v.begin() + 3, arr, arr + 2);
    EXPECT_EQ(*it, 7);
    mystl::vector<int> v3{1, 42, 10, 7, 8, 10, 2, 3};
    EXPECT_EQ(v, v3);

    // insert - 初始化列表
    it = v.insert(v.begin(), {0, 0});
    EXPECT_EQ(*it, 0);
    mystl::vector<int> v4{0, 0, 1, 42, 10, 7, 8, 10, 2, 3};
    EXPECT_EQ(v, v4);

    // emplace
    struct Point 
    {
        int x, y;
        Point(int x_, int y_) : x(x_), y(y_) {}
        bool operator==(const Point& p) const { return x == p.x && y == p.y; }
    };

    mystl::vector<Point> vp;
    auto pit = vp.emplace(vp.begin(), 1, 2);
    EXPECT_EQ(*pit, Point(1, 2));

    // emplace_back
    vp.emplace_back(3, 4);
    EXPECT_EQ(vp.back(), Point(3, 4));

    // push_back
    v.push_back(42);
    EXPECT_EQ(v.back(), 42);

    // pop_back
    v.pop_back();
    EXPECT_EQ(v.back(), 3);

    // erase - 单个元素
    it = v.erase(v.begin());
    EXPECT_EQ(*it, 0);

    // erase - 范围
    it = v.erase(v.begin(), v.begin() + 2);
    EXPECT_EQ(*it, 42);
}

// 比较操作测试
TEST(VectorTest, Comparison) 
{
    mystl::vector<int> v1{1, 2, 3};
    mystl::vector<int> v2{1, 2, 3};
    mystl::vector<int> v3{1, 2, 4};
    mystl::vector<int> v4{1, 2};
    
    EXPECT_TRUE(v1 == v2);
    EXPECT_FALSE(v1 == v3);
    EXPECT_TRUE(v1 != v3);
    EXPECT_TRUE(v1 > v4);
    EXPECT_TRUE(v4 < v1);
    EXPECT_TRUE(v1 >= v2);
    EXPECT_TRUE(v1 <= v2);
}

// 异常安全测试
struct ThrowOnCopy 
{
    int value;
    static bool should_throw;
    static int copy_count;
    
    ThrowOnCopy(int v = 0) noexcept : value(v) {}
    
    ThrowOnCopy(const ThrowOnCopy& other) 
    {
        if (should_throw) 
        {
            throw std::runtime_error("Copy error");
        }
        value = other.value;
    }
    
    ThrowOnCopy& operator=(const ThrowOnCopy& other) 
    {
        if (should_throw) 
        {
            throw std::runtime_error("Assignment error");
        }
        value = other.value;
        return *this;
    }
    
    // 移动操作不抛出异常
    ThrowOnCopy(ThrowOnCopy&& other) noexcept 
        : value(other.value) {}
    
    ThrowOnCopy& operator=(ThrowOnCopy&& other) noexcept 
    {
        value = other.value;
        return *this;
    }
    
    static void reset() { should_throw = false; }
};

bool ThrowOnCopy::should_throw = false;
int ThrowOnCopy::copy_count = 0;

TEST(VectorTest, ExceptionSafety) 
{
    mystl::vector<ThrowOnCopy> v;
    
    // 先添加两个元素（不抛出异常）
    ThrowOnCopy::should_throw = false;
    v.push_back(1);
    v.push_back(2);
    
    // 验证初始状态
    EXPECT_EQ(v.size(), 2);
    EXPECT_EQ(v[0].value, 1);
    EXPECT_EQ(v[1].value, 2);
    
    // 设置抛出异常，并尝试添加元素
    ThrowOnCopy::should_throw = true;
    try 
    {
        v.push_back(3);
        FAIL() << "Expected std::runtime_error";
    }
    catch (const std::runtime_error&) 
    {
        // 验证容器状态未改变
        EXPECT_EQ(v.size(), 2);
        EXPECT_EQ(v[0].value, 1);
        EXPECT_EQ(v[1].value, 2);
    }
}

// 测试 insert 的异常安全性
TEST(VectorTest, InsertExceptionSafety) 
{
    // 测试单个元素插入
    {
        mystl::vector<ThrowOnCopy> vec;
        ThrowOnCopy::reset();  // 重置计数器和标志
        vec.push_back(ThrowOnCopy(1));
        vec.push_back(ThrowOnCopy(2));
        
        ThrowOnCopy::reset();  // 重置计数器和标志
        ThrowOnCopy value(3);
        ThrowOnCopy::should_throw = true;  // 设置标志，使下一次拷贝抛出异常
        
        try 
        {
            vec.insert(vec.begin() + 1, value);
            FAIL() << "Expected std::runtime_error";
        }
        catch (const std::runtime_error&) 
        {
            // 验证容器状态未改变
            EXPECT_EQ(vec.size(), 2);
            EXPECT_EQ(vec[0].value, 1);
            EXPECT_EQ(vec[1].value, 2);
        }
    }
    
    // 测试多个元素插入
    {
        mystl::vector<ThrowOnCopy> vec;
        ThrowOnCopy::reset();  // 重置计数器和标志
        vec.push_back(ThrowOnCopy(1));
        vec.push_back(ThrowOnCopy(2));
        
        ThrowOnCopy::reset();  // 重置计数器和标志
        ThrowOnCopy::should_throw = true;  // 设置标志，使下一次拷贝抛出异常
        
        try 
        {
            vec.insert(vec.begin() + 1, 2, ThrowOnCopy(3));
            FAIL() << "Expected std::runtime_error";
        }
        catch (const std::runtime_error&) 
        {
            // 验证容器状态未改变
            EXPECT_EQ(vec.size(), 2);
            EXPECT_EQ(vec[0].value, 1);
            EXPECT_EQ(vec[1].value, 2);
        }
    }
}

// 测试 emplace 的异常安全性
TEST(VectorTest, EmplaceExceptionSafety) 
{
    mystl::vector<ThrowOnCopy> vec;
    ThrowOnCopy::reset();  // 重置计数器和标志
    vec.push_back(ThrowOnCopy(1));
    vec.push_back(ThrowOnCopy(2));
    
    ThrowOnCopy::reset();  // 重置计数器和标志
    ThrowOnCopy::should_throw = true;  // 设置标志，使下一次拷贝抛出异常
    
    try 
    {
        vec.emplace(vec.begin() + 1, 3);
        FAIL() << "Expected std::runtime_error";
    }
    catch (const std::runtime_error&) 
    {
        // 验证容器状态未改变
        EXPECT_EQ(vec.size(), 2);
        EXPECT_EQ(vec[0].value, 1);
        EXPECT_EQ(vec[1].value, 2);
    }
}

// 测试范围插入的自引用情况
TEST(VectorTest, InsertSelfReference) 
{
    mystl::vector<int> vec = {1, 2, 3, 4, 5};
    
    // 测试自引用插入
    vec.insert(vec.begin(), vec.begin() + 1, vec.end());
    
    // 验证结果：[2, 3, 4, 5, 1, 2, 3, 4, 5]
    ASSERT_EQ(vec.size(), 9);
    std::vector<int> expected = {2, 3, 4, 5, 1, 2, 3, 4, 5};
    for (size_t i = 0; i < vec.size(); ++i) 
    {
        EXPECT_EQ(vec[i], expected[i]);
    }
}

// 测试移动语义的异常安全性
TEST(VectorTest, MoveExceptionSafety) 
{
    mystl::vector<ThrowOnCopy> vec;
    ThrowOnCopy::reset();  // 重置计数器和标志
    vec.push_back(ThrowOnCopy(1));
    vec.push_back(ThrowOnCopy(2));
    
    ThrowOnCopy::reset();  // 重置计数器和标志
    ThrowOnCopy value(3);
    ThrowOnCopy::should_throw = true;  // 设置标志，使下一次拷贝抛出异常
    
    try 
    {
        vec.insert(vec.begin() + 1, std::move(value));
        FAIL() << "Expected std::runtime_error";
    }
    catch (const std::runtime_error&) 
    {
        // 验证容器状态未改变
        EXPECT_EQ(vec.size(), 2);
        EXPECT_EQ(vec[0].value, 1);
        EXPECT_EQ(vec[1].value, 2);
        // 原始值应该保持不变
        EXPECT_EQ(value.value, 3);
    }
}

// 测试 reserve 的异常安全性
TEST(VectorTest, ReserveExceptionSafety) 
{
    mystl::vector<ThrowOnCopy> vec;
    ThrowOnCopy::reset();  // 重置计数器和标志
    vec.push_back(ThrowOnCopy(1));
    vec.push_back(ThrowOnCopy(2));
    
    // 验证初始状态
    EXPECT_EQ(vec.size(), 2);
    EXPECT_EQ(vec[0].value, 1);
    EXPECT_EQ(vec[1].value, 2);
    
    size_t old_capacity = vec.capacity();
    ThrowOnCopy::reset();  // 重置计数器和标志
    ThrowOnCopy::should_throw = true;  // 设置标志，使下一次拷贝抛出异常
    
    try 
    {
        vec.reserve(10);  // 尝试扩容
        FAIL() << "Expected std::runtime_error";
    }
    catch (const std::runtime_error&) 
    {
        // 验证容器状态未改变
        EXPECT_EQ(vec.size(), 2);
        EXPECT_EQ(vec.capacity(), old_capacity);
        EXPECT_EQ(vec[0].value, 1);
        EXPECT_EQ(vec[1].value, 2);
    }
}

// 测试 resize 的异常安全性
TEST(VectorTest, ResizeExceptionSafety) 
{
    // 测试扩大容量时的异常安全性
    {
        mystl::vector<ThrowOnCopy> vec;
        ThrowOnCopy::reset();  // 重置计数器和标志
        vec.push_back(ThrowOnCopy(1));
        vec.push_back(ThrowOnCopy(2));
        
        size_t old_size = vec.size();
        size_t old_capacity = vec.capacity();
        ThrowOnCopy::reset();  // 重置计数器和标志
        ThrowOnCopy::should_throw = true;  // 设置标志，使下一次拷贝抛出异常
        
        try 
        {
            vec.resize(4);  // 尝试扩大容量
            FAIL() << "Expected std::runtime_error";
        }
        catch (const std::runtime_error&) 
        {
            // 验证容器状态未改变
            EXPECT_EQ(vec.size(), old_size);
            EXPECT_EQ(vec.capacity(), old_capacity);
            EXPECT_EQ(vec[0].value, 1);
            EXPECT_EQ(vec[1].value, 2);
        }
    }
    
    // 测试使用值填充时的异常安全性
    {
        mystl::vector<ThrowOnCopy> vec;
        ThrowOnCopy::reset();  // 重置计数器和标志
        vec.push_back(ThrowOnCopy(1));
        vec.push_back(ThrowOnCopy(2));
        
        size_t old_size = vec.size();
        size_t old_capacity = vec.capacity();
        ThrowOnCopy value(3);
        ThrowOnCopy::reset();  // 重置计数器和标志
        ThrowOnCopy::should_throw = true;  // 设置标志，使下一次拷贝抛出异常
        
        try 
        {
            vec.resize(4, value);  // 尝试用值填充扩容
            FAIL() << "Expected std::runtime_error";
        }
        catch (const std::runtime_error&) 
        {
            // 验证容器状态未改变
            EXPECT_EQ(vec.size(), old_size);
            EXPECT_EQ(vec.capacity(), old_capacity);
            EXPECT_EQ(vec[0].value, 1);
            EXPECT_EQ(vec[1].value, 2);
        }
    }
}

// 测试 assign 的异常安全性
TEST(VectorTest, AssignExceptionSafety) 
{
    // 测试填充 assign 的异常安全性
    {
        mystl::vector<ThrowOnCopy> vec;
        ThrowOnCopy::should_throw = false;  // 禁用异常
        vec.push_back(ThrowOnCopy(1));
        vec.push_back(ThrowOnCopy(2));
        
        size_t old_size = vec.size();
        size_t old_capacity = vec.capacity();
        ThrowOnCopy value(3);
        ThrowOnCopy::reset();  // 重置计数器
        ThrowOnCopy::should_throw = true;  // 启用异常
        
        try 
        {
            vec.assign(4, value);  // 尝试用值填充
            FAIL() << "Expected std::runtime_error";
        }
        catch (const std::runtime_error&) 
        {
            // 验证容器状态未改变
            EXPECT_EQ(vec.size(), old_size);
            EXPECT_EQ(vec.capacity(), old_capacity);
            EXPECT_EQ(vec[0].value, 1);
            EXPECT_EQ(vec[1].value, 2);
        }
    }
    
    // 测试范围 assign 的异常安全性
    {
        mystl::vector<ThrowOnCopy> vec;
        ThrowOnCopy::should_throw = false;  // 禁用异常
        vec.push_back(ThrowOnCopy(1));
        vec.push_back(ThrowOnCopy(2));
        
        mystl::vector<ThrowOnCopy> src;
        src.push_back(ThrowOnCopy(3));
        src.push_back(ThrowOnCopy(4));
        src.push_back(ThrowOnCopy(5));
        
        size_t old_size = vec.size();
        size_t old_capacity = vec.capacity();
        ThrowOnCopy::reset();  // 重置计数器
        ThrowOnCopy::should_throw = true;  // 启用异常
        
        try 
        {
            vec.assign(src.begin(), src.end());  // 尝试用范围赋值
            FAIL() << "Expected std::runtime_error";
        }
        catch (const std::runtime_error&) 
        {
            // 验证容器状态未改变
            EXPECT_EQ(vec.size(), old_size);
            EXPECT_EQ(vec.capacity(), old_capacity);
            EXPECT_EQ(vec[0].value, 1);
            EXPECT_EQ(vec[1].value, 2);
        }
    }

    // 测试初始化列表 assign 的异常安全性
    {
        mystl::vector<ThrowOnCopy> vec;
        ThrowOnCopy::should_throw = false;  // 禁用异常
        vec.push_back(ThrowOnCopy(1));
        vec.push_back(ThrowOnCopy(2));
        
        size_t old_size = vec.size();
        size_t old_capacity = vec.capacity();
        
        // 构造初始化列表时禁用异常
        std::initializer_list<ThrowOnCopy> ilist = {ThrowOnCopy(3), ThrowOnCopy(4), ThrowOnCopy(5)};
        
        // 在 assign 操作前启用异常
        ThrowOnCopy::reset();  // 重置状态
        ThrowOnCopy::should_throw = true;  // 启用异常
        
        try 
        {
            vec.assign(ilist);  // 尝试用初始化列表赋值
            FAIL() << "Expected std::runtime_error";
        }
        catch (const std::runtime_error&) 
        {
            // 验证容器状态未改变
            EXPECT_EQ(vec.size(), old_size);
            EXPECT_EQ(vec.capacity(), old_capacity);
            EXPECT_EQ(vec[0].value, 1);
            EXPECT_EQ(vec[1].value, 2);
        }
    }
}

// 测试迭代器失效
TEST(VectorTest, IteratorInvalidation) 
{
    // 测试扩容导致的迭代器失效
    {
        mystl::vector<int> vec{1, 2, 3};
        auto it = vec.begin() + 1;
        auto old_capacity = vec.capacity();
        
        // 通过push_back触发扩容
        while (vec.capacity() == old_capacity) 
        {
            vec.push_back(4);
        }
        
        // 此时迭代器已失效，不应该使用
        EXPECT_NE(vec.data(), &(*it) - 1);  // 检查底层数组是否已经改变
    }
    
    // 测试删除元素导致的迭代器失效
    {
        mystl::vector<int> vec{1, 2, 3, 4, 5};
        auto it2 = vec.begin() + 2;
        auto it3 = vec.begin() + 3;
        
        // 删除一个元素后，其后的迭代器都应该失效
        vec.erase(vec.begin() + 1);
        
        // 原来指向的元素已经移动
        EXPECT_EQ(*it2, 4);
        EXPECT_EQ(*it3, 5);
    }
    
    // 测试插入元素导致的迭代器失效
    {
        mystl::vector<int> vec{1, 2, 3};
        auto it = vec.begin() + 1;
        
        // 在不触发扩容的情况下插入元素
        if (vec.capacity() > vec.size()) 
        {
            vec.insert(vec.begin(), 0);
            // 此时it指向的位置应该后移一位
            EXPECT_EQ(*it, 1);
        }
    }
}

// 测试内存对齐
TEST(VectorTest, Alignment) 
{
    // 测试基本类型的对齐
    {
        mystl::vector<double> vec(5);
        EXPECT_EQ(reinterpret_cast<std::uintptr_t>(vec.data()) % alignof(double), 0);
    }
    
    // 测试自然对齐的类型
    {
        struct NaturalAligned 
        {
            int x;
            double y;
        };
        
        mystl::vector<NaturalAligned> vec(5);
        EXPECT_EQ(reinterpret_cast<std::uintptr_t>(vec.data()) % alignof(NaturalAligned), 0);
        
        // 测试扩容后的对齐
        vec.push_back(NaturalAligned());
        EXPECT_EQ(reinterpret_cast<std::uintptr_t>(vec.data()) % alignof(NaturalAligned), 0);
    }
} 
