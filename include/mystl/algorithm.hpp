#pragma once

#include "algorithm_base.hpp"
#include "iterator.hpp"
#include "type_traits.hpp"

namespace mystl 
{
    /*****************************************************************************************/
    // find
    // 在[first, last)范围内查找等于value的元素，返回指向第一个这样的元素的迭代器
    /*****************************************************************************************/
    template<class InputIt, class T>
    InputIt find(InputIt first, InputIt last, const T& value) 
    {
        for (; first != last; ++first) 
        {
            if (*first == value) 
            {
                return first;
            }
        }
        return last;
    }

    /*****************************************************************************************/
    // find_if
    // 在[first, last)范围内查找满足谓词pred的元素，返回指向第一个这样的元素的迭代器
    /*****************************************************************************************/
    template<class InputIt, class UnaryPredicate>
    InputIt find_if(InputIt first, InputIt last, UnaryPredicate pred) 
    {
        for (; first != last; ++first) 
        {
            if (pred(*first)) 
            {
                return first;
            }
        }
        return last;
    }

    /*****************************************************************************************/
    // find_if_not
    // 在[first, last)范围内查找不满足谓词pred的元素，返回指向第一个这样的元素的迭代器
    /*****************************************************************************************/
    template<class InputIt, class UnaryPredicate>
    InputIt find_if_not(InputIt first, InputIt last, UnaryPredicate pred) 
    {
        for (; first != last; ++first) 
        {
            if (!pred(*first)) 
            {
                return first;
            }
        }
        return last;
    }

    /*****************************************************************************************/
    // find_first_of
    // 在第一个范围[first1, last1)中查找第一个出现在第二个范围[first2, last2)中的元素
    /*****************************************************************************************/
    template<class InputIt, class ForwardIt>
    InputIt find_first_of(InputIt first1, InputIt last1, ForwardIt first2, ForwardIt last2) 
    {
        for (; first1 != last1; ++first1) 
        {
            for (ForwardIt it = first2; it != last2; ++it) 
            {
                if (*first1 == *it) 
                {
                    return first1;
                }
            }
        }
        return last1;
    }

    // 重载版本使用二元谓词comp
    template<class InputIt, class ForwardIt, class BinaryPredicate>
    InputIt find_first_of(InputIt first1, InputIt last1,
                          ForwardIt first2, ForwardIt last2,
                          BinaryPredicate comp) 
    {
        for (; first1 != last1; ++first1) 
        {
            for (ForwardIt it = first2; it != last2; ++it) 
            {
                if (comp(*first1, *it)) 
                {
                    return first1;
                }
            }
        }
        return last1;
    }

    /*****************************************************************************************/
    // adjacent_find
    // 查找[first, last)范围内连续两个相等的元素，返回指向第一个元素的迭代器
    /*****************************************************************************************/
    template<class ForwardIt>
    ForwardIt adjacent_find(ForwardIt first, ForwardIt last) 
    {
        if (first == last) 
        {
            return last;
        }

        ForwardIt next = first;
        ++next;
        
        while (next != last) 
        {
            if (*first == *next) 
            {
                return first;
            }
            ++first;
            ++next;
        }
        return last;
    }

    // 重载版本使用二元谓词comp
    template<class ForwardIt, class BinaryPredicate>
    ForwardIt adjacent_find(ForwardIt first, ForwardIt last, BinaryPredicate comp) 
    {
        if (first == last) 
        {
            return last;
        }

        ForwardIt next = first;
        ++next;
        
        while (next != last) 
        {
            if (comp(*first, *next)) 
            {
                return first;
            }
            ++first;
            ++next;
        }
        return last;
    }

    

    /*****************************************************************************************/
    // count
    // 统计范围内等于value的元素个数
    /*****************************************************************************************/
    template<class InputIt, class T>
    typename iterator_traits<InputIt>::difference_type
    count(InputIt first, InputIt last, const T& value) 
    {
        typename iterator_traits<InputIt>::difference_type n = 0;
        for (; first != last; ++first) 
        {
            if (*first == value) 
            {
                ++n;
            }
        }
        return n;
    }

