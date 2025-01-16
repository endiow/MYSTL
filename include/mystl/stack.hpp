#pragma once
#include "deque.hpp"

namespace mystl 
{

template <class T, class Container = mystl::deque<T>>
class stack 
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
    stack() = default;
    explicit stack(const Container& cont) : c(cont) {}
    explicit stack(Container&& cont) : c(mystl::move(cont)) {}
    
    // 基本操作
    bool empty() const { return c.empty(); }
    size_type size() const { return c.size(); }
    reference top() { return c.back(); }
    const_reference top() const { return c.back(); }
    void push(const value_type& value) { c.push_back(value); }
    void push(value_type&& value) { c.push_back(mystl::move(value)); }
    void pop() { c.pop_back(); }
    
    // 比较操作
    bool operator==(const stack& rhs) const { return c == rhs.c; }
    bool operator!=(const stack& rhs) const { return c != rhs.c; }
    bool operator<(const stack& rhs) const { return c < rhs.c; }
    bool operator>(const stack& rhs) const { return c > rhs.c; }
    bool operator<=(const stack& rhs) const { return c <= rhs.c; }
    bool operator>=(const stack& rhs) const { return c >= rhs.c; }
};

} // namespace mystl 