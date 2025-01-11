#pragma once

#include <cstring>  
#include "type_traits.hpp"
#include "util.hpp"
#include "iterator.hpp"

namespace mystl 
{
    /*****************************************************************************************/
    // min 
    // 取二者中的较小值，语义相等时保证返回第一个参数
    /*****************************************************************************************/
    template<class T>
    constexpr const T& min(const T& a, const T& b) 
    {
        return (b < a) ? b : a;
    }

    template<class T, class Compare>
    constexpr const T& min(const T& a, const T& b, Compare comp) 
    {
        return comp(b, a) ? b : a;
    }

    /*****************************************************************************************/
    // max
    // 取二者中的较大值，语义相等时保证返回第一个参数
    /*****************************************************************************************/
    template<class T>
    constexpr const T& max(const T& a, const T& b) 
    {
        return (a < b) ? b : a;
    }

    template<class T, class Compare>
    constexpr const T& max(const T& a, const T& b, Compare comp) 
    {
        return comp(a, b) ? b : a;
    }

    /*****************************************************************************************/
    // equal
    // 比较第一序列在 [first, last)区间上的元素值是否和第二序列相等
    /*****************************************************************************************/
    template<class InputIt1, class InputIt2>
    bool equal(InputIt1 first1, InputIt1 last1, InputIt2 first2) 
    {
        for (; first1 != last1; ++first1, ++first2) 
        {
            if (!(*first1 == *first2)) 
            {
                return false;
            }
        }
        return true;
    }

    template<class InputIt1, class InputIt2, class BinaryPredicate>
    bool equal(InputIt1 first1, InputIt1 last1, InputIt2 first2, BinaryPredicate pred) 
    {
        for (; first1 != last1; ++first1, ++first2) 
        {
            if (!pred(*first1, *first2)) 
            {
                return false;
            }
        }
        return true;
    }

    /*****************************************************************************************/
    // mismatch
    // 平行比较两个序列，找到第一处失配的元素，返回一对迭代器，分别指向两个序列中失配的元素
    /*****************************************************************************************/
    template<class InputIt1, class InputIt2>
    mystl::pair<InputIt1, InputIt2> mismatch(InputIt1 first1, InputIt1 last1, InputIt2 first2) 
    {
        while (first1 != last1 && *first1 == *first2) 
        {
            ++first1;
            ++first2;
        }
        return mystl::pair<InputIt1, InputIt2>(first1, first2);
    }

    template<class InputIt1, class InputIt2, class BinaryPredicate>
    mystl::pair<InputIt1, InputIt2> mismatch(InputIt1 first1, InputIt1 last1, 
                                             InputIt2 first2, BinaryPredicate pred) 
    {
        while (first1 != last1 && pred(*first1, *first2)) 
        {
            ++first1;
            ++first2;
        }
        return mystl::pair<InputIt1, InputIt2>(first1, first2);
    }

    /*****************************************************************************************/
    // lexicographical_compare
    // 以字典序排列对两个序列进行比较，当在某个位置发现第一组不相等元素时，有下列几种情况：
    // (1)如果第一序列的元素较小，返回 true ，否则返回 false
    // (2)如果到达 last1 而尚未到达 last2 返回 true
    // (3)如果到达 last2 而尚未到达 last1 返回 false
    // (4)如果同时到达 last1 和 last2 返回 false
    /*****************************************************************************************/
    template<class InputIt1, class InputIt2>
    bool lexicographical_compare(InputIt1 first1, InputIt1 last1,
                                 InputIt2 first2, InputIt2 last2)
    {
        for (; first1 != last1 && first2 != last2; ++first1, ++first2)
        {
            if (*first1 < *first2) return true;
            if (*first2 < *first1) return false;
        }
        return first1 == last1 && first2 != last2;
    }

    // 重载版本使用函数对象 comp 代替比较操作
    template<class InputIt1, class InputIt2, class Compare>
    bool lexicographical_compare(InputIt1 first1, InputIt1 last1,
                                 InputIt2 first2, InputIt2 last2, Compare comp)
    {
        for (; first1 != last1 && first2 != last2; ++first1, ++first2)
        {
            if (comp(*first1, *first2)) return true;
            if (comp(*first2, *first1)) return false;
        }
        return first1 == last1 && first2 != last2;
    }

    inline bool lexicographical_compare(const unsigned char* first1,
                                        const unsigned char* last1,
                                        const unsigned char* first2,
                                        const unsigned char* last2)
    {
        const auto len1 = last1 - first1;
        const auto len2 = last2 - first2;
        // 先比较相同长度的部分
        const auto result = std::memcmp(first1, first2, mystl::min(len1, len2));
        // 若相等，则长度较短的序列小
        return result != 0 ? result < 0 : len1 < len2;
    }