    /*****************************************************************************************/
    // count_if
    // 统计范围内满足谓词pred的元素个数
    /*****************************************************************************************/
    template<class InputIt, class UnaryPredicate>
    typename iterator_traits<InputIt>::difference_type
    count_if(InputIt first, InputIt last, UnaryPredicate pred) 
    {
        typename iterator_traits<InputIt>::difference_type n = 0;
        for (; first != last; ++first) 
        {
            if (pred(*first)) 
            {
                ++n;
            }
        }
        return n;
    }



    /*****************************************************************************************/
    // search
    // 在范围[first1, last1)中查找[first2, last2)首次出现的位置
    /*****************************************************************************************/
    template<class ForwardIt1, class ForwardIt2>
    ForwardIt1 search(ForwardIt1 first1, ForwardIt1 last1,
                      ForwardIt2 first2, ForwardIt2 last2) 
    {
        if (first2 == last2) 
        {
            return first1;  // 空序列总是被找到
        }

        for (; first1 != last1; ++first1) 
        {
            ForwardIt1 it1 = first1;
            ForwardIt2 it2 = first2;
            
            while (*it1 == *it2) 
            {
                ++it1;
                ++it2;
                if (it2 == last2) 
                {
                    return first1;  // 找到完整的子序列
                }
                if (it1 == last1) 
                {
                    return last1;   // 主序列已经结束
                }
            }
        }
        return last1;  // 未找到子序列
    }

    // 重载版本使用二元谓词comp
    template<class ForwardIt1, class ForwardIt2, class BinaryPredicate>
    ForwardIt1 search(ForwardIt1 first1, ForwardIt1 last1,
                      ForwardIt2 first2, ForwardIt2 last2,
                      BinaryPredicate comp) 
    {
        if (first2 == last2) 
        {
            return first1;  // 空序列总是被找到
        }

        for (; first1 != last1; ++first1) 
        {
            ForwardIt1 it1 = first1;
            ForwardIt2 it2 = first2;
            
            while (true) 
            {
                if (!comp(*it1, *it2))  // 如果当前元素不匹配
                {
                    break;
                }
                ++it1;
                ++it2;
                if (it2 == last2)  // 如果已经匹配完整个模式
                {
                    return first1;  // 返回匹配序列的起始位置
                }
                if (it1 == last1)  // 如果主序列已经结束
                {
                    return last1;
                }
            }
        }
        return last1;  // 未找到子序列
    }

    /*****************************************************************************************/
    // search_n
    // 在范围[first, last)中查找连续n个值为value的子序列
    /*****************************************************************************************/
    template<class ForwardIt, class Size, class T>
    ForwardIt search_n(ForwardIt first, ForwardIt last, Size count, const T& value) 
    {
        if (count <= 0) 
        {
            return first;
        }

        for (; first != last; ++first) 
        {
            if (*first != value) 
            {
                continue;
            }

            ForwardIt candidate = first;
            Size cur_count = 0;

            while (true) 
            {
                ++cur_count;
                if (cur_count == count) 
                {
                    return candidate;  // 找到足够数量的连续元素
                }

                ++first;
                if (first == last) 
                {
                    return last;  // 序列结束
                }

                if (*first != value) 
                {
                    break;  // 连续性被打断
                }
            }
        }
        return last;  // 未找到足够数量的连续元素
    }

    // 重载版本使用二元谓词comp
    template<class ForwardIt, class Size, class T, class BinaryPredicate>
    ForwardIt search_n(ForwardIt first, ForwardIt last, Size count, const T& value, BinaryPredicate comp) 
    {
        if (count <= 0) 
        {
            return first;
        }

        for (; first != last; ++first) 
        {
            if (!comp(*first, value)) 
            {
                continue;
            }

            ForwardIt candidate = first;
            Size cur_count = 0;

            while (true) 
            {
                ++cur_count;
                if (cur_count == count) 
                {
                    return candidate;  // 找到足够数量的连续元素
                }

                ++first;
                if (first == last) 
                {
                    return last;  // 序列结束
                }

                if (!comp(*first, value)) 
                {
                    break;  // 连续性被打断
                }
            }
        }
        return last;  // 未找到足够数量的连续元素
    }



    /*****************************************************************************************/
    // replace
    // 将区间[first, last)中所有等于old_value的元素都替换为new_value
    /*****************************************************************************************/
    template<class ForwardIt, class T>
    void replace(ForwardIt first, ForwardIt last, 
                 const T& old_value, const T& new_value) 
    {
        for (; first != last; ++first) 
        {
            if (*first == old_value) 
            {
                *first = new_value;
            }
        }
    }

