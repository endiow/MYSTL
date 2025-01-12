#include <gtest/gtest.h>
#include "mystl/uninitialized.hpp"
#include "mystl/allocator.hpp"
#include <string>

// 测试基本类型的未初始化操作
TEST(UninitializedTest, BasicTypes) 
{
    mystl::allocator<int> alloc;
    
    // 测试 uninitialized_fill
    {
        int* space = alloc.allocate(5);
        mystl::uninitialized_fill(space, space + 5, 42);
        
        for (int i = 0; i < 5; ++i) 
        {
            EXPECT_EQ(space[i], 42);
        }
        
        mystl::destroy(space, space + 5);
        alloc.deallocate(space, 5);
    }
    
    // 测试 uninitialized_fill_n
    {
        int* space = alloc.allocate(5);
        mystl::uninitialized_fill_n(space, 3, 42);  // 只填充前3个
        
        for (int i = 0; i < 3; ++i) 
        {
            EXPECT_EQ(space[i], 42);
        }
        
        mystl::destroy(space, space + 3);
        alloc.deallocate(space, 5);
    }
    
    // 测试 uninitialized_copy
    {
        int arr[] = {1, 2, 3, 4, 5};
        int* space = alloc.allocate(5);
        
        mystl::uninitialized_copy(arr, arr + 5, space);
        
        for (int i = 0; i < 5; ++i) 
        {
            EXPECT_EQ(space[i], arr[i]);
        }
        
        mystl::destroy(space, space + 5);
        alloc.deallocate(space, 5);
    }
    
    // 测试 uninitialized_copy_n
    {
        int arr[] = {1, 2, 3, 4, 5};
        int* space = alloc.allocate(3);
        
        mystl::uninitialized_copy_n(arr, 3, space);  // 只复制前3个
        
        for (int i = 0; i < 3; ++i) 
        {
            EXPECT_EQ(space[i], arr[i]);
        }
        
        mystl::destroy(space, space + 3);
        alloc.deallocate(space, 3);
    }
    
    // 测试 uninitialized_move
    {
        int arr[] = {1, 2, 3, 4, 5};
        int* space = alloc.allocate(5);
        
        mystl::uninitialized_move(arr, arr + 5, space);
        
        for (int i = 0; i < 5; ++i) 
        {
            EXPECT_EQ(space[i], arr[i]);
        }
        
        mystl::destroy(space, space + 5);
        alloc.deallocate(space, 5);
    }
    
    // 测试 uninitialized_move_n
    {
        int arr[] = {1, 2, 3, 4, 5};
        int* space = alloc.allocate(3);
        
        mystl::uninitialized_move_n(arr, 3, space);  // 只移动前3个
        
        for (int i = 0; i < 3; ++i) 
        {
            EXPECT_EQ(space[i], arr[i]);
        }
        
        mystl::destroy(space, space + 3);
        alloc.deallocate(space, 3);
    }
}

// 全局计数器
int constructor_count = 0;
int destructor_count = 0;
int copy_count = 0;
int move_count = 0;

// 测试自定义类型
TEST(UninitializedTest, CustomTypes) 
{
    struct Counter 
    {
        int value;
        
        Counter(int v = 0) : value(v) { ++constructor_count; }
        ~Counter() { ++destructor_count; }
        Counter(const Counter& other) : value(other.value) { ++copy_count; }
        Counter(Counter&& other) noexcept : value(other.value) { ++move_count; }
    };

    mystl::allocator<Counter> alloc;
    
    // 测试 uninitialized_fill
    {
        constructor_count = 0;
        destructor_count = 0;
        copy_count = 0;
        move_count = 0;

        Counter* space = alloc.allocate(3);
        mystl::uninitialized_fill(space, space + 3, Counter(42));
        
        EXPECT_EQ(constructor_count, 1);  // 一个临时对象
        EXPECT_EQ(copy_count, 3);         // 三次拷贝
        
        for (int i = 0; i < 3; ++i) 
        {
            EXPECT_EQ(space[i].value, 42);
        }
        
        mystl::destroy(space, space + 3);
        EXPECT_EQ(destructor_count, 4);  // 3个对象 + 1个临时对象
        alloc.deallocate(space, 3);
    }
    
    // 测试 uninitialized_move
    {
        constructor_count = 0;
        destructor_count = 0;
        copy_count = 0;
        move_count = 0;

        Counter arr[] = {Counter(1), Counter(2), Counter(3)};
        Counter* space = alloc.allocate(3);
        
        mystl::uninitialized_move(arr, arr + 3, space);
        
        EXPECT_EQ(move_count, 3);  // 三次移动
        
        for (int i = 0; i < 3; ++i) 
        {
            EXPECT_EQ(space[i].value, i + 1);
        }
        
        mystl::destroy(space, space + 3);
        alloc.deallocate(space, 3);
    }
}