    /*****************************************************************************************/
    // copy
    // 把 [first, last)区间内的元素拷贝到 [result, result + (last - first))内
    /*****************************************************************************************/
    template<class InputIt, class OutputIt>
    OutputIt copy_dispatch(InputIt first, InputIt last, OutputIt result, true_type)
    {
        const auto n = last - first;
        if (n != 0)
        {
            std::memmove(iterator_base(result), iterator_base(first), n * sizeof(*iterator_base(first)));
            return result + n;
        }
        return result;
    }

    template<class InputIt, class OutputIt>
    OutputIt copy_dispatch(InputIt first, InputIt last, OutputIt result, false_type)
    {
        while (first != last)
        {
            *result = *first;
            ++first;
            ++result;
        }
        return result;
    }

    template<class InputIt, class OutputIt>
    OutputIt copy(InputIt first, InputIt last, OutputIt result)
    {
        using value_type = typename iterator_traits<InputIt>::value_type;
        return copy_dispatch(first, last, result, is_trivially_copy_assignable<value_type>());
    }

    /*****************************************************************************************/
    // copy_backward
    // 将 [first, last)区间内的元素拷贝到 [result - (last - first), result)内
    /*****************************************************************************************/
    template<class BidirIt1, class BidirIt2>
    BidirIt2 copy_backward_dispatch(BidirIt1 first, BidirIt1 last, BidirIt2 result, true_type)
    {
        const auto n = last - first;
        if (n != 0)
        {
            result -= n;
            std::memmove(iterator_base(result), iterator_base(first), n * sizeof(*iterator_base(first)));
        }
        return result;
    }

    template<class BidirIt1, class BidirIt2>
    BidirIt2 copy_backward_dispatch(BidirIt1 first, BidirIt1 last, BidirIt2 result, false_type)
    {
        while (first != last)
        {
            *(--result) = *(--last);
        }
        return result;
    }

    template<class BidirIt1, class BidirIt2>
    BidirIt2 copy_backward(BidirIt1 first, BidirIt1 last, BidirIt2 result)
    {
        using value_type = typename iterator_traits<BidirIt1>::value_type;
        return copy_backward_dispatch(first, last, result, is_trivially_copy_assignable<value_type>());
    }

    /*****************************************************************************************/
    // copy_if
    // 把[first, last)内满足一元操作 pred 的元素拷贝到以 result 为起始的位置上
    /*****************************************************************************************/
    template<class InputIt, class OutputIt, class UnaryPredicate>
    OutputIt copy_if(InputIt first, InputIt last, OutputIt result, UnaryPredicate pred)
    {
        while (first != last)
        {
            if (pred(*first))
            {
                *result = *first;
                ++result;
            }
            ++first;
        }
        return result;
    }

    /*****************************************************************************************/
    // copy_n
    // 复制来自范围的 n 个元素
    /*****************************************************************************************/
    template<class InputIt, class Size, class OutputIt>
    OutputIt copy_n_dispatch(InputIt first, Size count, OutputIt result, true_type)
    {
        if (count > 0)
        {
            std::memmove(result.base(), first.base(), count * sizeof(*first.base()));
            return result + count;
        }
        return result;
    }

    template<class InputIt, class Size, class OutputIt>
    OutputIt copy_n_dispatch(InputIt first, Size count, OutputIt result, false_type)
    {
        for (; count > 0; --count, ++first, ++result)
        {
            *result = *first;
        }
        return result;
    }

    template<class InputIt, class Size, class OutputIt>
    OutputIt copy_n(InputIt first, Size count, OutputIt result)
    {
        using value_type = typename iterator_traits<InputIt>::value_type;
        return copy_n_dispatch(first, count, result, is_trivially_copy_assignable<value_type>());
    }

    /*****************************************************************************************/
    // move
    // 移动 [first, last)区间内的元素到 [result, result + (last - first))内
    /*****************************************************************************************/
    template<class InputIt, class OutputIt>
    OutputIt move_dispatch(InputIt first, InputIt last, OutputIt result, true_type)
    {
        const auto n = last - first;
        if (n != 0)
        {
            std::memmove(iterator_base(result), iterator_base(first), n * sizeof(*iterator_base(first)));
            return result + n;
        }
        return result;
    }

    template<class InputIt, class OutputIt>
    OutputIt move_dispatch(InputIt first, InputIt last, OutputIt result, false_type)
    {
        while (first != last)
        {
            *result = mystl::move(*first);
            ++first;
            ++result;
        }
        return result;
    }

