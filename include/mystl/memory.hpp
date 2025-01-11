#pragma once

#include "iterator.hpp"
#include "util.hpp"

namespace mystl 
{
    // addressof - 获取对象的实际地址
    template<class T>
    T* addressof(T& arg) noexcept 
    {
        return reinterpret_cast<T*>(
            &const_cast<char&>(
                reinterpret_cast<const volatile char&>(arg)
            )
        );
    }
} // namespace mystl
