#pragma once

#include <cstddef>
#include <stdexcept>
#include <cassert>
#include "iterator.hpp"

namespace mystl 
{
    // 从 <cstddef> 导入基本类型定义
    using std::size_t;      // 无符号整数类型，用于表示对象的大小
    using std::ptrdiff_t;   // 有符号整数类型，用于表示两个指针之间的距离
    using std::nullptr_t;   // 空指针类型
    using std::max_align_t; // 具有最严格对齐要求的类型

    // 从 <stdexcept> 导入标准异常类
    using std::exception;        // 所有标准异常的基类
    using std::runtime_error;    // 运行时错误，只能在程序运行时检测
    using std::logic_error;      // 逻辑错误，理论上可以通过代码检查来发现
    using std::out_of_range;     // 访问超出有效范围
    using std::length_error;     // 试图创建太长的对象
    using std::invalid_argument; // 无效的参数
    using std::domain_error;     // 参数的值域错误
    using std::range_error;      // 计算结果超出有效范围
    using std::overflow_error;   // 算术运算上溢
    using std::underflow_error;  // 算术运算下溢


    // 断言宏
    #define EXPECT(condition, message) \
        if (!(condition)) { throw std::runtime_error(message); }
} // namespace mystl 