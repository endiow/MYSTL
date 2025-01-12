#pragma once

#include "algorithm_base.hpp"
#include "iterator.hpp"
#include "type_traits.hpp"
#include "functional.hpp"
#include <cmath>  

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



    /*****************************************************************************************/
    // unique
    // 移除序列中连续重复的元素，返回新的尾部迭代器
    /*****************************************************************************************/
    template<class ForwardIt>
    ForwardIt unique(ForwardIt first, ForwardIt last) 
    {
        if (first == last) 
        {
            return last;
        }

        ForwardIt result = first;
        while (++first != last) 
        {
            if (!(*result == *first) && ++result != first) 
            {
                *result = mystl::move(*first);
            }
        }
        return ++result;
    }

    // 重载版本使用二元谓词 pred
    template<class ForwardIt, class BinaryPredicate>
    ForwardIt unique(ForwardIt first, ForwardIt last, BinaryPredicate pred) 
    {
        if (first == last) 
        {
            return last;
        }

        ForwardIt result = first;
        while (++first != last) 
        {
            if (!pred(*result, *first) && ++result != first) 
            {
                *result = mystl::move(*first);
            }
        }
        return ++result;
    }

    /*****************************************************************************************/
    // unique_copy
    // 从[first, last)中将元素复制到result，连续重复的元素只复制一次
    /*****************************************************************************************/
    template<class InputIt, class OutputIt>
    OutputIt unique_copy(InputIt first, InputIt last, OutputIt result) 
    {
        if (first == last) 
        {
            return result;
        }

        typename iterator_traits<InputIt>::value_type value = *first;
        *result = value;
        ++result;

        while (++first != last) 
        {
            if (!(value == *first)) 
            {
                value = *first;
                *result = value;
                ++result;
            }
        }
        return result;
    }

    // 重载版本使用二元谓词 pred
    template<class InputIt, class OutputIt, class BinaryPredicate>
    OutputIt unique_copy(InputIt first, InputIt last, OutputIt result, BinaryPredicate pred) 
    {
        if (first == last) 
        {
            return result;
        }

        typename iterator_traits<InputIt>::value_type value = *first;
        *result = value;
        ++result;

        while (++first != last) 
        {
            if (!pred(value, *first)) 
            {
                value = *first;
                *result = value;
                ++result;
            }
        }
        return result;
    }

    

    /*****************************************************************************************/
    // reverse
    // 将[first, last)区间内的元素反转
    /*****************************************************************************************/
    template<class BidirIt>
    void reverse(BidirIt first, BidirIt last) 
    {
        while ((first != last) && (first != --last)) 
        {
            mystl::iter_swap(first++, last);
        }
    }

    /*****************************************************************************************/
    // reverse_copy
    // 将[first, last)区间内的元素反转复制到result
    /*****************************************************************************************/
    template<class BidirIt, class OutputIt>
    OutputIt reverse_copy(BidirIt first, BidirIt last, OutputIt result) 
    {
        while (first != last) 
        {
            --last;
            *result = *last;
            ++result;
        }
        return result;
    }

    

    /*****************************************************************************************/
    // rotate
    // 将[first, middle)内的元素和[middle, last)内的元素互换，可以理解为将middle指向的元素旋转到first位置
    /*****************************************************************************************/
    template<class ForwardIt>
    ForwardIt rotate(ForwardIt first, ForwardIt middle, ForwardIt last) 
    {
        if (first == middle) 
        {
            return last;
        }
        if (middle == last) 
        {
            return first;
        }

        ForwardIt next = middle;
        do 
        {
            mystl::iter_swap(first++, next++);
            if (first == middle)
            {
                middle = next;
            }
        } while (next != last);

        ForwardIt ret = first;  // 记录返回值
        next = middle;
        while (next != last) 
        {
            mystl::iter_swap(first++, next++);
            if (first == middle)
            {
                middle = next;
            }
            else if (next == last)
            {
                next = middle;
            }
        }
        return ret;  // 返回原来middle位置的新位置
    }

    /*****************************************************************************************/
    // rotate_copy
    // 行为与rotate类似，不过将结果复制到result所指的范围
    /*****************************************************************************************/
    template<class ForwardIt, class OutputIt>
    OutputIt rotate_copy(ForwardIt first, ForwardIt middle, ForwardIt last, OutputIt result) 
    {
        // 先复制[middle, last)
        result = mystl::copy(middle, last, result);
        // 再复制[first, middle)
        return mystl::copy(first, middle, result);
    }



    /*****************************************************************************************/
    // random_shuffle
    // 将[first, last)内的元素随机重排
    // 重载版本使用自定义的随机数生成器 rand
    /*****************************************************************************************/
    template<class RandomIt>
    void random_shuffle(RandomIt first, RandomIt last) 
    {
        if (first == last) return;
        
        using difference_type = typename iterator_traits<RandomIt>::difference_type;
        for (RandomIt i = first + 1; i != last; ++i) 
        {
            difference_type j = rand() % ((i - first) + 1);  // 生成 [0, i-first] 范围内的随机数
            if (j != (i - first))  // 如果随机位置不是当前位置
            {
                mystl::iter_swap(first + j, i);  // 交换元素
            }
        }
    }

    template<class RandomIt, class RandomFunc>
    void random_shuffle(RandomIt first, RandomIt last, RandomFunc&& rand) 
    {
        if (first == last) return;
        
        using difference_type = typename iterator_traits<RandomIt>::difference_type;
        for (RandomIt i = first + 1; i != last; ++i) 
        {
            difference_type j = rand((i - first) + 1);  // 使用自定义随机数生成器
            if (j != (i - first)) 
            {
                mystl::iter_swap(first + j, i);
            }
        }
    }

    

    /*****************************************************************************************/
    // 稳定排序算法
    /*****************************************************************************************/

    /*****************************************************************************************/
    // insertion_sort (稳定)
    // 插入排序，假设当前元素之前的元素已经有序，将当前元素插入到前面合适的位置
    /*****************************************************************************************/
    template<class RandomIt, class Compare>
    void insertion_sort(RandomIt first, RandomIt last, Compare comp)
    {
        if (first == last) return;
        
        for (auto i = first + 1; i != last; ++i)
        {
            auto key = mystl::move(*i);
            auto j = i;
            
            for (; j != first && comp(key, *(j - 1)); --j)
            {
                *j = mystl::move(*(j - 1));
            }
            *j = mystl::move(key);
        }
    }

    template<class RandomIt>
    void insertion_sort(RandomIt first, RandomIt last)
    {
        insertion_sort(first, last, mystl::less<typename iterator_traits<RandomIt>::value_type>());
    }


    /*****************************************************************************************/
    // merge_sort (稳定)
    // 归并排序，递归将两个有序的序列合并成一个有序的序列
    /*****************************************************************************************/
    template<class RandomIt, class Compare>
    void merge_sort_impl(RandomIt first, RandomIt middle, RandomIt last,
                         typename iterator_traits<RandomIt>::value_type* buffer,
                         Compare comp)
    {
        RandomIt first1 = first;
        RandomIt first2 = middle;
        typename iterator_traits<RandomIt>::value_type* buffer_ptr = buffer;

        while (first1 != middle && first2 != last)
        {
            if (comp(*first2, *first1))
                *buffer_ptr++ = mystl::move(*first2++);
            else
                *buffer_ptr++ = mystl::move(*first1++);
        }
        if (first1 != middle)
            mystl::move(first1, middle, buffer_ptr);
        else
            mystl::move(first2, last, buffer_ptr);
        mystl::move(buffer, buffer + (last - first), first);
    }

    template<class RandomIt, class Compare>
    void merge_sort(RandomIt first, RandomIt last, Compare comp)
    {
        if (last - first < 2) return;
        
        auto len = last - first;
        auto middle = first + len / 2;
        
        // 分配临时缓冲区
        auto buffer = new typename iterator_traits<RandomIt>::value_type[len];
        
        merge_sort(first, middle, comp);
        merge_sort(middle, last, comp);
        merge_sort_impl(first, middle, last, buffer, comp);
        
        delete[] buffer;
    }

    template<class RandomIt>
    void merge_sort(RandomIt first, RandomIt last)
    {
        merge_sort(first, last, mystl::less<typename iterator_traits<RandomIt>::value_type>());
    }


    /*****************************************************************************************/
    // bubble_sort (稳定)
    // 冒泡排序，每次找出最小的元素，移动到最前面
    /*****************************************************************************************/
    template<class RandomIt, class Compare>
    void bubble_sort(RandomIt first, RandomIt last, Compare comp)
    {
        if (first == last) return;
        
        for (auto i = first; i != last - 1; ++i)
        {
            bool swapped = false;
            for (auto j = last - 1; j != i; --j)
            {
                if (comp(*j, *(j - 1)))
                {
                    mystl::iter_swap(j, j - 1);
                    swapped = true;
                }
            }
            if (!swapped) break;  // 如果没有交换，说明已经有序
        }
    }

    template<class RandomIt>
    void bubble_sort(RandomIt first, RandomIt last)
    {
        bubble_sort(first, last, mystl::less<typename iterator_traits<RandomIt>::value_type>());
    }



    /*****************************************************************************************/
    // 不稳定排序算法
    /*****************************************************************************************/

    /*****************************************************************************************/
    // quick_sort (不稳定)
    // 快速排序，令first为基准值，将小于基准值的元素放在基准值左边，大于基准值的元素放在基准值右边，递归处理直到只有两个元素
    /*****************************************************************************************/
    template<class RandomIt, class Compare>
    void quick_sort_impl(RandomIt first, RandomIt last, Compare comp)
    {
        if (first >= last) return;
        
        auto pivot = mystl::move(*first);
        auto i = first;
        auto j = last - 1;
        
        while (i < j)
        {
            while (i < j && !comp(pivot, *j)) --j;
            if (i < j) *i = mystl::move(*j);
            
            while (i < j && comp(*i, pivot)) ++i;
            if (i < j) *j = mystl::move(*i);
        }
        *i = mystl::move(pivot);
        
        quick_sort_impl(first, i, comp);
        quick_sort_impl(i + 1, last, comp);
    }

    template<class RandomIt, class Compare>
    void quick_sort(RandomIt first, RandomIt last, Compare comp)
    {
        if (last - first > 16)
        {
            quick_sort_impl(first, last, comp);
        }
        else
        {
            insertion_sort(first, last, comp);  // 小数组使用插入排序
        }
    }

    template<class RandomIt>
    void quick_sort(RandomIt first, RandomIt last)
    {
        quick_sort(first, last, mystl::less<typename iterator_traits<RandomIt>::value_type>());
    }


    /*****************************************************************************************/
    // heap_sort (不稳定)
    // 堆排序，建堆，然后每次将堆顶元素与最后一个元素交换，然后调整堆，弹出的堆顶元素就是有序的
    /*****************************************************************************************/
    // 上溯，用于调整堆
    template<class RandomIt, class Compare>
    void up_heap(RandomIt first, 
                 typename iterator_traits<RandomIt>::difference_type holeIndex,
                 typename iterator_traits<RandomIt>::difference_type topIndex,
                 Compare comp)
    {
        auto value = mystl::move(*(first + holeIndex));
        while (holeIndex > topIndex && comp(*(first + (holeIndex - 1) / 2), value))
        {
            *(first + holeIndex) = mystl::move(*(first + (holeIndex - 1) / 2));
            holeIndex = (holeIndex - 1) / 2;
        }
        *(first + holeIndex) = mystl::move(value);
    }

    // 下溯，用于调整堆
    template<class RandomIt, class Compare>
    void down_heap(RandomIt first,
                   typename iterator_traits<RandomIt>::difference_type holeIndex,
                   typename iterator_traits<RandomIt>::difference_type len,
                   Compare comp)
    {
        auto value = mystl::move(*(first + holeIndex));
        typename iterator_traits<RandomIt>::difference_type child;
        while (holeIndex < (len - 1) / 2)
        {
            child = 2 * holeIndex + 1;
            if (child + 1 < len && comp(*(first + child), *(first + child + 1)))
                ++child;
            if (!comp(value, *(first + child)))
                break;
            *(first + holeIndex) = mystl::move(*(first + child));
            holeIndex = child;
        }
        *(first + holeIndex) = mystl::move(value);
    }

    template<class RandomIt, class Compare>
    void heap_sort(RandomIt first, RandomIt last, Compare comp)
    {
        if (last - first < 2) return;
        
        // 建堆
        auto len = last - first;
        for (typename iterator_traits<RandomIt>::difference_type i = (len - 2) / 2; i >= 0; --i)
            down_heap(first, i, len, comp);
        
        // 排序
        while (len > 1)
        {
            --len;
            mystl::iter_swap(first, first + len);
            down_heap(first, 0, len, comp);
        }
    }

    template<class RandomIt>
    void heap_sort(RandomIt first, RandomIt last)
    {
        heap_sort(first, last, mystl::less<typename iterator_traits<RandomIt>::value_type>());
    }


    /*****************************************************************************************/
    // selection_sort (不稳定)
    // 双向选择排序，每次同时找出最大值和最小值，最小值放在前面，最大值放在后面
    /*****************************************************************************************/
    template<class RandomIt, class Compare>
    void selection_sort(RandomIt first, RandomIt last, Compare comp)
    {
        if (first == last) return;
        
        while (first < last)
        {
            auto min_pos = first;
            auto max_pos = first;
            auto i = first + 1;
            
            // 同时寻找最大值和最小值
            while (i != last)
            {
                if (comp(*i, *min_pos))
                    min_pos = i;
                else if (comp(*max_pos, *i))
                    max_pos = i;
                ++i;
            }
            
            // 如果最小值不在开头，交换到开头
            if (min_pos != first)
                mystl::iter_swap(first, min_pos);
                
            // 如果最大值是first(已被交换)，更新为min_pos的位置
            if (max_pos == first)
                max_pos = min_pos;
                
            // 如果最大值不在末尾，交换到末尾
            --last;
            if (max_pos != last)
                mystl::iter_swap(last, max_pos);
                
            ++first;
        }
    }

    template<class RandomIt>
    void selection_sort(RandomIt first, RandomIt last)
    {
        selection_sort(first, last, mystl::less<typename iterator_traits<RandomIt>::value_type>());
    }


    /*****************************************************************************************/
    // shell_sort (不稳定)
    // 希尔排序，使用不同的间隔对子序列进行插入排序，最后一次间隔为1就是普通插入排序
    /*****************************************************************************************/
    template<class RandomIt, class Compare>
    void shell_sort(RandomIt first, RandomIt last, Compare comp)
    {
        if (first == last) return;
        
        auto len = last - first;
        for (auto gap = len / 2; gap > 0; gap /= 2)  // 每次将间隔减半
        {
            // 对每个子序列进行插入排序
            for (auto i = first + gap; i != last; ++i)
            {
                auto temp = mystl::move(*i);
                auto j = i;
                
                for (; j >= first + gap && comp(temp, *(j - gap)); j -= gap)
                {
                    *j = mystl::move(*(j - gap));
                }
                *j = mystl::move(temp);
            }
        }
    }

    template<class RandomIt>
    void shell_sort(RandomIt first, RandomIt last)
    {
        shell_sort(first, last, mystl::less<typename iterator_traits<RandomIt>::value_type>());
    }


    /*****************************************************************************************/
    // introsort (不稳定)
    // 内省排序，结合了快速排序、堆排序和插入排序的优点
    /*****************************************************************************************/
    template<class RandomIt, class Compare>
    void introsort_impl(RandomIt first, RandomIt last, Compare comp, int depth)
    {
        auto len = last - first;
        if (len < 16)
        {
            insertion_sort(first, last, comp);  // 小数组使用插入排序
            return;
        }
        
        if (depth == 0)
        {
            heap_sort(first, last, comp);  // 递归太深时改用堆排序
            return;
        }
        
        // 否则使用快速排序
        auto pivot = mystl::move(*first);
        auto i = first;
        auto j = last - 1;
        
        while (i < j)
        {
            while (i < j && !comp(pivot, *j)) --j;
            if (i < j) *i = mystl::move(*j);
            
            while (i < j && comp(*i, pivot)) ++i;
            if (i < j) *j = mystl::move(*i);
        }
        *i = mystl::move(pivot);
        
        introsort_impl(first, i, comp, depth - 1);
        introsort_impl(i + 1, last, comp, depth - 1);
    }

    template<class RandomIt, class Compare>
    void introsort(RandomIt first, RandomIt last, Compare comp)
    {
        if (first == last) return;
        
        // 计算最大递归深度，通常取 2 * log2(n)
        int depth = 2 * static_cast<int>(std::log2(last - first));
        introsort_impl(first, last, comp, depth);
    }

    template<class RandomIt>
    void introsort(RandomIt first, RandomIt last)
    {
        introsort(first, last, mystl::less<typename iterator_traits<RandomIt>::value_type>());
    }


    /*****************************************************************************************/
    // sort
    // 对 [first, last) 进行排序，使用内省排序
    /*****************************************************************************************/
    template<class RandomIt>
    void sort(RandomIt first, RandomIt last)
    {
        mystl::introsort(first, last);
    }

    // 重载版本使用函数对象 comp 代替比较操作
    template<class RandomIt, class Compare>
    void sort(RandomIt first, RandomIt last, Compare comp)
    {
        mystl::introsort(first, last, comp);
    }



    /*****************************************************************************************/
    // is_sorted
    // 检查序列是否已按升序排序
    /*****************************************************************************************/
    template<class ForwardIt>
    bool is_sorted(ForwardIt first, ForwardIt last)
    {
        if (first == last)
            return true;
            
        ForwardIt next = first;
        while (++next != last)
        {
            if (*next < *first)  // 如果后一个元素小于前一个元素，则不是有序的
                return false;
            first = next;
        }
        return true;
    }

    // 重载版本使用函数对象 comp 代替比较操作
    template<class ForwardIt, class Compare>
    bool is_sorted(ForwardIt first, ForwardIt last, Compare comp)
    {
        if (first == last)
            return true;
            
        ForwardIt next = first;
        while (++next != last)
        {
            if (comp(*next, *first))  // 如果后一个元素小于前一个元素，则不是有序的
                return false;
            first = next;
        }
        return true;
    }

    

    /*****************************************************************************************/
    // is_sorted_until
    // 找到最长的已排序序列，返回第一个不满足排序的位置
    /*****************************************************************************************/
    template<class ForwardIt>
    ForwardIt is_sorted_until(ForwardIt first, ForwardIt last)
    {
        if (first == last)
            return last;
            
        ForwardIt next = first;
        while (++next != last)
        {
            if (*next < *first)  // 找到第一个不满足升序的位置
                return next;
            first = next;
        }
        return last;
    }

    // 重载版本使用函数对象 comp 代替比较操作
    template<class ForwardIt, class Compare>
    ForwardIt is_sorted_until(ForwardIt first, ForwardIt last, Compare comp)
    {
        if (first == last)
            return last;
            
        ForwardIt next = first;
        while (++next != last)
        {
            if (comp(*next, *first))  // 找到第一个不满足排序规则的位置
                return next;
            first = next;
        }
        return last;
    }



    /*****************************************************************************************/
    // 二分查找算法
    /*****************************************************************************************/

    /*****************************************************************************************/
    // lower_bound
    // 在已排序的 [first, last) 中查找第一个大于等于 value 的元素位置
    /*****************************************************************************************/
    template<class ForwardIt, class T, class Compare>
    ForwardIt lower_bound(ForwardIt first, ForwardIt last, const T& value, Compare comp)
    {
        auto len = mystl::distance(first, last);
        
        while (len > 0)
        {
            auto half = len / 2;
            auto middle = first;
            mystl::advance(middle, half);
            
            if (comp(*middle, value))    // 如果中间值小于value，则value在中间值的右边
            {
                first = middle;
                ++first;
                len = len - half - 1;
            }
            else
            {
                len = half;
            }
        }
        return first;
    }

    template<class ForwardIt, class T>
    ForwardIt lower_bound(ForwardIt first, ForwardIt last, const T& value)
    {
        return lower_bound(first, last, value, mystl::less<typename iterator_traits<ForwardIt>::value_type>());
    }


    /*****************************************************************************************/
    // upper_bound
    // 在已排序的 [first, last) 中查找第一个大于 value 的元素位置
    /*****************************************************************************************/
    template<class ForwardIt, class T, class Compare>
    ForwardIt upper_bound(ForwardIt first, ForwardIt last, const T& value, Compare comp)
    {
        auto len = mystl::distance(first, last);
        
        while (len > 0)
        {
            auto half = len / 2;
            auto middle = first;
            mystl::advance(middle, half);
            
            if (!comp(value, *middle))
            {
                first = middle;
                ++first;
                len = len - half - 1;
            }
            else
            {
                len = half;
            }
        }
        return first;
    }

    template<class ForwardIt, class T>
    ForwardIt upper_bound(ForwardIt first, ForwardIt last, const T& value)
    {
        return upper_bound(first, last, value, mystl::less<typename iterator_traits<ForwardIt>::value_type>());
    }


    /*****************************************************************************************/
    // binary_search
    // 在已排序的 [first, last) 中查找等于 value 的元素，存在返回 true，否则返回 false
    /*****************************************************************************************/
    template<class ForwardIt, class T, class Compare>
    bool binary_search(ForwardIt first, ForwardIt last, const T& value, Compare comp)
    {
        first = mystl::lower_bound(first, last, value, comp);
        return (first != last && !comp(value, *first));
    }

    template<class ForwardIt, class T>
    bool binary_search(ForwardIt first, ForwardIt last, const T& value)
    {
        return binary_search(first, last, value, mystl::less<typename iterator_traits<ForwardIt>::value_type>());
    }


    /*****************************************************************************************/
    // equal_range
    // 在已排序的 [first, last) 中查找等于 value 的元素所在区间，返回一对迭代器指向区间首尾
    /*****************************************************************************************/
    template<class ForwardIt, class T, class Compare>
    mystl::pair<ForwardIt, ForwardIt>
    equal_range(ForwardIt first, ForwardIt last, const T& value, Compare comp)
    {
        return mystl::pair<ForwardIt, ForwardIt>(
            mystl::lower_bound(first, last, value, comp), 
            mystl::upper_bound(first, last, value, comp));
    }

    template<class ForwardIt, class T>
    mystl::pair<ForwardIt, ForwardIt> 
    equal_range(ForwardIt first, ForwardIt last, const T& value)
    {
        return equal_range(first, last, value, mystl::less<typename iterator_traits<ForwardIt>::value_type>());
    }

} // namespace mystl 