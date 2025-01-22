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



    /*****************************************************************************************/ 
    // 关系运算
    /*****************************************************************************************/ 
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
    


    /*****************************************************************************************/ 
    // 算术运算
    /*****************************************************************************************/ 
    // 加法
    template <class T>
    struct plus 
    {
        T operator()(const T& x, const T& y) const { return x + y; }
    };

    // 减法
    template <class T>
    struct minus 
    {
        T operator()(const T& x, const T& y) const { return x - y; }
    };

    // 乘法
    template <class T>
    struct multiplies 
    {
        T operator()(const T& x, const T& y) const { return x * y; }
    };

    // 除法
    template <class T>
    struct divides 
    {
        T operator()(const T& x, const T& y) const { return x / y; }
    };

    // 取模
    template <class T>
    struct modulus 
    {
        T operator()(const T& x, const T& y) const { return x % y; }
    };

    // 取反 
    template <class T>
    struct negate 
    {
        T operator()(const T& x) const { return -x; }
    };


    /*****************************************************************************************/ 
    // 算术运算的单位元素
    /*****************************************************************************************/ 
    // 加法单位元（0）
    template <class T>
    inline T identity_element(plus<T>) { return T(0); }

    // 乘法单位元（1）
    template <class T>
    inline T identity_element(multiplies<T>) { return T(1); }



    /*****************************************************************************************/ 
    // 逻辑运算
    /*****************************************************************************************/ 
    // 逻辑与
    template <class T>
    struct logical_and : public binary_function<T, T, bool> 
    {
        bool operator()(const T& x, const T& y) const { return x && y; }
    };

    // 逻辑或
    template <class T>
    struct logical_or : public binary_function<T, T, bool> 
    {
        bool operator()(const T& x, const T& y) const { return x || y; }
    };

    // 逻辑非
    template <class T>
    struct logical_not : public unary_function<T, bool> 
    {
        bool operator()(const T& x) const { return !x; }
    };



    /*****************************************************************************************/ 
    // 位运算
    /*****************************************************************************************/ 
    // 位与 
    template <class T>
    struct bit_and : public binary_function<T, T, T> 
    {
        T operator()(const T& x, const T& y) const { return x & y; }
    };

    // 位或 
    template <class T>
    struct bit_or : public binary_function<T, T, T> 
    {
        T operator()(const T& x, const T& y) const { return x | y; }
    };

    // 位异或
    template <class T>
    struct bit_xor : public binary_function<T, T, T> 
    {
        T operator()(const T& x, const T& y) const { return x ^ y; }
    };

    // 位非
    template <class T>
    struct bit_not : public unary_function<T, T> 
    {
        T operator()(const T& x) const { return ~x; }
    };

    

    /*****************************************************************************************/ 
    // 证同、选择和投影
    /*****************************************************************************************/ 
    // 证同函数：返回自己
    template <class T>
    struct identity : public unary_function<T, T> 
    {
        const T& operator()(const T& x) const { return x; }
    };

    // 选择函数：返回第一个参数
    template <class Pair>
    struct select1st : public unary_function<Pair, typename Pair::first_type> 
    {
        const typename Pair::first_type& operator()(const Pair& x) const 
        {
            return x.first;
        }
    };

    // 选择函数：返回第二个参数
    template <class Pair>
    struct select2nd : public unary_function<Pair, typename Pair::second_type> 
    {
        const typename Pair::second_type& operator()(const Pair& x) const 
        {
            return x.second;
        }
    };

    // 投影函数：返回第一个参数
    template <class Arg1, class Arg2>
    struct project1st : public binary_function<Arg1, Arg2, Arg1> 
    {
        Arg1 operator()(const Arg1& x, const Arg2&) const { return x; }
    };

    // 投影函数：返回第二个参数
    template <class Arg1, class Arg2>
    struct project2nd : public binary_function<Arg1, Arg2, Arg2> 
    {
        Arg2 operator()(const Arg1&, const Arg2& y) const { return y; }
    };

    // 添加基础类型的哈希函数
    template <class Key>
    struct hash {};

    template <>
    struct hash<int>
    {
        size_t operator()(int x) const { return x; }
    };

    template <>
    struct hash<long>
    {
        size_t operator()(long x) const { return x; }
    };

    template <>
    struct hash<size_t>
    {
        size_t operator()(size_t x) const { return x; }
    };

} // namespace mystl
