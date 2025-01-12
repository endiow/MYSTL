#include <gtest/gtest.h>
#include "mystl/algorithm_base.hpp"
#include "mystl/vector.hpp"

// 测试判断算法
TEST(AlgorithmBaseTest, PredicateCheck)
{
    // 测试基本类型
    mystl::vector<int> v1{2, 4, 6, 8, 10};
    mystl::vector<int> v2{1, 2, 3, 4, 5};
    mystl::vector<int> v3{1, 3, 5, 7, 9};

    auto is_even = [](int n) { return n % 2 == 0; };
    auto is_positive = [](int n) { return n > 0; };
    auto is_negative = [](int n) { return n < 0; };

    // 测试 all_of
    EXPECT_TRUE(mystl::all_of(v1.begin(), v1.end(), is_even));   // 全是偶数
    EXPECT_FALSE(mystl::all_of(v2.begin(), v2.end(), is_even));  // 部分偶数
    EXPECT_FALSE(mystl::all_of(v3.begin(), v3.end(), is_even));  // 没有偶数
    EXPECT_TRUE(mystl::all_of(v1.begin(), v1.end(), is_positive));  // 全是正数

    // 测试 any_of
    EXPECT_TRUE(mystl::any_of(v1.begin(), v1.end(), is_even));   // 全是偶数
    EXPECT_TRUE(mystl::any_of(v2.begin(), v2.end(), is_even));   // 部分偶数
    EXPECT_FALSE(mystl::any_of(v3.begin(), v3.end(), is_even));  // 没有偶数
    EXPECT_FALSE(mystl::any_of(v1.begin(), v1.end(), is_negative));  // 没有负数

    // 测试 none_of
    EXPECT_FALSE(mystl::none_of(v1.begin(), v1.end(), is_even));  // 全是偶数
    EXPECT_FALSE(mystl::none_of(v2.begin(), v2.end(), is_even));  // 部分偶数
    EXPECT_TRUE(mystl::none_of(v3.begin(), v3.end(), is_even));   // 没有偶数
    EXPECT_TRUE(mystl::none_of(v1.begin(), v1.end(), is_negative));  // 没有负数

    // 测试自定义类型
    struct Person 
    {
        std::string name;
        int age;
        double salary;
        bool is_student;

        Person(std::string n, int a, double s, bool student = false)
            : name(n), age(a), salary(s), is_student(student) {}
    };

    mystl::vector<Person> people{
        {"Alice", 30, 5000.0},
        {"Bob", 25, 3000.0, true},
        {"Charlie", 35, 6000.0},
        {"David", 20, 0.0, true},
        {"Eve", 28, 4000.0}
    };

    auto is_adult = [](const Person& p) { return p.age >= 18; };
    auto is_student = [](const Person& p) { return p.is_student; };
    auto has_high_salary = [](const Person& p) { return p.salary >= 5000.0; };
    auto is_teenager = [](const Person& p) { return p.age >= 13 && p.age <= 19; };

    EXPECT_TRUE(mystl::all_of(people.begin(), people.end(), is_adult));  // 都是成年人
    EXPECT_FALSE(mystl::all_of(people.begin(), people.end(), is_student));  // 不是都是学生
    EXPECT_FALSE(mystl::all_of(people.begin(), people.end(), has_high_salary));  // 不是都是高薪

    EXPECT_TRUE(mystl::any_of(people.begin(), people.end(), is_student));  // 有学生
    EXPECT_TRUE(mystl::any_of(people.begin(), people.end(), has_high_salary));  // 有高薪
    EXPECT_FALSE(mystl::any_of(people.begin(), people.end(), is_teenager));  // 没有青少年

    EXPECT_FALSE(mystl::none_of(people.begin(), people.end(), is_student));  // 有学生
    EXPECT_FALSE(mystl::none_of(people.begin(), people.end(), has_high_salary));  // 有高薪
    EXPECT_TRUE(mystl::none_of(people.begin(), people.end(), is_teenager));  // 没有青少年

    // 测试边界情况
    mystl::vector<int> empty;
    mystl::vector<int> single{1};
    mystl::vector<int> all_zero{0, 0, 0};
    mystl::vector<int> with_negative{1, -2, 3, -4, 5};

    EXPECT_TRUE(mystl::all_of(empty.begin(), empty.end(), is_even));  // 空序列
    EXPECT_FALSE(mystl::any_of(empty.begin(), empty.end(), is_even));
    EXPECT_TRUE(mystl::none_of(empty.begin(), empty.end(), is_even));

    EXPECT_FALSE(mystl::all_of(single.begin(), single.end(), is_even));  // 单个元素
    EXPECT_FALSE(mystl::any_of(single.begin(), single.end(), is_even));
    EXPECT_TRUE(mystl::none_of(single.begin(), single.end(), is_even));

    EXPECT_TRUE(mystl::all_of(all_zero.begin(), all_zero.end(), is_even));  // 全0序列
    EXPECT_TRUE(mystl::any_of(all_zero.begin(), all_zero.end(), is_even));
    EXPECT_FALSE(mystl::none_of(all_zero.begin(), all_zero.end(), is_even));

    EXPECT_TRUE(mystl::any_of(with_negative.begin(), with_negative.end(), is_negative));  // 包含负数
    EXPECT_FALSE(mystl::all_of(with_negative.begin(), with_negative.end(), is_positive));
    EXPECT_FALSE(mystl::none_of(with_negative.begin(), with_negative.end(), is_negative));
}