    /*****************************************************************************************/
    // replace_if
    // 将区间[first, last)中所有满足谓词pred的元素都替换为new_value
    /*****************************************************************************************/
    template<class ForwardIt, class UnaryPredicate, class T>
    void replace_if(ForwardIt first, ForwardIt last,
                    UnaryPredicate pred, const T& new_value) 
    {
        for (; first != last; ++first) 
        {
            if (pred(*first)) 
            {
                *first = new_value;
            }
        }
    }

    /*****************************************************************************************/
    // replace_copy
    // 复制[first, last)区间的元素到result，将其中等于old_value的元素替换为new_value
    /*****************************************************************************************/
    template<class InputIt, class OutputIt, class T>
    OutputIt replace_copy(InputIt first, InputIt last, OutputIt result,
                          const T& old_value, const T& new_value) 
    {
        for (; first != last; ++first, ++result) 
        {
            *result = (*first == old_value) ? new_value : *first;
        }
        return result;
    }

    /*****************************************************************************************/
    // replace_copy_if
    // 复制[first, last)区间的元素到result，将其中满足谓词pred的元素替换为new_value
    /*****************************************************************************************/
    template<class InputIt, class OutputIt, class UnaryPredicate, class T>
    OutputIt replace_copy_if(InputIt first, InputIt last, OutputIt result,
                             UnaryPredicate pred, const T& new_value) 
    {
        for (; first != last; ++first, ++result) 
        {
            *result = pred(*first) ? new_value : *first;
        }
        return result;
    }

    

    /*****************************************************************************************/
    // remove
    // 移除区间[first, last)中所有等于value的元素，返回新区间的尾部
    /*****************************************************************************************/
    template<class ForwardIt, class T>
    ForwardIt remove(ForwardIt first, ForwardIt last, const T& value) 
    {
        first = mystl::find(first, last, value);  // 找到第一个要移除的元素
        if (first == last) 
        {
            return first;  // 没有要移除的元素
        }

        ForwardIt result = first;  // result指向新区间的写入位置
        ++first;  // 从下一个位置开始检查
        for (; first != last; ++first) 
        {
            if (!(*first == value)) 
            {
                *result = mystl::move(*first);  // 移动不需要移除的元素
                ++result;
            }
        }
        return result;  // 返回新区间的尾部
    }

    /*****************************************************************************************/
    // remove_if
    // 移除区间[first, last)中所有满足谓词pred的元素，返回新区间的尾部
    /*****************************************************************************************/
    template<class ForwardIt, class UnaryPredicate>
    ForwardIt remove_if(ForwardIt first, ForwardIt last, UnaryPredicate pred) 
    {
        first = mystl::find_if(first, last, pred);  // 找到第一个要移除的元素
        if (first == last) 
        {
            return first;  // 没有要移除的元素
        }

        ForwardIt result = first;  // result指向新区间的写入位置
        ++first;  // 从下一个位置开始检查
        for (; first != last; ++first) 
        {
            if (!pred(*first)) 
            {
                *result = mystl::move(*first);  // 移动不需要移除的元素
                ++result;
            }
        }
        return result;  // 返回新区间的尾部
    }

    /*****************************************************************************************/
    // remove_copy
    // 复制[first, last)区间的元素到result，忽略所有等于value的元素
    /*****************************************************************************************/
    template<class InputIt, class OutputIt, class T>
    OutputIt remove_copy(InputIt first, InputIt last, OutputIt result, const T& value) 
    {
        for (; first != last; ++first) 
        {
            if (!(*first == value)) 
            {
                *result = *first;
                ++result;
            }
        }
        return result;
    }

    /*****************************************************************************************/
    // remove_copy_if
    // 复制[first, last)区间的元素到result，忽略所有满足谓词pred的元素
    /*****************************************************************************************/
    template<class InputIt, class OutputIt, class UnaryPredicate>
    OutputIt remove_copy_if(InputIt first, InputIt last, OutputIt result, UnaryPredicate pred) 
    {
        for (; first != last; ++first) 
        {
            if (!pred(*first)) 
            {
                *result = *first;
                ++result;
            }
        }
        return result;
    }

} // namespace mystl 