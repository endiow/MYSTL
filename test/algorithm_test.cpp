#include <gtest/gtest.h>
#include "mystl/algorithm.hpp"
#include "mystl/vector.hpp"
#include <random>
#include <algorithm>

namespace 
{
    // 生成随机数据的辅助函数
    template<class T>
    mystl::vector<T> generate_random_data(size_t size, T min_val, T max_val) 
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<T> dis(min_val, max_val);

        mystl::vector<T> data;
        for (size_t i = 0; i < size; ++i)
        {
            data.push_back(dis(gen));
        }
        return data;
    }

    // 检查序列是否有序
    template<class Iterator>
    bool is_sorted(Iterator first, Iterator last) 
    {
        if (first == last) return true;
        Iterator next = first;
        while (++next != last) 
        {
            if (*next < *first)
                return false;
            first = next;
        }
        return true;
    }
}

// 基本查找测试
TEST(AlgorithmTest, Find) 
{
    mystl::vector<int> vec{1, 2, 3, 4, 5};
    
    // 测试 find
    {
        // 查找存在的元素
        auto it1 = mystl::find(vec.begin(), vec.end(), 3);
        EXPECT_NE(it1, vec.end());
        EXPECT_EQ(*it1, 3);
        
        // 查找不存在的元素
        auto it2 = mystl::find(vec.begin(), vec.end(), 6);
        EXPECT_EQ(it2, vec.end());
    }

    // 测试 find_if
    {
        // 查找第一个大于3的元素
        auto it = mystl::find_if(vec.begin(), vec.end(), 
            [](int x) { return x > 3; });
        EXPECT_NE(it, vec.end());
        EXPECT_EQ(*it, 4);
        
        // 查找不存在的条件
        auto it2 = mystl::find_if(vec.begin(), vec.end(),
            [](int x) { return x > 10; });
        EXPECT_EQ(it2, vec.end());
    }

    // 测试 find_if_not
    {
        // 查找第一个不小于3的元素
        auto it = mystl::find_if_not(vec.begin(), vec.end(),
            [](int x) { return x < 3; });
        EXPECT_NE(it, vec.end());
        EXPECT_EQ(*it, 3);
        
        // 所有元素都满足条件的情况
        auto it2 = mystl::find_if_not(vec.begin(), vec.end(),
            [](int x) { return x < 10; });
        EXPECT_EQ(it2, vec.end());
    }
}

// 自定义类型测试
TEST(AlgorithmTest, FindCustomType) 
{
    struct Point {
        int x, y;
        bool operator==(const Point& other) const {
            return x == other.x && y == other.y;
        }
        bool operator!=(const Point& other) const {
            return !(*this == other);
        }
    };

    mystl::vector<Point> points{{1,1}, {2,2}, {3,3}, {4,4}};
    
    // 查找特定点
    auto it = mystl::find(points.begin(), points.end(), Point{2,2});
    EXPECT_NE(it, points.end());
    EXPECT_EQ(it->x, 2);
    EXPECT_EQ(it->y, 2);

    // 使用谓词查找
    auto it2 = mystl::find_if(points.begin(), points.end(),
        [](const Point& p) { return p.x + p.y > 5; });
    EXPECT_NE(it2, points.end());
    EXPECT_EQ(it2->x, 3);
} 