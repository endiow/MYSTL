#pragma once

#include "type_traits.hpp"
#include "algorithm_base.hpp"
#include "memory.hpp"
#include "construct.hpp"

namespace mystl 
{
    /*****************************************************************************************/
    // uninitialized_copy
    // 在未初始化内存空间上复制一个序列
    /*****************************************************************************************/
    template<class InputIt, class ForwardIt>
    ForwardIt 
    uninitialized_copy_aux(InputIt first, InputIt last, ForwardIt d_first, true_type) 
    {
        return mystl::copy(first, last, d_first);
    }

    // 对不可平凡赋值类型的实现
    template<class InputIt, class ForwardIt>
    ForwardIt 
    uninitialized_copy_aux(InputIt first, InputIt last, ForwardIt d_first, false_type) 
    {
        ForwardIt current = d_first;
        try 
        {
            for (; first != last; ++first, ++current)
            {
                mystl::construct(addressof(*current), *first);
            }
            return current;
        }
        catch (...) 
        {
            destroy(d_first, current);
            throw;
        }
    }

    // uninitialized_copy 的分派函数
    template<class InputIt, class ForwardIt>
    ForwardIt uninitialized_copy(InputIt first, InputIt last, ForwardIt d_first) 
    {
        using Value = typename iterator_traits<ForwardIt>::value_type;
        return uninitialized_copy_aux(first, last, d_first, is_trivially_copy_assignable<Value>{});
    }



    /*****************************************************************************************/
    // uninitialized_copy_n
    // 在未初始化内存空间上复制 n 个元素
    /*****************************************************************************************/
    template<class InputIt, class Size, class ForwardIt>
    ForwardIt 
    uninitialized_copy_n_aux(InputIt first, Size n, ForwardIt d_first, true_type) 
    {
        return mystl::copy(first, first + n, d_first);  // 可平凡赋值类型直接使用copy
    }

    template<class InputIt, class Size, class ForwardIt>
    ForwardIt 
    uninitialized_copy_n_aux(InputIt first, Size n, ForwardIt d_first, false_type) 
    {
        ForwardIt current = d_first;
        try 
        {
            for (; n > 0; --n, ++first, ++current)
            {
                mystl::construct(addressof(*current), *first);
            }
            return current;
        }
        catch (...) 
        {
            destroy(d_first, current);
            throw;
        }
    }

    template<class InputIt, class Size, class ForwardIt>
    ForwardIt uninitialized_copy_n(InputIt first, Size n, ForwardIt d_first) 
    {
        using Value = typename iterator_traits<ForwardIt>::value_type;
        return uninitialized_copy_n_aux(first, n, d_first, is_trivially_copy_assignable<Value>{});
    }



    /*****************************************************************************************/
    // uninitialized_move
    // 在未初始化内存空间上移动一个序列
    /*****************************************************************************************/
    template<class InputIt, class ForwardIt>
    ForwardIt 
    uninitialized_move_aux(InputIt first, InputIt last, ForwardIt d_first, true_type) 
    {
        return mystl::move(first, last, d_first);
    }

    // 对不可平凡赋值类型的实现
    template<class InputIt, class ForwardIt>
    ForwardIt 
    uninitialized_move_aux(InputIt first, InputIt last, ForwardIt d_first, false_type) 
    {
        ForwardIt current = d_first;
        try 
        {
            for (; first != last; ++first, ++current)
            {
                mystl::construct(addressof(*current), mystl::move(*first));
            }
            return current;
        }
        catch (...) 
        {
            destroy(d_first, current);
            throw;
        }
    }

    // uninitialized_move 的分派函数
    template<class InputIt, class ForwardIt>
    ForwardIt uninitialized_move(InputIt first, InputIt last, ForwardIt d_first) 
    {
        using Value = typename iterator_traits<ForwardIt>::value_type;
        return uninitialized_move_aux(first, last, d_first, is_trivially_move_assignable<Value>{});
    }



    /*****************************************************************************************/
    // uninitialized_move_n
    // 在未初始化内存空间上移动 n 个元素
    /*****************************************************************************************/
    template<class InputIt, class Size, class ForwardIt>
    ForwardIt 
    uninitialized_move_n_aux(InputIt first, Size n, ForwardIt d_first, true_type) 
    {
        return mystl::move(first, first + n, d_first);  // 可平凡赋值类型直接使用move
    }

