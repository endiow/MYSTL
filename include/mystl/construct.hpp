#pragma once

#include <new>  // for placement new
#include "memory.hpp"
#include "iterator.hpp"

namespace mystl 
{
    //------------------------------------------------------------------------------        
    // construct 在已分配的内存上构造对象
    //------------------------------------------------------------------------------        
    
    // 指针版本 - 无参数
    template <class T>
    void construct(T* ptr) 
    {
        ::new ((void*)ptr) T();
    }

    // 可解引用类型版本 - 无参数
    template <class T>
    typename enable_if<!is_pointer<T>::value && is_dereferenceable<T>::value>::type
    construct(T pos) 
    {
        construct(mystl::addressof(*pos));
    }

    // 带值的指针版本
    template <class T1, class T2>
    void construct(T1* ptr, const T2& value) 
    {
        ::new ((void*)ptr) T1(value);
    }

    // 带值的可解引用类型版本
    template <class T, class U>
    typename enable_if<!is_pointer<T>::value && is_dereferenceable<T>::value>::type
    construct(T pos, const U& value)
    {
        construct(mystl::addressof(*pos), value);
    }

    // 可变参数的指针版本
    template <class T, class... Args>
    void construct(T* ptr, Args&&... args) 
    {
        ::new ((void*)ptr) T(mystl::forward<Args>(args)...);
    }

    // 可变参数的可解引用类型版本
    template <class T, class... Args>
    typename enable_if<!is_pointer<T>::value && is_dereferenceable<T>::value>::type
    construct(T pos, Args&&... args)
    {
        construct(mystl::addressof(*pos), mystl::forward<Args>(args)...);
    }



    //------------------------------------------------------------------------------        
    // destroy 销毁对象但不释放内存
    //------------------------------------------------------------------------------        
    
    // 销毁单个对象 - 指针版本
    template <class T>
    void destroy_at(T* ptr) noexcept
    {
        if (ptr) ptr->~T();
    }

    // 销毁单个对象 - 可解引用类型版本
    template <class T>
    typename enable_if<!is_pointer<T>::value && is_dereferenceable<T>::value>::type
    destroy_at(T pos) noexcept
    {
        destroy_at(mystl::addressof(*pos));
    }

    // 销毁范围内的对象 - 指针版本
    template <class T>
    void destroy(T* first, T* last) noexcept
    {
        for (; first != last; ++first)
        {
            destroy_at(first);
        }
    }

    // 销毁范围内的对象 - 可解引用类型版本
    template <class T>
    typename enable_if<!is_pointer<T>::value && is_dereferenceable<T>::value>::type
    destroy(T first, T last) noexcept
    {
        for (; first != last; ++first)
        {
            destroy_at(mystl::addressof(*first));
        }
    }

    // 销毁n个对象 - 指针版本
    template <class T>
    T* destroy_n(T* first, size_t n) noexcept
    {
        for (size_t i = 0; i < n; ++i)
        {
            destroy_at(first + i);
        }
        return first + n;
    }

    // 销毁n个对象 - 可解引用类型版本
    template <class T>
    typename enable_if<!is_pointer<T>::value && is_dereferenceable<T>::value, T>::type
    destroy_n(T first, size_t n) noexcept
    {
        for (size_t i = 0; i < n; ++i, ++first)
        {
            destroy_at(mystl::addressof(*first));
        }
        return first;
    }

    // 平凡类型的特化
    template <class T>
    inline typename enable_if<is_trivially_destructible<T>::value>::type
    destroy(T*, T*) noexcept {}

    template <class T>
    inline typename enable_if<is_trivially_destructible<T>::value, T*>::type
    destroy_n(T* first, size_t) noexcept
    {
        return first;
    }



    

} // namespace mystl 