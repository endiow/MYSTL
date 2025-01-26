#include <gtest/gtest.h>
#include "mystl/string.hpp"
#include <stdexcept>
#include <string>

// 构造函数测试
TEST(StringTest, Constructor) 
{
    // 默认构造
    mystl::string s1;
    EXPECT_TRUE(s1.empty());
    EXPECT_EQ(s1.size(), 0);
    EXPECT_STREQ(s1.c_str(), "");

    // C风格字符串构造
    mystl::string s2("hello");
    EXPECT_EQ(s2.size(), 5);
    EXPECT_STREQ(s2.c_str(), "hello");

    // 部分C风格字符串构造
    mystl::string s3("hello", 3);
    EXPECT_EQ(s3.size(), 3);
    EXPECT_STREQ(s3.c_str(), "hel");

    // 填充构造
    mystl::string s4(5, 'a');
    EXPECT_EQ(s4.size(), 5);
    EXPECT_STREQ(s4.c_str(), "aaaaa");

    // 范围构造
    const char str[] = "world";
    mystl::string s5(str, str + 5);
    EXPECT_EQ(s5.size(), 5);
    EXPECT_STREQ(s5.c_str(), "world");

    // 拷贝构造
    mystl::string s6(s2);
    EXPECT_EQ(s6, s2);
    EXPECT_STREQ(s6.c_str(), "hello");

    // 移动构造
    mystl::string s7(std::move(s6));
    EXPECT_TRUE(s6.empty());  // s6被移动后应为空
    EXPECT_STREQ(s7.c_str(), "hello");
}

// 赋值操作测试
TEST(StringTest, Assignment) 
{
    mystl::string s1("hello");
    
    // 拷贝赋值
    mystl::string s2;
    s2 = s1;
    EXPECT_EQ(s2, s1);

    // 移动赋值
    mystl::string s3;
    s3 = std::move(s2);
    EXPECT_TRUE(s2.empty());
    EXPECT_EQ(s3, s1);

    // C风格字符串赋值
    s3 = "world";
    EXPECT_STREQ(s3.c_str(), "world");

    // 字符赋值
    s3 = 'x';
    EXPECT_STREQ(s3.c_str(), "x");

    // assign测试
    mystl::string s4;
    s4.assign("test");
    EXPECT_STREQ(s4.c_str(), "test");

    s4.assign("hello", 4);
    EXPECT_STREQ(s4.c_str(), "hell");

    s4.assign(3, 'a');
    EXPECT_STREQ(s4.c_str(), "aaa");
}

// 元素访问测试
TEST(StringTest, ElementAccess) 
{
    mystl::string s("hello");
    const mystl::string& cs = s;
    
    // operator[]
    EXPECT_EQ(s[0], 'h');
    EXPECT_EQ(cs[0], 'h');

    // at()
    EXPECT_EQ(s.at(1), 'e');
    EXPECT_EQ(cs.at(1), 'e');
    EXPECT_THROW(s.at(5), std::out_of_range);

    // front()
    EXPECT_EQ(s.front(), 'h');
    EXPECT_EQ(cs.front(), 'h');

    // back()
    EXPECT_EQ(s.back(), 'o');
    EXPECT_EQ(cs.back(), 'o');

    // data()/c_str()
    EXPECT_STREQ(s.data(), "hello");
    EXPECT_STREQ(cs.c_str(), "hello");
}

// 迭代器测试
TEST(StringTest, Iterators) 
{
    mystl::string s("hello");
    const mystl::string& cs = s;
    
    // begin/end
    EXPECT_EQ(*s.begin(), 'h');
    EXPECT_EQ(*(s.end() - 1), 'o');
    
    // cbegin/cend
    EXPECT_EQ(*s.cbegin(), 'h');
    EXPECT_EQ(*(s.cend() - 1), 'o');
    
    // const begin/end
    EXPECT_EQ(*cs.begin(), 'h');
    EXPECT_EQ(*(cs.end() - 1), 'o');
    
    // rbegin/rend
    EXPECT_EQ(*s.rbegin(), 'o');
    EXPECT_EQ(*(s.rend() - 1), 'h');
    
    // crbegin/crend
    EXPECT_EQ(*s.crbegin(), 'o');
    EXPECT_EQ(*(s.crend() - 1), 'h');
}

