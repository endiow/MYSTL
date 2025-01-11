#pragma once

#include <type_traits>

namespace mystl 
{
    // 基本类型特征
    using true_type = std::true_type;   // 表示编译期真值的类型
    using false_type = std::false_type; // 表示编译期假值的类型

    template<bool B>
    using bool_constant = std::bool_constant<B>; // 包装布尔值的类型特征

    // 类型修饰特征
    template<class T>
    using remove_reference = std::remove_reference<T>;      // 移除类型的引用
    template<class T>
    using remove_reference_t = std::remove_reference_t<T>;  // 移除引用的辅助模板

    template<class T>
    using remove_const = std::remove_const<T>;             // 移除类型的const限定符
    template<class T>
    using remove_const_t = std::remove_const_t<T>;         // 移除const的辅助模板

    template<class T>
    using remove_cv = std::remove_cv<T>;                   // 移除const和volatile限定符
    template<class T>
    using remove_cv_t = std::remove_cv_t<T>;               // 移除cv限定符的辅助模板

    // 类型关系特征
    template<class T, class U>
    using is_same = std::is_same<T, U>;                    // 判断两个类型是否相同
    template<class T, class U>
    inline constexpr bool is_same_v = std::is_same_v<T, U>;

    template<class Base, class Derived>
    using is_base_of = std::is_base_of<Base, Derived>;     // 判断是否为基类关系
    template<class Base, class Derived>
    inline constexpr bool is_base_of_v = is_base_of<Base, Derived>::value;

    template<class From, class To>
    using is_convertible = std::is_convertible<From, To>;   // 判断类型是否可转换
    template<class From, class To>
    inline constexpr bool is_convertible_v = is_convertible<From, To>::value;

    // 类型属性特征
    template<class T>
    using is_pod = std::is_pod<T>;                         // 判断是否为POD类型
    template<class T>
    inline constexpr bool is_pod_v = std::is_pod_v<T>;

    template<class T>
    using is_integral = std::is_integral<T>;               // 判断是否为整数类型（包括bool）
    template<class T>
    inline constexpr bool is_integral_v = std::is_integral_v<T>;

    template<class T>
    using is_pointer = std::is_pointer<T>;                 // 判断是否为指针类型
    template<class T>
    inline constexpr bool is_pointer_v = std::is_pointer_v<T>;

    template<class T>
    using is_trivially_destructible = std::is_trivially_destructible<T>;     // 判断是否可平凡析构
    template<class T>
    inline constexpr bool is_trivially_destructible_v = std::is_trivially_destructible_v<T>;

    template<class T>
    using is_lvalue_reference = std::is_lvalue_reference<T>;                 // 判断是否为左值引用
    template<class T>
    inline constexpr bool is_lvalue_reference_v = std::is_lvalue_reference_v<T>;

    // 移动语义相关特征
    template<class T>
    using is_nothrow_move_constructible = std::is_nothrow_move_constructible<T>;    // 判断是否为无异常移动构造
    template<class T>
    inline constexpr bool is_nothrow_move_constructible_v = std::is_nothrow_move_constructible_v<T>;

    template<class T>
    using is_nothrow_move_assignable = std::is_nothrow_move_assignable<T>;          // 判断是否为无异常移动赋值
    template<class T>
    inline constexpr bool is_nothrow_move_assignable_v = std::is_nothrow_move_assignable_v<T>;

    template<class T>
    using is_trivially_move_assignable = std::is_trivially_move_assignable<T>;      // 判断是否为平凡移动赋值
    template<class T>
    inline constexpr bool is_trivially_move_assignable_v = std::is_trivially_move_assignable_v<T>;

    // 拷贝语义相关特征
    template<class T>
    using is_trivially_copy_assignable = std::is_trivially_copy_assignable<T>;      // 判断是否为平凡拷贝赋值
    template<class T>
    inline constexpr bool is_trivially_copy_assignable_v = std::is_trivially_copy_assignable_v<T>;

    // 模板元编程工具
    template<class...>
    using void_t = void;                                   // void_t的实现，用于SFINAE

    template<bool B, class T = void>
    using enable_if = std::enable_if<B, T>;               // 条件启用模板
    template<bool B, class T = void>
    using enable_if_t = typename enable_if<B, T>::type;

    // mystl特有的类型特征
    // 判断是否为字节类型（用于特化字节相关的算法）
    template<typename T>
    struct is_byte_type : mystl::false_type {};

    template<> struct is_byte_type<char> : mystl::true_type {};
    template<> struct is_byte_type<signed char> : mystl::true_type {};
    template<> struct is_byte_type<unsigned char> : mystl::true_type {};

    template<typename T>
    inline constexpr bool is_byte_type_v = is_byte_type<T>::value;

} // namespace mystl 