    template<class InputIt, class OutputIt>
    OutputIt move(InputIt first, InputIt last, OutputIt result)
    {
        using value_type = typename iterator_traits<InputIt>::value_type;
        return move_dispatch(first, last, result, is_trivially_move_assignable<value_type>());
    }

    /*****************************************************************************************/
    // move_backward
    // 将 [first, last)区间内的元素移动到 [result - (last - first), result)内
    /*****************************************************************************************/
    template<class BidirIt1, class BidirIt2>
    BidirIt2 move_backward_dispatch(BidirIt1 first, BidirIt1 last, BidirIt2 result, true_type)
    {
        const auto n = last - first;
        if (n != 0)
        {
            result -= n;
            std::memmove(iterator_base(result), iterator_base(first), n * sizeof(*iterator_base(first)));
        }
        return result;
    }

    template<class BidirIt1, class BidirIt2>
    BidirIt2 move_backward_dispatch(BidirIt1 first, BidirIt1 last, BidirIt2 result, false_type)
    {
        while (first != last)
        {
            *(--result) = mystl::move(*(--last));
        }
        return result;
    }

    template<class BidirIt1, class BidirIt2>
    BidirIt2 move_backward(BidirIt1 first, BidirIt1 last, BidirIt2 result)
    {
        using value_type = typename iterator_traits<BidirIt1>::value_type;
        return move_backward_dispatch(first, last, result, is_trivially_move_assignable<value_type>());
    }

    /*****************************************************************************************/
    // fill
    // 为 [first, last)区间内的所有元素填充新值
    /*****************************************************************************************/
    // 单字节类型的特化版本
    template<class ForwardIt, class T>
    typename enable_if<is_byte_type<T>::value>::type
    fill(ForwardIt first, ForwardIt last, const T& value)
    {
        unsigned char tmp = static_cast<unsigned char>(value);
        std::memset(iterator_base(first), tmp, last - first);
    }

    // 一般类型的实现
    template<class ForwardIt, class T>
    typename enable_if<!is_byte_type<T>::value>::type
    fill(ForwardIt first, ForwardIt last, const T& value)
    {
        for (; first != last; ++first)
        {
            *first = value;
        }
    }

    /*****************************************************************************************/
    // fill_n
    // 从 first 位置开始填充 n 个值
    /*****************************************************************************************/
    // 单字节类型的特化版本
    template<class OutputIt, class Size, class T>
    typename enable_if<is_byte_type<T>::value, OutputIt>::type
    fill_n(OutputIt first, Size count, const T& value)
    {
        if (count > 0)
        {
            std::memset(iterator_base(first), static_cast<unsigned char>(value), count);
            return first + count;
        }
        return first;
    }

    // 一般类型的实现
    template<class OutputIt, class Size, class T>
    typename enable_if<!is_byte_type<T>::value, OutputIt>::type
    fill_n(OutputIt first, Size count, const T& value)
    {
        for (; count > 0; --count, ++first)
        {
            *first = value;
        }
        return first;
    }




    /*****************************************************************************************/
    // iter_swap
    // 将两个迭代器所指对象对调
    /*****************************************************************************************/
    template<class ForwardIt1, class ForwardIt2>
    void iter_swap(ForwardIt1 a, ForwardIt2 b) 
    {
        mystl::swap(*a, *b);
    }

    

    // 二分查找基础操作
    template<class ForwardIt, class T>
    ForwardIt lower_bound(ForwardIt first, ForwardIt last, const T& value) 
    {
        ForwardIt it;
        typename iterator_traits<ForwardIt>::difference_type count, step;
        count = mystl::distance(first, last);
        
        while (count > 0) 
        {
            it = first;
            step = count / 2;
            mystl::advance(it, step);
            
            if (*it < value) 
            {
                first = ++it;
                count -= step + 1;
            }
            else 
            {
                count = step;
            }
        }
        return first;
    }

    template<class ForwardIt, class T>
    ForwardIt upper_bound(ForwardIt first, ForwardIt last, const T& value) 
    {
        ForwardIt it;
        typename iterator_traits<ForwardIt>::difference_type count, step;
        count = mystl::distance(first, last);
        
        while (count > 0) 
        {
            it = first;
            step = count / 2;
            mystl::advance(it, step);
            
            if (!(value < *it)) 
            {
                first = ++it;
                count -= step + 1;
            }
            else 
            {
                count = step;
            }
        }
        return first;
    }

    

} // namespace mystl 