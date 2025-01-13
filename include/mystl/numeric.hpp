#include "mystl/functional.hpp"
#include "mystl/iterator.hpp"

namespace mystl     
{
    /*****************************************************************************************/
    // 数值算法
    /*****************************************************************************************/

    /*****************************************************************************************/
    // accumulate
    // 计算序列中元素的和，可以提供初值和自定义操作
    /*****************************************************************************************/
    template<class InputIt, class T, class BinaryOperation>
    T accumulate(InputIt first, InputIt last, T init, BinaryOperation op)
    {
        for (; first != last; ++first)
        {
            init = op(init, *first);  // 使用自定义操作
        }
        return init;
    }

    template<class InputIt, class T>
    T accumulate(InputIt first, InputIt last, T init)
    {
        return mystl::accumulate(first, last, init, mystl::plus<T>());
    }



    /*****************************************************************************************/
    // inner_product
    // 计算两个序列的内积，可以提供初值和自定义操作
    /*****************************************************************************************/
    template<class InputIt1, class InputIt2, class T, class BinaryOperation1, class BinaryOperation2>
    T inner_product(InputIt1 first1, InputIt1 last1, InputIt2 first2, T init,
                    BinaryOperation1 op1, BinaryOperation2 op2)
    {
        for (; first1 != last1; ++first1, ++first2)
        {
            init = op1(init, op2(*first1, *first2));  // 使用自定义操作
        }
        return init;
    }

    template<class InputIt1, class InputIt2, class T>
    T inner_product(InputIt1 first1, InputIt1 last1, InputIt2 first2, T init)
    {
        return mystl::inner_product(first1, last1, first2, init,
                                    mystl::plus<T>(), mystl::multiplies<T>());
    }



    /*****************************************************************************************/
    // partial_sum
    // 计算序列的部分和，结果保存到以 d_first 为起始的位置
    /*****************************************************************************************/
    template<class InputIt, class OutputIt, class BinaryOperation>
    OutputIt partial_sum(InputIt first, InputIt last, OutputIt d_first, BinaryOperation op)
    {
        if (first == last) return d_first;
        
        typename iterator_traits<InputIt>::value_type sum = *first;
        *d_first = sum;
        
        while (++first != last)
        {
            sum = op(sum, *first);  // 使用自定义操作
            *++d_first = sum;
        }
        return ++d_first;
    }

    template<class InputIt, class OutputIt>
    OutputIt partial_sum(InputIt first, InputIt last, OutputIt d_first)
    {
        return mystl::partial_sum(first, last, d_first,
                                  mystl::plus<typename iterator_traits<InputIt>::value_type>());
    }



    /*****************************************************************************************/
    // adjacent_difference
    // 计算相邻元素的差，结果保存到以 d_first 为起始的位置
    /*****************************************************************************************/
    template<class InputIt, class OutputIt, class BinaryOperation>
    OutputIt adjacent_difference(InputIt first, InputIt last, OutputIt d_first, BinaryOperation op)
    {
        if (first == last) return d_first;
        
        typename iterator_traits<InputIt>::value_type value = *first;
        *d_first = value;
        
        while (++first != last)
        {
            typename iterator_traits<InputIt>::value_type tmp = *first;
            *++d_first = op(tmp, value);  // 使用自定义操作
            value = tmp;
        }
        return ++d_first;
    }

    template<class InputIt, class OutputIt>
    OutputIt adjacent_difference(InputIt first, InputIt last, OutputIt d_first)
    {
        return mystl::adjacent_difference(first, last, d_first,
                                          mystl::minus<typename iterator_traits<InputIt>::value_type>());
    }



    /*****************************************************************************************/
    // iota 函数：用连续递增的值填充一个范围
    /*****************************************************************************************/
    template <class ForwardIt, class T>
    void iota(ForwardIt first, ForwardIt last, T value)
    {
        while (first != last)
        {
            *first = value;
            ++first;
            ++value;
        }
    }
}; // namespace mystl   