// 测试比较算法
TEST(AlgorithmBaseTest, CompareOperations)
{
    // 测试 min/max
    EXPECT_EQ(mystl::min(3, 5), 3);
    EXPECT_EQ(mystl::max(3, 5), 5);
    EXPECT_EQ(mystl::min(5, 5), 5);  // 相等时返回第一个参数
    EXPECT_EQ(mystl::max(5, 5), 5);

    auto comp = std::greater<int>();
    EXPECT_EQ(mystl::min(3, 5, comp), 5);  // 使用自定义比较器
    EXPECT_EQ(mystl::max(3, 5, comp), 3);

    // 测试 equal
    mystl::vector<int> v1{1, 2, 3, 4, 5};
    mystl::vector<int> v2{1, 2, 3, 4, 5};
    mystl::vector<int> v3{1, 2, 3, 4, 6};
    mystl::vector<int> v4{1, 2, 3};

    EXPECT_TRUE(mystl::equal(v1.begin(), v1.end(), v2.begin()));
    EXPECT_FALSE(mystl::equal(v1.begin(), v1.end(), v3.begin()));
    
    auto equal_mod_2 = [](int a, int b) { return a % 2 == b % 2; };
    EXPECT_TRUE(mystl::equal(v1.begin(), v1.end(), v2.begin(), equal_mod_2));

    // 测试 mismatch
    auto p1 = mystl::mismatch(v1.begin(), v1.end(), v3.begin());
    EXPECT_EQ(p1.first, v1.begin() + 4);
    EXPECT_EQ(p1.second, v3.begin() + 4);

    auto p2 = mystl::mismatch(v1.begin(), v1.end(), v4.begin());
    EXPECT_EQ(p2.first, v1.begin() + 3);
    EXPECT_EQ(p2.second, v4.begin() + 3);

    // 测试 lexicographical_compare
    EXPECT_FALSE(mystl::lexicographical_compare(v1.begin(), v1.end(), v2.begin(), v2.end()));
    EXPECT_TRUE(mystl::lexicographical_compare(v1.begin(), v1.end(), v3.begin(), v3.end()));
    EXPECT_FALSE(mystl::lexicographical_compare(v1.begin(), v1.end(), v4.begin(), v4.end()));
    EXPECT_TRUE(mystl::lexicographical_compare(v4.begin(), v4.end(), v1.begin(), v1.end()));

    // 测试自定义类型
    struct Point {
        int x, y;
        bool operator==(const Point& other) const { return x == other.x && y == other.y; }
        bool operator<(const Point& other) const { return x < other.x || (x == other.x && y < other.y); }
    };

    mystl::vector<Point> points1{{1, 2}, {3, 4}, {5, 6}};
    mystl::vector<Point> points2{{1, 2}, {3, 4}, {5, 6}};
    mystl::vector<Point> points3{{1, 2}, {3, 4}, {5, 7}};

    EXPECT_TRUE(mystl::equal(points1.begin(), points1.end(), points2.begin()));
    EXPECT_FALSE(mystl::equal(points1.begin(), points1.end(), points3.begin()));

    auto manhattan_less = [](const Point& a, const Point& b) {
        return (a.x + a.y) < (b.x + b.y);
    };
    EXPECT_TRUE(mystl::lexicographical_compare(points1.begin(), points1.end(), 
                                             points3.begin(), points3.end(), manhattan_less));
}

