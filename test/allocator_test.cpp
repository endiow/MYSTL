#include <gtest/gtest.h>
#include "mystl/allocator.hpp"

// 测试基本分配和释放
TEST(AllocatorTest, BasicAllocation) {
    mystl::allocator<int> alloc;
    
    // 分配单个对象
    int* p = alloc.allocate(1);
    EXPECT_NE(p, nullptr);
    alloc.deallocate(p, 1);
    
    // 分配多个对象
    int* arr = alloc.allocate(5);
    EXPECT_NE(arr, nullptr);
    alloc.deallocate(arr, 5);
}

// 测试构造和析构
TEST(AllocatorTest, ConstructDestroy) {
    mystl::allocator<std::string> alloc;
    
    // 分配内存
    std::string* p = alloc.allocate(1);
    
    // 构造对象
    alloc.construct(p, "test");
    EXPECT_EQ(*p, "test");
    
    // 析构对象
    alloc.destroy(p);
    
    // 释放内存
    alloc.deallocate(p, 1);
}

// 测试复杂类型
class ComplexType {
public:
    static int constructor_count;
    static int destructor_count;
    
    ComplexType() { ++constructor_count; }
    ~ComplexType() { ++destructor_count; }
    
    static void reset_counters() {
        constructor_count = 0;
        destructor_count = 0;
    }
};

int ComplexType::constructor_count = 0;
int ComplexType::destructor_count = 0;

TEST(AllocatorTest, ComplexTypeAllocation) {
    ComplexType::reset_counters();
    mystl::allocator<ComplexType> alloc;
    
    // 分配并构造对象数组
    ComplexType* arr = alloc.allocate(3);
    for (int i = 0; i < 3; ++i) {
        alloc.construct(arr + i);
    }
    
    EXPECT_EQ(ComplexType::constructor_count, 3);
    
    // 析构并释放对象数组
    for (int i = 0; i < 3; ++i) {
        alloc.destroy(arr + i);
    }
    alloc.deallocate(arr, 3);
    
    EXPECT_EQ(ComplexType::destructor_count, 3);
}

// 测试异常情况
TEST(AllocatorTest, ExceptionCases) {
    mystl::allocator<int> alloc;
    
    // 测试分配大小为0的内存
    EXPECT_NO_THROW({
        int* p = alloc.allocate(0);
        alloc.deallocate(p, 0);
    });
    
    // 测试分配非常大的内存（可能失败）
    //EXPECT_ANY_THROW(alloc.allocate(size_t(-1) / sizeof(int)));
}

// 测试不同类型的分配器
TEST(AllocatorTest, DifferentTypes) {
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