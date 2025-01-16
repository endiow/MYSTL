#include <gtest/gtest.h>
#include "mystl/allocator.hpp"
#include "mystl/construct.hpp"
#include "mystl/vector.hpp"

// 测试基本类型的分配和释放
TEST(AllocatorTest, BasicTypes) 
{
    mystl::allocator<int> alloc;
    
    // 分配单个对象
    int* p1 = alloc.allocate(1);
    EXPECT_NE(p1, nullptr);
    mystl::construct(p1, 42);
    EXPECT_EQ(*p1, 42);
    mystl::destroy_at(p1);
    alloc.deallocate(p1, 1);

    // 分配数组
    int* p2 = alloc.allocate(5);
    EXPECT_NE(p2, nullptr);
    for (int i = 0; i < 5; ++i) 
    {
        mystl::construct(p2 + i, i);
    }
    for (int i = 0; i < 5; ++i) 
    {
        EXPECT_EQ(p2[i], i);
    }
    for (int i = 0; i < 5; ++i) 
    {
        mystl::destroy_at(p2 + i);
    }
    alloc.deallocate(p2, 5);

    // 测试不同基本类型
    mystl::allocator<char> char_alloc;
    char* pc = char_alloc.allocate(1);
    mystl::construct(pc, 'A');
    EXPECT_EQ(*pc, 'A');
    mystl::destroy_at(pc);
    char_alloc.deallocate(pc, 1);

    mystl::allocator<double> double_alloc;
    double* pd = double_alloc.allocate(1);
    mystl::construct(pd, 3.14);
    EXPECT_DOUBLE_EQ(*pd, 3.14);
    mystl::destroy_at(pd);
    double_alloc.deallocate(pd, 1);
}

// 测试自定义类型
TEST(AllocatorTest, CustomTypes) 
{
    struct TestStruct 
    {
        int x;
        double y;
        std::string s;
        
        TestStruct() : x(0), y(0.0), s("") {}
        TestStruct(int x_, double y_, std::string s_) 
            : x(x_), y(y_), s(s_) {}
        
        bool operator==(const TestStruct& other) const 
        {
            return x == other.x && y == other.y && s == other.s;
        }
    };

    mystl::allocator<TestStruct> alloc;
    
    // 分配并构造单个对象
    TestStruct* p1 = alloc.allocate(1);
    EXPECT_NE(p1, nullptr);
    mystl::construct(p1, 42, 3.14, "test");
    EXPECT_EQ(p1->x, 42);
    EXPECT_DOUBLE_EQ(p1->y, 3.14);
    EXPECT_EQ(p1->s, "test");
    mystl::destroy_at(p1);
    alloc.deallocate(p1, 1);

    // 分配并构造数组
    TestStruct* p2 = alloc.allocate(3);
    EXPECT_NE(p2, nullptr);
    for (int i = 0; i < 3; ++i) 
    {
        mystl::construct(p2 + i, i, i * 1.1, "test" + std::to_string(i));
    }
    for (int i = 0; i < 3; ++i) 
    {
        EXPECT_EQ(p2[i].x, i);
        EXPECT_DOUBLE_EQ(p2[i].y, i * 1.1);
        EXPECT_EQ(p2[i].s, "test" + std::to_string(i));
    }
    for (int i = 0; i < 3; ++i) 
    {
        mystl::destroy_at(p2 + i);
    }
    alloc.deallocate(p2, 3);
}

// 测试构造和析构
TEST(AllocatorTest, ConstructDestroy) 
{
    mystl::allocator<std::string> alloc;
    
    // 分配内存
    std::string* p = alloc.allocate(1);
    
    // 构造对象
    mystl::construct(p, "test");
    EXPECT_EQ(*p, "test");
    
    // 析构对象
    mystl::destroy_at(p);
    
    // 释放内存
    alloc.deallocate(p, 1);
}

// 测试复杂类型
class ComplexType 
{
public:
    static int constructor_count;
    static int destructor_count;
    
    ComplexType() { ++constructor_count; }
    ~ComplexType() { ++destructor_count; }
    
    static void reset_counters() 
    {
        constructor_count = 0;
        destructor_count = 0;
    }
};

int ComplexType::constructor_count = 0;
int ComplexType::destructor_count = 0;

TEST(AllocatorTest, ComplexTypeAllocation) 
{
    ComplexType::reset_counters();
    mystl::allocator<ComplexType> alloc;
    
    // 分配并构造对象数组
    ComplexType* arr = alloc.allocate(3);
    for (int i = 0; i < 3; ++i) 
    {
        mystl::construct(arr + i);
    }
    
    EXPECT_EQ(ComplexType::constructor_count, 3);
    
    // 析构并释放对象数组
    for (int i = 0; i < 3; ++i) 
    {
        mystl::destroy_at(arr + i);
    }
    alloc.deallocate(arr, 3);
    
    EXPECT_EQ(ComplexType::destructor_count, 3);
}

// 测试异常情况
TEST(AllocatorTest, ExceptionCases) 
{
    mystl::allocator<int> alloc;
    
    // 测试分配大小为0的内存
    EXPECT_NO_THROW(
    {
        int* p = alloc.allocate(0);
        alloc.deallocate(p, 0);
    });
    
    // 测试分配较大但合理的内存大小
    constexpr size_t large_size = 1024 * 1024 * 1024;  // 1GB
    EXPECT_NO_THROW(
    {
        try 
        {
            int* p = alloc.allocate(large_size);
            alloc.deallocate(p, large_size);
        }
        catch (const std::bad_alloc&) 
        {
            // 如果内存不足，捕获异常是正常的
            throw;
        }
    });

    // 测试分配负数大小（应该在编译时就报错）
    // int* p = alloc.allocate(-1);  // 这行应该编译失败

    // 测试释放nullptr
    EXPECT_NO_THROW(alloc.deallocate(nullptr, 0));
}

// 测试不同类型的分配器
TEST(AllocatorTest, DifferentTypes) 
{
    mystl::allocator<int> int_alloc;
    mystl::allocator<double> double_alloc;
    mystl::allocator<char> char_alloc;
    
    // 分配不同类型的内存
    int* pi = int_alloc.allocate(1);
    double* pd = double_alloc.allocate(1);
    char* pc = char_alloc.allocate(1);
    
    EXPECT_NE(pi, nullptr);
    EXPECT_NE(pd, nullptr);
    EXPECT_NE(pc, nullptr);
    
    int_alloc.deallocate(pi, 1);
    double_alloc.deallocate(pd, 1);
    char_alloc.deallocate(pc, 1);
} 