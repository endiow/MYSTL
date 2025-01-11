#include <gtest/gtest.h>
#include "mystl/uninitialized.hpp"
#include <memory>
#include <vector>

// 用于测试的简单类
class TestClass 
{
public:
    static int constructor_count;
    static int destructor_count;
    
    TestClass() : value_(0) { ++constructor_count; }
    explicit TestClass(int v) : value_(v) { ++constructor_count; }
    TestClass(const TestClass& other) : value_(other.value_) { ++constructor_count; }
    ~TestClass() { ++destructor_count; }
    
    int value() const { return value_; }
    
    static void reset_counters() 
    {
        constructor_count = 0;
        destructor_count = 0;
    }
    
private:
    int value_;
};

int TestClass::constructor_count = 0;
int TestClass::destructor_count = 0;

TEST(UninitializedTest, UninitializedFill) 
{
    TestClass::reset_counters();
    
    alignas(TestClass) unsigned char buffer[sizeof(TestClass) * 3];
    TestClass* start = reinterpret_cast<TestClass*>(buffer);
    TestClass* end = start + 3;
    
    TestClass value(42);
    mystl::uninitialized_fill(start, end, value);
    
    EXPECT_EQ(TestClass::constructor_count, 4);  // 1 for value, 3 for fill
    EXPECT_EQ(start[0].value(), 42);
    EXPECT_EQ(start[1].value(), 42);
    EXPECT_EQ(start[2].value(), 42);
    
    mystl::destroy(start, end);
    EXPECT_EQ(TestClass::destructor_count, 3);
}

TEST(UninitializedTest, UninitializedFillN) 
{
    TestClass::reset_counters();
    
    alignas(TestClass) unsigned char buffer[sizeof(TestClass) * 3];
    TestClass* start = reinterpret_cast<TestClass*>(buffer);
    
    TestClass value(42);
    mystl::uninitialized_fill_n(start, 3, value);
    
    EXPECT_EQ(TestClass::constructor_count, 4);  // 1 for value, 3 for fill_n
    EXPECT_EQ(start[0].value(), 42);
    EXPECT_EQ(start[1].value(), 42);
    EXPECT_EQ(start[2].value(), 42);
    
    mystl::destroy(start, start + 3);
    EXPECT_EQ(TestClass::destructor_count, 3);
}

TEST(UninitializedTest, UninitializedCopy) 
{
    TestClass::reset_counters();
    
    std::vector<TestClass> src;
    src.emplace_back(1);
    src.emplace_back(2);
    src.emplace_back(3);
    
    alignas(TestClass) unsigned char buffer[sizeof(TestClass) * 3];
    TestClass* start = reinterpret_cast<TestClass*>(buffer);
    
    mystl::uninitialized_copy(src.begin(), src.end(), start);
    
    EXPECT_EQ(TestClass::constructor_count, 6);  // 3 for src, 3 for copy
    EXPECT_EQ(start[0].value(), 1);
    EXPECT_EQ(start[1].value(), 2);
    EXPECT_EQ(start[2].value(), 3);
    
    mystl::destroy(start, start + 3);
    EXPECT_EQ(TestClass::destructor_count, 3);
}

TEST(UninitializedTest, UninitializedMove) 
{
    TestClass::reset_counters();
    
    std::vector<TestClass> src;
    src.emplace_back(1);
    src.emplace_back(2);
    src.emplace_back(3);
    
    alignas(TestClass) unsigned char buffer[sizeof(TestClass) * 3];
    TestClass* start = reinterpret_cast<TestClass*>(buffer);
    
    mystl::uninitialized_move(src.begin(), src.end(), start);
    
    EXPECT_EQ(TestClass::constructor_count, 6);  // 3 for src, 3 for move
    EXPECT_EQ(start[0].value(), 1);
    EXPECT_EQ(start[1].value(), 2);
    EXPECT_EQ(start[2].value(), 3);
    
    mystl::destroy(start, start + 3);
    EXPECT_EQ(TestClass::destructor_count, 3);
}

TEST(UninitializedTest, Destroy) 
{
    TestClass::reset_counters();
    
    alignas(TestClass) unsigned char buffer[sizeof(TestClass) * 3];
    TestClass* start = reinterpret_cast<TestClass*>(buffer);
    
    new (start) TestClass(1);
    new (start + 1) TestClass(2);
    new (start + 2) TestClass(3);
    
    EXPECT_EQ(TestClass::constructor_count, 3);
    
    mystl::destroy(start, start + 3);
    EXPECT_EQ(TestClass::destructor_count, 3);
}

TEST(UninitializedTest, DestroyAt) 
{
    TestClass::reset_counters();
    
    alignas(TestClass) unsigned char buffer[sizeof(TestClass)];
    TestClass* ptr = reinterpret_cast<TestClass*>(buffer);
    
    new (ptr) TestClass(42);
    EXPECT_EQ(TestClass::constructor_count, 1);
    
    mystl::destroy_at(ptr);
    EXPECT_EQ(TestClass::destructor_count, 1);
} 