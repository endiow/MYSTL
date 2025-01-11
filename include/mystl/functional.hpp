#pragma once

namespace mystl 
{

    // 一元函数的参数型别和返回值型别
    template<class Arg, class Result>
    struct unary_function 
    {
        typedef Arg    argument_type;
        typedef Result result_type;
    };

    // 二元函数的参数型别的返回值型别
    template<class Arg1, class Arg2, class Result>
    struct binary_function 
    {
        typedef Arg1   first_argument_type;
        typedef Arg2   second_argument_type;
        typedef Result result_type;
    };

    // 函数对象：大于
    template<class T>
    struct greater : public binary_function<T, T, bool> 
    {
        bool operator()(const T& x, const T& y) const { return x > y; }
    };

    // 函数对象：小于
    template<class T>
    struct less : public binary_function<T, T, bool> 
    {
        bool operator()(const T& x, const T& y) const { return x < y; }
    };

    // 函数对象：等于
    template<class T>
    struct equal_to : public binary_function<T, T, bool> 
    {
        bool operator()(const T& x, const T& y) const { return x == y; }
    };

    // 函数对象：不等于
    template<class T>
    struct not_equal_to : public binary_function<T, T, bool> 
    {
        bool operator()(const T& x, const T& y) const { return x != y; }
    };

    // 函数对象：大于等于
    template<class T>
    struct greater_equal : public binary_function<T, T, bool> 
    {
        bool operator()(const T& x, const T& y) const { return x >= y; }
    };

    // 函数对象：小于等于
    template<class T>
    struct less_equal : public binary_function<T, T, bool> 
    {
        bool operator()(const T& x, const T& y) const { return x <= y; }
    };

} // namespace mystl