    template<class InputIt, class Size, class ForwardIt>
    ForwardIt 
    uninitialized_move_n_aux(InputIt first, Size n, ForwardIt d_first, false_type) 
    {
        ForwardIt current = d_first;
        try 
        {
            for (; n > 0; --n, ++first, ++current)
            {
                mystl::construct(addressof(*current), mystl::move(*first));
            }
            return current;
        }
        catch (...) 
        {
            destroy(d_first, current);
            throw;
        }
    }

    template<class InputIt, class Size, class ForwardIt>
    ForwardIt uninitialized_move_n(InputIt first, Size n, ForwardIt d_first) 
    {
        using Value = typename iterator_traits<ForwardIt>::value_type;
        return uninitialized_move_n_aux(first, n, d_first, is_trivially_move_assignable<Value>{});
    }



    /*****************************************************************************************/
    // uninitialized_fill
    // 在未初始化内存空间上填充元素
    /*****************************************************************************************/
    // 对单字节类型的特化版本
    template<class ForwardIt, class T>
    typename enable_if<
        is_byte_type<typename iterator_traits<ForwardIt>::value_type>::value &&
        is_byte_type<T>::value, void>::type
    uninitialized_fill(ForwardIt first, ForwardIt last, const T& value) 
    {
        std::memset(addressof(*first), static_cast<unsigned char>(value), last - first);
    }

    // 其他类型的一般实现
    template<class ForwardIt, class T>
    typename enable_if<!
        (is_byte_type<typename iterator_traits<ForwardIt>::value_type>::value &&
         is_byte_type<T>::value), void>::type
    uninitialized_fill(ForwardIt first, ForwardIt last, const T& value) 
    {
        ForwardIt current = first;
        try 
        {
            for (; current != last; ++current)
            {
                mystl::construct(addressof(*current), value);
            }
        }
        catch (...) 
        {
            destroy(first, current);
            throw;
        }
    }



    /*****************************************************************************************/
    // uninitialized_fill_n
    // 在未初始化内存空间上填充 n 个元素
    /*****************************************************************************************/
    // 对单字节类型的特化版本
    template<class ForwardIt, class Size, class T>
    typename enable_if<
        is_byte_type<typename iterator_traits<ForwardIt>::value_type>::value &&
        is_byte_type<T>::value, ForwardIt>::type
    uninitialized_fill_n(ForwardIt first, Size n, const T& value) 
    {
        std::memset(addressof(*first), static_cast<unsigned char>(value), n);
        return first + n;
    }

    // 其他类型的一般实现
    template<class ForwardIt, class Size, class T>
    typename enable_if<!
        (is_byte_type<typename iterator_traits<ForwardIt>::value_type>::value &&
         is_byte_type<T>::value), ForwardIt>::type
    uninitialized_fill_n(ForwardIt first, Size n, const T& value) 
    {
        ForwardIt current = first;
        try 
        {
            for (; n > 0; --n, ++current)
            {
                mystl::construct(addressof(*current), value);
            }
            return current;
        }
        catch (...) 
        {
            destroy(first, current);
            throw;
        }
    }



    /*****************************************************************************************/
    // uninitialized_default_construct
    // 在未初始化内存空间上默认构造元素
    /*****************************************************************************************/
    template<class ForwardIt>
    void uninitialized_default_construct(ForwardIt first, ForwardIt last) 
    {
        using T = typename iterator_traits<ForwardIt>::value_type;
        ForwardIt current = first;
        try 
        {
            for (; current != last; ++current)
            {
                mystl::construct(addressof(*current));
            }
        }
        catch (...) 
        {
            destroy(first, current);
            throw;
        }
    }

    

    /*****************************************************************************************/
    // uninitialized_value_construct
    // 在未初始化内存空间上值构造元素
    /*****************************************************************************************/
    template<class ForwardIt>
    void uninitialized_value_construct(ForwardIt first, ForwardIt last) 
    {
        using T = typename iterator_traits<ForwardIt>::value_type;
        ForwardIt current = first;
        try 
        {
            for (; current != last; ++current)
            {
                mystl::construct(addressof(*current));
            }
        }
        catch (...) 
        {
            destroy(first, current);
            throw;
        }
    }

} // namespace mystl 