// 全局计数器
int throw_copy_count = 0;
int throw_destructor_count = 0;

// 测试异常安全性
TEST(UninitializedTest, ExceptionSafety) 
{
    struct ThrowOnCopy 
    {
        ThrowOnCopy() = default;
        ThrowOnCopy(const ThrowOnCopy&) 
        {
            if (++throw_copy_count == 3) 
            {
                throw std::runtime_error("Copy constructor threw");
            }
        }
        ~ThrowOnCopy() { ++throw_destructor_count; }
    };
    
    mystl::allocator<ThrowOnCopy> alloc;
    ThrowOnCopy arr[5];
    
    // 测试异常时的资源清理
    {
        throw_copy_count = 0;
        throw_destructor_count = 0;

        ThrowOnCopy* space = alloc.allocate(5);
        
        EXPECT_THROW(
        {
            mystl::uninitialized_copy(arr, arr + 5, space);
        }, std::runtime_error);
        
        // 验证已构造的对象被正确析构
        EXPECT_EQ(throw_destructor_count, throw_copy_count);
        
        alloc.deallocate(space, 5);
    }
}

// 测试边界情况
TEST(UninitializedTest, EdgeCases) 
{
    mystl::allocator<int> alloc;
    
    // 测试空范围
    {
        int* space = alloc.allocate(1);
        
        // 空范围的操作应该是安全的
        mystl::uninitialized_fill(space, space, 42);
        mystl::uninitialized_copy(space, space, space);
        mystl::uninitialized_move(space, space, space);
        
        alloc.deallocate(space, 1);
    }
    
    // 测试单个元素
    {
        int* space = alloc.allocate(1);
        
        mystl::uninitialized_fill_n(space, 1, 42);
        EXPECT_EQ(*space, 42);
        
        mystl::destroy(space, space + 1);
        alloc.deallocate(space, 1);
    }
}

// 测试平凡类型的优化
TEST(UninitializedTest, TrivialTypeOptimization) 
{
    // 测试平凡拷贝赋值类型
    {
        struct TrivialCopyAssignable 
        {
            int x;
            double y;
            // 默认的拷贝赋值运算符是平凡的
        };
        static_assert(std::is_trivially_copy_assignable_v<TrivialCopyAssignable>);

        mystl::allocator<TrivialCopyAssignable> alloc;
        TrivialCopyAssignable* space = alloc.allocate(3);
        TrivialCopyAssignable arr[3] = {{1, 1.0}, {2, 2.0}, {3, 3.0}};

        mystl::uninitialized_copy(arr, arr + 3, space);
        
        for (int i = 0; i < 3; ++i) 
        {
            EXPECT_EQ(space[i].x, arr[i].x);
            EXPECT_EQ(space[i].y, arr[i].y);
        }

        mystl::destroy(space, space + 3);
        alloc.deallocate(space, 3);
    }

    // 测试平凡移动赋值类型
    {
        struct TrivialMoveAssignable 
        {
            int x;
            double y;
            // 默认的移动赋值运算符是平凡的
        };
        static_assert(std::is_trivially_move_assignable_v<TrivialMoveAssignable>);

        mystl::allocator<TrivialMoveAssignable> alloc;
        TrivialMoveAssignable* space = alloc.allocate(3);
        TrivialMoveAssignable arr[3] = {{1, 1.0}, {2, 2.0}, {3, 3.0}};

        mystl::uninitialized_move(arr, arr + 3, space);
        
        for (int i = 0; i < 3; ++i) 
        {
            EXPECT_EQ(space[i].x, arr[i].x);
            EXPECT_EQ(space[i].y, arr[i].y);
        }

        mystl::destroy(space, space + 3);
        alloc.deallocate(space, 3);
    }
} 