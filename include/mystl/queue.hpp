#pragma once
#include "deque.hpp"

namespace mystl 
{

    // 普通队列
    template <class T, class Container = mystl::deque<T>>
    class queue 
    {
    public:
        using container_type = Container;
        using value_type = typename Container::value_type;
        using size_type = typename Container::size_type;
        using reference = typename Container::reference;
        using const_reference = typename Container::const_reference;

    protected:
        Container c;  // 底层容器

    public:
        // 构造函数
        queue() = default;
        explicit queue(const Container& cont) : c(cont) {}
        explicit queue(Container&& cont) : c(mystl::move(cont)) {}
        
        // 基本操作
        bool empty() const { return c.empty(); }
        size_type size() const { return c.size(); }
        reference front() { return c.front(); }
        const_reference front() const { return c.front(); }
        reference back() { return c.back(); }
        const_reference back() const { return c.back(); }
        void push(const value_type& value) { c.push_back(value); }
        void push(value_type&& value) { c.push_back(mystl::move(value)); }
        void pop() { c.pop_front(); }
        
        // 比较操作
        bool operator==(const queue& rhs) const { return c == rhs.c; }
        bool operator!=(const queue& rhs) const { return c != rhs.c; }
        bool operator<(const queue& rhs) const { return c < rhs.c; }
        bool operator>(const queue& rhs) const { return c > rhs.c; }
        bool operator<=(const queue& rhs) const { return c <= rhs.c; }
        bool operator>=(const queue& rhs) const { return c >= rhs.c; }
    };

    // 优先队列
    template <class T, class Container = mystl::deque<T>, 
            class Compare = mystl::less<typename Container::value_type>>
    class priority_queue 
    {
    public:
        using container_type = Container;
        using value_compare = Compare;
        using value_type = typename Container::value_type;
        using size_type = typename Container::size_type;
        using reference = typename Container::reference;
        using const_reference = typename Container::const_reference;

    protected:
        Container c;     // 底层容器
        Compare comp;    // 比较函数对象

        // 上溯操作
        void sift_up(size_type pos)
        {
            value_type temp = mystl::move(c[pos]);
            while (pos > 0)
            {
                size_type parent = (pos - 1) / 2;
                if (!comp(c[parent], temp))
                    break;
                c[pos] = mystl::move(c[parent]);
                pos = parent;
            }
            c[pos] = mystl::move(temp);
        }

        // 下溯操作
        void sift_down(size_type pos)
        {
            value_type temp = mystl::move(c[pos]);
            size_type child;
            while ((child = 2 * pos + 1) < c.size())
            {
                if (child + 1 < c.size() && comp(c[child], c[child + 1]))
                    ++child;
                if (!comp(temp, c[child]))
                    break;
                c[pos] = mystl::move(c[child]);
                pos = child;
            }
            c[pos] = mystl::move(temp);
        }

    public:
        // 构造函数
        priority_queue() = default;
        explicit priority_queue(const Compare& compare) : comp(compare) {}
        priority_queue(const Compare& compare, const Container& cont)
            : c(cont), comp(compare)
        {
            // 建堆
            for (int i = c.size()/2 - 1; i >= 0; --i)
                sift_down(i);
        }
        
        // 基本操作
        bool empty() const { return c.empty(); }
        size_type size() const { return c.size(); }
        const_reference top() const { return c.front(); }
        
        void push(const value_type& value)
        {
            c.push_back(value);
            sift_up(c.size() - 1);
        }
        
        void push(value_type&& value)
        {
            c.push_back(mystl::move(value));
            sift_up(c.size() - 1);
        }
        
        void pop()
        {
            c[0] = mystl::move(c.back());
            c.pop_back();
            if (!c.empty())
                sift_down(0);
        }
    };

} // namespace mystl 