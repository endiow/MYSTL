#include <gtest/gtest.h>
#include "mystl/iterator.hpp"
#include "mystl/vector.hpp"
#include <list>

// 测试迭代器特性
TEST(IteratorTest, IteratorTraits) 
{
    // 测试指针的迭代器特性
    using ptr_traits = mystl::iterator_traits<int*>;
    static_assert(std::is_same_v<ptr_traits::iterator_category, mystl::random_access_iterator_tag>);
    static_assert(std::is_same_v<ptr_traits::value_type, int>);
    static_assert(std::is_same_v<ptr_traits::difference_type, std::ptrdiff_t>);
    static_assert(std::is_same_v<ptr_traits::pointer, int*>);
    static_assert(std::is_same_v<ptr_traits::reference, int&>);

    // 测试const指针的迭代器特性
    using const_ptr_traits = mystl::iterator_traits<const int*>;
    static_assert(std::is_same_v<const_ptr_traits::value_type, int>);
    static_assert(std::is_same_v<const_ptr_traits::reference, const int&>);

    // 测试vector迭代器的特性
    using vector_iter_traits = mystl::iterator_traits<mystl::vector<int>::iterator>;
    static_assert(std::is_same_v<vector_iter_traits::iterator_category, mystl::random_access_iterator_tag>);
    static_assert(std::is_same_v<vector_iter_traits::value_type, int>);
}

// 测试迭代器操作
TEST(IteratorTest, IteratorOperations) 
{
    mystl::vector<int> vec{1, 2, 3, 4, 5};
    
    // 测试 advance
    auto it = vec.begin();
    mystl::advance(it, 2);
    EXPECT_EQ(*it, 3);
    
    // 测试 distance
    auto dist = mystl::distance(vec.begin(), vec.end());
    EXPECT_EQ(dist, 5);
    
    // 测试 next
    auto next_it = mystl::next(vec.begin(), 2);
    EXPECT_EQ(*next_it, 3);
    
    // 测试 prev
    auto prev_it = mystl::prev(vec.end(), 2);
    EXPECT_EQ(*prev_it, 4);
}

// 测试不同类型的迭代器
TEST(IteratorTest, DifferentIterators) 
{
    // 测试输入迭代器
    struct InputIterator 
    {
        using iterator_category = mystl::input_iterator_tag;
        using value_type = int;
        using difference_type = std::ptrdiff_t;
        using pointer = int*;
        using reference = int&;
    };
    static_assert(mystl::is_input_iterator<InputIterator>::value);

    // 测试输出迭代器
    struct OutputIterator 
    {
        using iterator_category = mystl::output_iterator_tag;
        using value_type = void;
        using difference_type = void;
        using pointer = void;
        using reference = void;
    };
    static_assert(mystl::is_output_iterator<OutputIterator>::value);

    // 测试前向迭代器
    struct ForwardIterator 
    {
        using iterator_category = mystl::forward_iterator_tag;
        using value_type = int;
        using difference_type = std::ptrdiff_t;
        using pointer = int*;
        using reference = int&;
    };
    static_assert(mystl::is_forward_iterator<ForwardIterator>::value);
    static_assert(mystl::is_input_iterator<ForwardIterator>::value);

    // 测试双向迭代器
    struct BidirectionalIterator 
    {
        using iterator_category = mystl::bidirectional_iterator_tag;
        using value_type = int;
        using difference_type = std::ptrdiff_t;
        using pointer = int*;
        using reference = int&;
    };
    static_assert(mystl::is_bidirectional_iterator<BidirectionalIterator>::value);
    static_assert(mystl::is_forward_iterator<BidirectionalIterator>::value);

    // 测试随机访问迭代器
    struct RandomAccessIterator 
    {
        using iterator_category = mystl::random_access_iterator_tag;
        using value_type = int;
        using difference_type = std::ptrdiff_t;
        using pointer = int*;
        using reference = int&;
    };
    static_assert(mystl::is_random_access_iterator<RandomAccessIterator>::value);
    static_assert(mystl::is_bidirectional_iterator<RandomAccessIterator>::value);
}

// 测试迭代器适配器
TEST(IteratorTest, IteratorAdapters) 
{
    // 测试反向迭代器
    mystl::vector<int> vec{1, 2, 3, 4, 5};
    mystl::reverse_iterator<mystl::vector<int>::iterator> rit(vec.end());
    
    EXPECT_EQ(*rit, 5);
    ++rit;
    EXPECT_EQ(*rit, 4);
    
    // 测试反向迭代器的基础迭代器
    auto base_it = rit.base();
    EXPECT_EQ(*base_it, 5);

    // 测试反向迭代器的算术运算
    auto rit2 = rit + 2;
    EXPECT_EQ(*rit2, 2);
    
    // 测试反向迭代器的比较运算
    EXPECT_TRUE(rit < rit2);
    EXPECT_FALSE(rit > rit2);
}

// 测试迭代器辅助函数
TEST(IteratorTest, IteratorHelpers) 
{
    mystl::vector<int> vec{1, 2, 3, 4, 5};
    
    // 测试 iterator_base
    auto raw_ptr = mystl::iterator_base(vec.begin());
    EXPECT_EQ(*raw_ptr, 1);
    
    // 测试迭代器移动
    auto it = vec.begin();
    mystl::advance(it, 3);
    EXPECT_EQ(*it, 4);
    
    // 测试迭代器距离
    auto dist = mystl::distance(vec.begin(), it);
    EXPECT_EQ(dist, 3);
}

// 测试边界情况
TEST(IteratorTest, EdgeCases) 
{
    mystl::vector<int> empty;
    
    // 空容器的迭代器
    EXPECT_EQ(empty.begin(), empty.end());
    
    // 测试distance在空范围上的行为
    EXPECT_EQ(mystl::distance(empty.begin(), empty.end()), 0);
    
    // 单元素容器
    mystl::vector<int> single{1};
    auto it = single.begin();
    
    // 测试advance不超过范围
    mystl::advance(it, 0);
    EXPECT_EQ(it, single.begin());
    mystl::advance(it, 1);
    EXPECT_EQ(it, single.end());
}

// 测试自定义类型
TEST(IteratorTest, CustomTypes) 
{
    struct CustomType 
    {
        int value;
        CustomType(int v) : value(v) {}
        bool operator==(const CustomType& other) const 
        {
            return value == other.value;
        }
    };

    mystl::vector<CustomType> vec{{1}, {2}, {3}};
    
    // 测试迭代器访问自定义类型
    auto it = vec.begin();
    EXPECT_EQ(it->value, 1);
    
    // 测试迭代器移动
    mystl::advance(it, 2);
    EXPECT_EQ(it->value, 3);
} 