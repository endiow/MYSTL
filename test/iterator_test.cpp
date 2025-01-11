#include <gtest/gtest.h>
#include "mystl/iterator.hpp"
#include "mystl/vector.hpp"

// 测试迭代器特性萃取
TEST(IteratorTest, HasIteratorType) 
{
    EXPECT_TRUE(mystl::has_iterator_type<mystl::vector<int>::iterator>::value);
    EXPECT_FALSE(mystl::has_iterator_type<int>::value);
}

// 测试迭代器类别判断
TEST(IteratorTest, IteratorCategory) 
{
    // 输入迭代器
    EXPECT_TRUE((mystl::is_input_iterator<mystl::vector<int>::iterator>::value));
    EXPECT_TRUE((mystl::is_input_iterator<int*>::value));
    EXPECT_FALSE((mystl::is_input_iterator<int>::value));

    // 输出迭代器
    EXPECT_FALSE((mystl::is_output_iterator<mystl::vector<int>::iterator>::value));
    EXPECT_FALSE((mystl::is_output_iterator<int>::value));

    // 前向迭代器
    EXPECT_TRUE((mystl::is_forward_iterator<mystl::vector<int>::iterator>::value));
    EXPECT_FALSE((mystl::is_forward_iterator<int>::value));

    // 双向迭代器
    EXPECT_TRUE((mystl::is_bidirectional_iterator<mystl::vector<int>::iterator>::value));
    EXPECT_FALSE((mystl::is_bidirectional_iterator<int>::value));

    // 随机访问迭代器
    EXPECT_TRUE((mystl::is_random_access_iterator<mystl::vector<int>::iterator>::value));
    EXPECT_FALSE((mystl::is_random_access_iterator<int>::value));
}

// 测试 iterator_traits
TEST(IteratorTest, IteratorTraits) 
{
    using traits = mystl::iterator_traits<mystl::vector<int>::iterator>;
    using pointer_traits = mystl::iterator_traits<int*>;
    
    static_assert(std::is_same<traits::value_type, int>::value, "value_type error");
    static_assert(std::is_same<pointer_traits::value_type, int>::value, "pointer value_type error");
}

// 测试 advance
TEST(IteratorTest, Advance) 
{
    int arr[] = {1, 2, 3, 4, 5};
    mystl::vector<int> v(arr, arr + 5);
    mystl::vector<int> u{1,2,3,4,5};
    auto it = v.begin();
    
    // 前进
    mystl::advance(it, 2);
    EXPECT_EQ(*it, 3);
    
    // 后退
    mystl::advance(it, -1);
    EXPECT_EQ(*it, 2);
}

// 测试 distance
TEST(IteratorTest, Distance) 
{
    int arr[] = {1, 2, 3, 4, 5};
    mystl::vector<int> v(arr, arr + 5);
    
    // 随机访问迭代器
    auto dist = mystl::distance(v.begin(), v.end());
    EXPECT_EQ(dist, 5);
    
    // 正向距离
    dist = mystl::distance(v.begin(), v.begin() + 3);
    EXPECT_EQ(dist, 3);
    
    // 零距离
    dist = mystl::distance(v.begin(), v.begin());
    EXPECT_EQ(dist, 0);
}

// 测试反向迭代器
TEST(IteratorTest, ReverseIterator) 
{
    int arr[] = {1, 2, 3};
    mystl::vector<int> v(arr, arr + 3);
    mystl::reverse_iterator<mystl::vector<int>::iterator> rit(v.end());
    
    EXPECT_EQ(*rit, 3);
    ++rit;
    EXPECT_EQ(*rit, 2);
    ++rit;
    EXPECT_EQ(*rit, 1);
    
    // 测试 base()
    auto it = rit.base();
    EXPECT_EQ(*it, 2);
}

// 测试反向迭代器的算术运算
TEST(IteratorTest, ReverseIteratorArithmetic) 
{
    int arr[] = {1, 2, 3, 4, 5};
    mystl::vector<int> v(arr, arr + 5);
    mystl::reverse_iterator<mystl::vector<int>::iterator> rit(v.end());
    
    // operator+
    auto rit2 = rit + 2;
    EXPECT_EQ(*rit2, 3);
    
    // operator-
    auto rit3 = rit2 - 1;
    EXPECT_EQ(*rit3, 4);
    
    // operator[]
    EXPECT_EQ(rit[0], 5);
    EXPECT_EQ(rit[1], 4);
    
    // 迭代器之间的距离
    auto dist = rit2 - rit3;
    EXPECT_EQ(dist, 1);
} 