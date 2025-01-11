#pragma once

#include <type_traits>

namespace mystl 
{
    // 从标准库导入常用的类型萃取
    template<class T>
    using remove_reference = std::remove_reference<T>;

    template<class T>
    using remove_reference_t = std::remove_reference_t<T>;

    template<class T>
    using remove_const = std::remove_const<T>;

    template<class T>
    using remove_const_t = std::remove_const_t<T>;

    template<class T>
    using remove_cv = std::remove_cv<T>;

    template<class T>
    using remove_cv_t = std::remove_cv_t<T>;

    template<class T, class U>
    using is_same = std::is_same<T, U>;

    template<class T, class U>
    inline constexpr bool is_same_v = std::is_same_v<T, U>;

    template<class T>
    using is_trivially_destructible = std::is_trivially_destructible<T>;

    template<class T>
    inline constexpr bool is_trivially_destructible_v = std::is_trivially_destructible_v<T>;

    // 直接使用标准库的 is_lvalue_reference
    template<class T>
    using is_lvalue_reference = std::is_lvalue_reference<T>;

    template<class T>
    inline constexpr bool is_lvalue_reference_v = std::is_lvalue_reference_v<T>;

    // 添加移动相关的类型萃取
    template<class T>
    using is_nothrow_move_constructible = std::is_nothrow_move_constructible<T>;

    template<class T>
    inline constexpr bool is_nothrow_move_constructible_v = std::is_nothrow_move_constructible_v<T>;

    template<class T>
    using is_nothrow_move_assignable = std::is_nothrow_move_assignable<T>;

    template<class T>
    inline constexpr bool is_nothrow_move_assignable_v = std::is_nothrow_move_assignable_v<T>;

    // 辅助的 true_type 和 false_type
    using true_type = std::true_type;
    using false_type = std::false_type;

    // 可以根据需要添加更多类型萃取

    template<class...>
    using void_t = void;

    template<bool B, class T = void>
    using enable_if = std::enable_if<B, T>;

    template<bool B, class T = void>
    using enable_if_t = typename enable_if<B, T>::type;

    // bool 常量
    template<bool B>
    using bool_constant = std::bool_constant<B>;

    // 类型转换
    template<class From, class To>
    using is_convertible = std::is_convertible<From, To>;

    template<class From, class To>
    inline constexpr bool is_convertible_v = is_convertible<From, To>::value;

    // 继承关系
    template<class Base, class Derived>
    using is_base_of = std::is_base_of<Base, Derived>;

    template<class Base, class Derived>
    inline constexpr bool is_base_of_v = is_base_of<Base, Derived>::value;

    template<class T>
    using is_pod = std::is_pod<T>;

    template<class T>
    inline constexpr bool is_pod_v = std::is_pod_v<T>;

    // 使用标准库的实现
    template<class T>
    using is_trivially_copy_assignable = std::is_trivially_copy_assignable<T>;

    template<class T>
    inline constexpr bool is_trivially_copy_assignable_v = std::is_trivially_copy_assignable_v<T>;

    template<class T>
    using is_trivially_move_assignable = std::is_trivially_move_assignable<T>;

    template<class T>
    inline constexpr bool is_trivially_move_assignable_v = std::is_trivially_move_assignable_v<T>;

    template<typename T>
    struct is_byte_type : mystl::false_type {};

    template<> struct is_byte_type<char> : mystl::true_type {};
    template<> struct is_byte_type<signed char> : mystl::true_type {};
    template<> struct is_byte_type<unsigned char> : mystl::true_type {};

    template<typename T>
    inline constexpr bool is_byte_type_v = is_byte_type<T>::value;
} // namespace mystl 