// 测试复制和移动算法
TEST(AlgorithmBaseTest, CopyAndMove)
{
    // 测试 copy
    mystl::vector<int> src{1, 2, 3, 4, 5};
    mystl::vector<int> dest(5);
    mystl::copy(src.begin(), src.end(), dest.begin());
    EXPECT_EQ(dest, src);

    // 测试 copy_backward
    mystl::vector<int> dest2(5);
    mystl::copy_backward(src.begin(), src.end(), dest2.end());
    EXPECT_EQ(dest2, src);

    // 测试 copy_if
    mystl::vector<int> even_nums;
    mystl::copy_if(src.begin(), src.end(), mystl::back_inserter(even_nums),
                  [](int n) { return n % 2 == 0; });
    EXPECT_EQ(even_nums, (mystl::vector<int>{2, 4}));

    // 测试 copy_n
    mystl::vector<int> dest3(3);
    mystl::copy_n(src.begin(), 3, dest3.begin());
    EXPECT_EQ(dest3, (mystl::vector<int>{1, 2, 3}));

    // 测试 move
    mystl::vector<std::string> src_str{"hello", "world", "!"};
    mystl::vector<std::string> dest_str(3);
    mystl::move(src_str.begin(), src_str.end(), dest_str.begin());
    EXPECT_EQ(dest_str[0], "hello");
    EXPECT_EQ(dest_str[1], "world");
    EXPECT_EQ(dest_str[2], "!");

    // 测试 move_backward
    mystl::vector<std::string> src_str2{"hello", "world", "!"};
    mystl::vector<std::string> dest_str2(3);
    mystl::move_backward(src_str2.begin(), src_str2.end(), dest_str2.end());
    EXPECT_EQ(dest_str2[0], "hello");
    EXPECT_EQ(dest_str2[1], "world");
    EXPECT_EQ(dest_str2[2], "!");
}

// 测试填充算法
TEST(AlgorithmBaseTest, Fill)
{
    // 测试 fill
    mystl::vector<int> v(5);
    mystl::fill(v.begin(), v.end(), 42);
    EXPECT_TRUE(mystl::all_of(v.begin(), v.end(), [](int n) { return n == 42; }));

    // 测试 fill_n
    mystl::vector<int> v2(5);
    mystl::fill_n(v2.begin(), 3, 42);  // 只填充前3个元素
    EXPECT_EQ(v2[0], 42);
    EXPECT_EQ(v2[1], 42);
    EXPECT_EQ(v2[2], 42);
    EXPECT_EQ(v2[3], 0);
    EXPECT_EQ(v2[4], 0);

    // 测试不同类型
    mystl::vector<std::string> v3(3);
    mystl::fill(v3.begin(), v3.end(), "test");
    EXPECT_TRUE(mystl::all_of(v3.begin(), v3.end(), [](const std::string& s) { return s == "test"; }));

    // 测试单字节类型
    mystl::vector<char> v4(5);
    mystl::fill(v4.begin(), v4.end(), 'x');
    EXPECT_TRUE(mystl::all_of(v4.begin(), v4.end(), [](char c) { return c == 'x'; }));
}

// 测试其他算法
TEST(AlgorithmBaseTest, Miscellaneous)
{
    // 测试 iter_swap
    mystl::vector<int> v{1, 2, 3, 4, 5};
    mystl::iter_swap(v.begin(), v.begin() + 4);
    EXPECT_EQ(v[0], 5);
    EXPECT_EQ(v[4], 1);

    // 测试自定义类型的 iter_swap
    struct Point {
        int x, y;
        Point(int x = 0, int y = 0) : x(x), y(y) {}
    };

    mystl::vector<Point> points{{1, 2}, {3, 4}};
    mystl::iter_swap(points.begin(), points.begin() + 1);
    EXPECT_EQ(points[0].x, 3);
    EXPECT_EQ(points[0].y, 4);
    EXPECT_EQ(points[1].x, 1);
    EXPECT_EQ(points[1].y, 2);
} 