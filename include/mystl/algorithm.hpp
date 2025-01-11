#pragma once

#include "iterator.hpp"

namespace mystl 
{
    // 查找范围内第一个等于指定值的元素
    template<class InputIterator, class T>
    InputIterator find(InputIterator first, InputIterator last, const T& value) 
    {
        while (first != last && *first != value) 
        {
            ++first;
        }
        return first;
    }

    // 查找范围内第一个满足谓词的元素
    template<class InputIterator, class UnaryPredicate>
    InputIterator find_if(InputIterator first, InputIterator last, UnaryPredicate pred) 
    {
        while (first != last && !pred(*first)) 
        {
            ++first;
        }
        return first;
    }

    // 查找范围内第一个不满足谓词的元素
    template<class InputIterator, class UnaryPredicate>
    InputIterator find_if_not(InputIterator first, InputIterator last, UnaryPredicate pred) 
    {
        while (first != last && pred(*first)) 
        {
            ++first;
        }
        return first;
    }
} // namespace mystl 