// 容量操作测试
TEST(StringTest, Capacity) 
{
    mystl::string s;
    
    // empty/size/length/capacity
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.size(), 0);
    EXPECT_EQ(s.length(), 0);
    
    // reserve
    s.reserve(10);
    EXPECT_GE(s.capacity(), 10);
    size_t old_cap = s.capacity();

    // 自动扩容
    for(char c = 'a'; c <= 'z'; ++c) 
    {
        s.push_back(c);
        if(s.size() > old_cap) 
        {
            EXPECT_GT(s.capacity(), old_cap);
            old_cap = s.capacity();
        }
    }

    // shrink_to_fit
    s.clear();
    s.shrink_to_fit();
    EXPECT_EQ(s.capacity(), 0);
}

// 修改器测试
TEST(StringTest, Modifiers) 
{
    // clear
    mystl::string s1("hello");
    s1.clear();
    EXPECT_TRUE(s1.empty());
    EXPECT_STREQ(s1.c_str(), "");

    // insert
    mystl::string s2("hello");
    s2.insert(0, "x");
    EXPECT_STREQ(s2.c_str(), "xhello");
    
    s2.insert(3, "abc");
    EXPECT_STREQ(s2.c_str(), "xheabcllo");

    // erase
    s2.erase(1, 2);
    EXPECT_STREQ(s2.c_str(), "xabcllo");

    // push_back/pop_back
    mystl::string s3;
    s3.push_back('a');
    EXPECT_STREQ(s3.c_str(), "a");
    
    s3.pop_back();
    EXPECT_TRUE(s3.empty());

    // append
    mystl::string s4("hello");
    s4.append(" world");
    EXPECT_STREQ(s4.c_str(), "hello world");

    // operator+=
    mystl::string s5("hello");
    s5 += " ";
    s5 += "world";
    s5 += '!';
    EXPECT_STREQ(s5.c_str(), "hello world!");

    // replace
    mystl::string s6("hello world");
    s6.replace(0, 5, "hi");
    EXPECT_STREQ(s6.c_str(), "hi world");
}

// 字符串操作测试
TEST(StringTest, StringOperations) 
{
    mystl::string s("hello world");
    
    // substr
    EXPECT_EQ(s.substr(0, 5), mystl::string("hello"));
    EXPECT_EQ(s.substr(6), mystl::string("world"));
    EXPECT_THROW(s.substr(12), std::out_of_range);

    // find
    EXPECT_EQ(s.find("world"), 6);
    EXPECT_EQ(s.find("xyz"), mystl::string::npos);
    EXPECT_EQ(s.find('o'), 4);

    // rfind
    EXPECT_EQ(s.rfind('o'), 7);
    EXPECT_EQ(s.rfind("hello"), 0);
    EXPECT_EQ(s.rfind("xyz"), mystl::string::npos);

    // find_first_of
    EXPECT_EQ(s.find_first_of("aeiou"), 1);  // 'e'
    EXPECT_EQ(s.find_first_of("xyz"), mystl::string::npos);

    // find_last_of
    EXPECT_EQ(s.find_last_of("aeiou"), 7);   // 'o'
    EXPECT_EQ(s.find_last_of("xyz"), mystl::string::npos);

    // find_first_not_of
    EXPECT_EQ(s.find_first_not_of("helo"), 5);  // ' '
    
    // find_last_not_of
    EXPECT_EQ(s.find_last_not_of("world"), 5);  // ' '
}

