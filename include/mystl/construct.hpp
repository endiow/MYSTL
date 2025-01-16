#pragma once

#include <new>  // for placement new
#include "memory.hpp"
#include "iterator.hpp"

namespace mystl 
{
    //------------------------------------------------------------------------------        
    // construct 在已分配的内存上构造对象
    //------------------------------------------------------------------------------        
    // 无参数版本
    template <class T>
    void construct(T* ptr) 
    {
        ::new ((void*)ptr) T();
    }

    // 一个参数版本
    template <class T1, class T2>
    void construct(T1* ptr, const T2& value) 
    {
        ::new ((void*)ptr) T1(value);
    }

    // 可变参数版本，支持多参数构造
    template <class T, class... Args>
    void construct(T* ptr, Args&&... args) 
    {
        ::new ((void*)ptr) T(mystl::forward<Args>(args)...);
    }

    // 迭代器版本的construct
    template <class Iterator>
    void construct(Iterator pos) 
    {
        construct(mystl::addressof(*pos));
    }

    template <class Iterator, class T>
    void construct(Iterator pos, const T& value)
    {
        construct(mystl::addressof(*pos), value);
    }

    template <class Iterator, class... Args>
    void construct(Iterator pos, Args&&... args)
    {
        construct(mystl::addressof(*pos), mystl::forward<Args>(args)...);
    }

    //------------------------------------------------------------------------------        
    // destroy 销毁对象但不释放内存
    //------------------------------------------------------------------------------        
    // 销毁单个对象
    template <class T>
    void destroy_at(T* ptr) 
    {
        if (ptr != nullptr) 
        {
            ptr->~T();
        }
    }

    // destroy 的别名
    template <class T>
    void destroy(T* ptr) 
    {
        destroy_at(ptr);
    }

    // 销毁一个范围内的对象
    template <class ForwardIt>
    void destroy(ForwardIt first, ForwardIt last) 
    {
        for (; first != last; ++first) 
        {
            destroy_at(mystl::addressof(*first));
        }
    }

    // 销毁n个对象
    template <class ForwardIt, class Size>
    ForwardIt destroy_n(ForwardIt first, Size n) 
    {
        for (; n > 0; ++first, --n) 
        {
            destroy_at(mystl::addressof(*first));
        }
        return first;
    }

    // 对于有平凡析构函数的类型，destroy 可以是空操作
    template <class ForwardIt>
    inline void destroy(ForwardIt, ForwardIt, typename enable_if<
        is_trivially_destructible<
            typename iterator_traits<ForwardIt>::value_type>::value>::type* = 0) {}

    template <class ForwardIt, class Size>
    inline ForwardIt destroy_n(ForwardIt first, Size, typename enable_if<
        is_trivially_destructible<
            typename iterator_traits<ForwardIt>::value_type>::value>::type* = 0) 
    {
        return first;
    }

} // namespace mystl 