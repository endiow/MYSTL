#pragma once

#include <cstddef>
#include <stdexcept>

namespace mystl 
{
    // 从 <cstddef> 导入
    using std::size_t;
    using std::ptrdiff_t;
    using std::nullptr_t;
    using std::max_align_t;

    // 从 <stdexcept> 导入常用异常类
    using std::exception;
    using std::runtime_error;
    using std::logic_error;
    using std::out_of_range;
    using std::length_error;
    using std::invalid_argument;
    using std::domain_error;
    using std::range_error;
    using std::overflow_error;
    using std::underflow_error;

} // namespace mystl 