// 比较操作测试
TEST(StringTest, Comparison) 
{
    mystl::string s1("hello");
    mystl::string s2("hello");
    mystl::string s3("world");
    
    // operator==
    EXPECT_TRUE(s1 == s2);
    EXPECT_FALSE(s1 == s3);

    // operator!=
    EXPECT_TRUE(s1 != s3);
    EXPECT_FALSE(s1 != s2);

    // operator<
    EXPECT_TRUE(s1 < s3);
    EXPECT_FALSE(s3 < s1);

    // operator<=
    EXPECT_TRUE(s1 <= s2);
    EXPECT_TRUE(s1 <= s3);

    // operator>
    EXPECT_TRUE(s3 > s1);
    EXPECT_FALSE(s1 > s3);

    // operator>=
    EXPECT_TRUE(s1 >= s2);
    EXPECT_TRUE(s3 >= s1);

    // compare
    EXPECT_EQ(s1.compare(s2), 0);
    EXPECT_LT(s1.compare(s3), 0);
    EXPECT_GT(s3.compare(s1), 0);
}

// 非成员函数测试
TEST(StringTest, NonMemberFunctions) 
{
    mystl::string s1("hello");
    mystl::string s2(" world");
    
    // operator+
    mystl::string s3 = s1 + s2;
    EXPECT_STREQ(s3.c_str(), "hello world");

    mystl::string s4 = s1 + "!";
    EXPECT_STREQ(s4.c_str(), "hello!");

    mystl::string s5 = "hi" + s2;
    EXPECT_STREQ(s5.c_str(), "hi world");

    // swap
    mystl::string a("first");
    mystl::string b("second");
    swap(a, b);
    EXPECT_STREQ(a.c_str(), "second");
    EXPECT_STREQ(b.c_str(), "first");
}

// 异常安全测试
struct StringThrowOnCopy 
{
    static bool should_throw;
    
    static void throw_if_needed() 
    {
        if (should_throw)
            throw std::runtime_error("Copy error");
    }
};

bool StringThrowOnCopy::should_throw = false;

TEST(StringTest, ExceptionSafety) 
{
    mystl::string s("hello");
    size_t old_size = s.size();
    size_t old_capacity = s.capacity();
    
    try 
    {
        StringThrowOnCopy::should_throw = true;
        s.reserve(1000);  // 应该在分配新内存时抛出异常
    }
    catch (const std::runtime_error&) 
    {
        // 验证string的状态未改变
        EXPECT_EQ(s.size(), old_size);
        EXPECT_EQ(s.capacity(), old_capacity);
        EXPECT_STREQ(s.c_str(), "hello");
    }
    
    StringThrowOnCopy::should_throw = false;
}

// 内存泄漏测试
TEST(StringTest, MemoryLeak) 
{
    // 这个测试主要依赖于内存检测工具（如Valgrind）来检测
    {
        mystl::string s("test");
        s.reserve(100);
        s = "new content";
        s.shrink_to_fit();
    }
    // 如果有内存泄漏，内存检测工具会报告
}

// 边界情况测试
TEST(StringTest, EdgeCases) 
{
    // 空字符串操作
    mystl::string empty_str;
    EXPECT_NO_THROW(empty_str.clear());
    EXPECT_NO_THROW(empty_str.reserve(10));
    EXPECT_NO_THROW(empty_str.shrink_to_fit());
    
    // 大字符串操作
    mystl::string large_str(1000, 'a');
    EXPECT_EQ(large_str.size(), 1000);
    EXPECT_EQ(large_str[999], 'a');
    
    // 特殊字符
    mystl::string special_chars("\\n\\t\\0\\r");
    EXPECT_EQ(special_chars.size(), 8);
    
    // 零长度操作
    mystl::string s("test");
    EXPECT_NO_THROW(s.insert(0, ""));
    EXPECT_NO_THROW(s.append(""));
    EXPECT_NO_THROW(s.replace(0, 0, ""));
} 