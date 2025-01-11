#pragma once

#include "type_traits.hpp"

namespace mystl 
{
    // move
    template<class T>
    constexpr typename remove_reference<T>::type&& move(T&& arg) noexcept
    {
        return static_cast<typename remove_reference<T>::type&&>(arg);
    }

    // forward
    template<class T>
    constexpr T&& forward(typename remove_reference<T>::type& arg) noexcept
    {
        return static_cast<T&&>(arg);
    }

    template<class T>
    constexpr T&& forward(typename remove_reference<T>::type&& arg) noexcept
    {
        static_assert(!is_lvalue_reference<T>::value, "Can not forward an rvalue as an lvalue.");
        return static_cast<T&&>(arg);
    }

    // swap
    template<class T>
    constexpr void swap(T& a, T& b) noexcept(is_nothrow_move_constructible_v<T> && is_nothrow_move_assignable_v<T>)
    {
        T temp = move(a);
        a = move(b);
        b = move(temp);
    }

    template<class ForwardIt1, class ForwardIt2>
    ForwardIt2 swap_ranges(ForwardIt1 first1, ForwardIt1 last1, ForwardIt2 first2) 
    {
        while (first1 != last1) 
        {
            mystl::swap(*first1, *first2);
            ++first1;
            ++first2;
        }
        return first2;
    }

    template<class T, size_t N>
    constexpr void swap(T (&a)[N], T (&b)[N]) noexcept(noexcept(swap(*a, *b)))
    {
        for (size_t i = 0; i < N; ++i)
        {
            swap(a[i], b[i]);
        }
    }


    // --------------------------------------------------------------------------------------
    // pair

    // 结构体模板 : pair
    // 两个模板参数分别表示两个数据的类型
    // 用 first 和 second 来分别取出第一个数据和第二个数据
    template<class T1, class T2>
    struct pair 
    {
        T1 first;
        T2 second;
        
        pair() = default;
        pair(const T1& x, const T2& y) : first(x), second(y) {}
    };

    template<class T1, class T2>
    pair<T1, T2> make_pair(T1 first, T2 second) 
    {
        return pair<T1, T2>(first, second);
    }

} // namespace mystl 