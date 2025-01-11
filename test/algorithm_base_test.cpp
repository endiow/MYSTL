#include <gtest/gtest.h>
#include "mystl/algorithm_base.hpp"
#include <vector>

TEST(AlgorithmBaseTest, MinMax) 
{
    EXPECT_EQ(mystl::min(3, 5), 3);
    EXPECT_EQ(mystl::max(3, 5), 5);
}

TEST(AlgorithmBaseTest, Equal) 
{
    std::vector<int> v1 = {1, 2, 3};
    std::vector<int> v2 = {1, 2, 3};
    std::vector<int> v3 = {1, 2, 4};
    
    EXPECT_TRUE(mystl::equal(v1.begin(), v1.end(), v2.begin()));
    EXPECT_FALSE(mystl::equal(v1.begin(), v1.end(), v3.begin()));
}

TEST(AlgorithmBaseTest, Mismatch) 
{
    std::vector<int> v1 = {1, 2, 3};
    std::vector<int> v2 = {1, 2, 4};
    
    auto result = mystl::mismatch(v1.begin(), v1.end(), v2.begin());
    EXPECT_EQ(*result.first, 3);
    EXPECT_EQ(*result.second, 4);
}

TEST(AlgorithmBaseTest, LexicographicalCompare) 
{
    std::vector<int> v1 = {1, 2, 3};
    std::vector<int> v2 = {1, 2, 4};
    std::vector<int> v3 = {1, 2};
    
    EXPECT_TRUE(mystl::lexicographical_compare(v1.begin(), v1.end(), v2.begin(), v2.end()));
    EXPECT_FALSE(mystl::lexicographical_compare(v2.begin(), v2.end(), v1.begin(), v1.end()));
    EXPECT_TRUE(mystl::lexicographical_compare(v3.begin(), v3.end(), v1.begin(), v1.end()));
}

TEST(AlgorithmBaseTest, Copy) 
{
    std::vector<int> src = {1, 2, 3};
    std::vector<int> dst(3);
    
    mystl::copy(src.begin(), src.end(), dst.begin());
    EXPECT_TRUE(mystl::equal(src.begin(), src.end(), dst.begin()));
}

TEST(AlgorithmBaseTest, CopyBackward) 
{
    std::vector<int> src = {1, 2, 3};
    std::vector<int> dst(3);
    
    mystl::copy_backward(src.begin(), src.end(), dst.end());
    EXPECT_TRUE(mystl::equal(src.begin(), src.end(), dst.begin()));
}

TEST(AlgorithmBaseTest, CopyIf) 
{
    std::vector<int> src = {1, 2, 3, 4, 5};
    std::vector<int> dst;
    
    mystl::copy_if(src.begin(), src.end(), std::back_inserter(dst), 
                   [](int x) { return x % 2 == 0; });
    EXPECT_EQ(dst.size(), 2);
    EXPECT_EQ(dst[0], 2);
    EXPECT_EQ(dst[1], 4);
}

TEST(AlgorithmBaseTest, Move) 
{
    std::vector<std::unique_ptr<int>> src;
    src.push_back(std::make_unique<int>(1));
    src.push_back(std::make_unique<int>(2));
    std::vector<std::unique_ptr<int>> dst(2);
    
    mystl::move(src.begin(), src.end(), dst.begin());
    EXPECT_EQ(*dst[0], 1);
    EXPECT_EQ(*dst[1], 2);
    EXPECT_EQ(src[0], nullptr);
    EXPECT_EQ(src[1], nullptr);
}

TEST(AlgorithmBaseTest, Fill) 
{
    std::vector<int> v(3);
    mystl::fill(v.begin(), v.end(), 42);
    EXPECT_EQ(v[0], 42);
    EXPECT_EQ(v[1], 42);
    EXPECT_EQ(v[2], 42);
}

TEST(AlgorithmBaseTest, FillN) 
{
    std::vector<int> v(3);
    mystl::fill_n(v.begin(), 2, 42);
    EXPECT_EQ(v[0], 42);
    EXPECT_EQ(v[1], 42);
    EXPECT_EQ(v[2], 0);
}

TEST(AlgorithmBaseTest, IterSwap) 
{
    int a = 1, b = 2;
    mystl::iter_swap(&a, &b);
    EXPECT_EQ(a, 2);
    EXPECT_EQ(b, 1);
} 