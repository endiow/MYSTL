#pragma once

#include "expectdef.hpp"
#include "type_traits.hpp"

namespace mystl 
{
    // 五种迭代器类型
    struct input_iterator_tag {};        // 只读迭代器
    struct output_iterator_tag {};       // 只写迭代器
    struct forward_iterator_tag : public input_iterator_tag {};  // 前向迭代器
    struct bidirectional_iterator_tag : public forward_iterator_tag {};  // 双向迭代器
    struct random_access_iterator_tag : public bidirectional_iterator_tag {};  // 随机访问迭代器

    // 迭代器基类
    template<class Category, class T, class Distance = mystl::ptrdiff_t, class Pointer = T*, class Reference = T&>
    struct iterator 
    {
        using iterator_category = Category;  // 迭代器类别
        using value_type = T;               // 元素类型
        using difference_type = Distance;    // 距离类型
        using pointer = Pointer;            // 指针类型
        using reference = Reference;         // 引用类型
    };

    // 迭代器萃取
    template<class T>
    struct has_iterator_type 
    {
    private:
        // 检查是否有迭代器类型
        template<class U> 
        static auto check_iter(int) -> decltype(typename U::iterator_category(), std::true_type());
        
        // 检查是否是指针
        template<class U>
        static auto check_ptr(int) -> typename std::enable_if<std::is_pointer<U>::value, std::true_type>::type;
        
        // 默认情况
        template<class U> 
        static std::false_type check_iter(...);
        template<class U> 
        static std::false_type check_ptr(...);

    public:
        static constexpr bool value = decltype(check_iter<T>(0))::value || decltype(check_ptr<T>(0))::value;
    };

    template<class Iter, bool>
    struct iterator_traits_impl {};

    template<class Iter>
    struct iterator_traits_impl<Iter, true> 
    {
        using iterator_category = typename Iter::iterator_category;
        using value_type = typename Iter::value_type;
        using difference_type = typename Iter::difference_type;
        using pointer = typename Iter::pointer;
        using reference = typename Iter::reference;
    };

    template<class Iter>
    struct iterator_traits : 
        public iterator_traits_impl<Iter, has_iterator_type<Iter>::value> {};

    // 原生指针特化
    template<class T>
    struct iterator_traits<T*> 
    {
        using iterator_category = random_access_iterator_tag;
        using value_type = std::remove_cv_t<T>;
        using difference_type = ptrdiff_t;
        using pointer = T*;
        using reference = T&;
    };

    template<class T>
    struct iterator_traits<const T*> 
    {
        using iterator_category = random_access_iterator_tag;
        using value_type = std::remove_cv_t<T>;
        using difference_type = ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;
    };

    // 迭代器类别判断
    template<class Iterator, class = void>
    struct is_input_iterator : false_type {};

    template<class Iterator>
    // has_iterator_type<Iterator>::value返回true时，才使用Iterator的类型
    //即验证迭代器是否是输入迭代器前提，必须是迭代器
    struct is_input_iterator<Iterator, typename std::enable_if<has_iterator_type<Iterator>::value>::type> 
        : bool_constant<is_convertible<typename iterator_traits<Iterator>::iterator_category, input_iterator_tag>::value> {};

    template<class Iterator, class = void>
    struct is_output_iterator : false_type {};

    template<class Iterator>
    struct is_output_iterator<Iterator, typename std::enable_if<has_iterator_type<Iterator>::value>::type> 
        : bool_constant<is_convertible<typename iterator_traits<Iterator>::iterator_category, output_iterator_tag>::value> {};

    template<class Iterator, class = void>
    struct is_forward_iterator : false_type {};

    template<class Iterator>
    struct is_forward_iterator<Iterator, typename std::enable_if<has_iterator_type<Iterator>::value>::type> 
        : bool_constant<is_convertible<typename iterator_traits<Iterator>::iterator_category, forward_iterator_tag>::value> {};

    template<class Iterator, class = void>
    struct is_bidirectional_iterator : false_type {};

    template<class Iterator>
    struct is_bidirectional_iterator<Iterator, typename std::enable_if<has_iterator_type<Iterator>::value>::type> 
        : bool_constant<is_convertible<typename iterator_traits<Iterator>::iterator_category, bidirectional_iterator_tag>::value> {};

    template<class Iterator, class = void>
    struct is_random_access_iterator : false_type {};

    template<class Iterator>
    struct is_random_access_iterator<Iterator, typename std::enable_if<has_iterator_type<Iterator>::value>::type> 
        : bool_constant<is_convertible<typename iterator_traits<Iterator>::iterator_category, random_access_iterator_tag>::value> {};

    // 变量模板
    template<class Iterator>
    inline constexpr bool is_input_iterator_v = is_input_iterator<Iterator>::value;

    template<class Iterator>
    inline constexpr bool is_output_iterator_v = is_output_iterator<Iterator>::value;

    template<class Iterator>
    inline constexpr bool is_forward_iterator_v = is_forward_iterator<Iterator>::value;

    template<class Iterator>
    inline constexpr bool is_bidirectional_iterator_v = is_bidirectional_iterator<Iterator>::value;

    template<class Iterator>
    inline constexpr bool is_random_access_iterator_v = is_random_access_iterator<Iterator>::value;

    // 获取迭代器类别
    template<class Iter>
    inline typename iterator_traits<Iter>::iterator_category
    iterator_category(const Iter&) 
    {
        using Category = typename iterator_traits<Iter>::iterator_category;
        return Category{};
    }

    // 获取迭代器距离类型
    template<class Iter>
    inline typename iterator_traits<Iter>::difference_type*
    difference_type(const Iter&) 
    {
        return static_cast<typename iterator_traits<Iter>::difference_type*>(nullptr);
    }

    // 获取迭代器元素类型
    template<class Iter>
    inline typename iterator_traits<Iter>::value_type*
    value_type(const Iter&) 
    {
        return static_cast<typename iterator_traits<Iter>::value_type*>(nullptr);
    }


    // 计算迭代器距离
    // 输入迭代器只能通过++操作来移动
    template<typename InputIterator>
    inline typename iterator_traits<InputIterator>::difference_type
    distance_dispatch(InputIterator first, InputIterator last, input_iterator_tag) 
    {
        typename iterator_traits<InputIterator>::difference_type n = 0;
        while (first != last) 
        {
            ++first;
            ++n;
        }
        return n;
    }

    // 随机访问迭代器可以通过算术运算来计算距离
    template<typename RandomAccessIterator>
    inline typename iterator_traits<RandomAccessIterator>::difference_type
    distance_dispatch(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag) 
    {
        return last - first;
    }

    template<class InputIterator>
    inline typename iterator_traits<InputIterator>::difference_type
    distance(InputIterator first, InputIterator last) 
    {
        using category = typename iterator_traits<InputIterator>::iterator_category;
        return distance_dispatch(first, last, category());
    }

    // 移动迭代器
    // 输入迭代器只能通过++操作来移动
    template<typename InputIterator, typename Distance>
    inline void advance_dispatch(InputIterator& i, Distance n, input_iterator_tag) 
    {
        while (n--) ++i;
    }

    // 双向迭代器可以通过++和--操作来移动
    template<typename BidirectionalIterator, typename Distance>
    inline void advance_dispatch(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag) 
    {
        if (n >= 0)
            while (n--) ++i;
        else
            while (n++) --i;
    }

    // 随机访问迭代器可以通过算术运算来移动 
    template<typename RandomAccessIterator, typename Distance>
    inline void advance_dispatch(RandomAccessIterator& i, Distance n, random_access_iterator_tag) 
    {
        i += n;
    }

    template<typename InputIterator, typename Distance>
    inline void advance(InputIterator& i, Distance n) 
    {
        using category = typename iterator_traits<InputIterator>::iterator_category;
        advance_dispatch(i, n, category());
    }

    

    // 反向迭代器
    template<typename Iterator>
    class reverse_iterator 
    {
    protected:
        Iterator current;

    public:
        using iterator_type = Iterator;
        using iterator_category = typename iterator_traits<Iterator>::iterator_category;
        using value_type = typename iterator_traits<Iterator>::value_type;
        using difference_type = typename iterator_traits<Iterator>::difference_type;
        using pointer = typename iterator_traits<Iterator>::pointer;
        using reference = typename iterator_traits<Iterator>::reference;

        reverse_iterator() : current() {}
        explicit reverse_iterator(iterator_type it) : current(it) {}
        reverse_iterator(const reverse_iterator& other) : current(other.current) {}

        iterator_type base() const { return current; }

        // 反向迭代器实际对应正向迭代器的前一个位置
        reference operator*() const 
        {
            Iterator tmp = current;
            return *--tmp;
        }

        pointer operator->() const 
        {
            return &(operator*());
        }

        // 反向迭代器的操作与正向迭代器相反
        reverse_iterator& operator++() 
        {
            --current;
            return *this;
        }

        reverse_iterator operator++(int) 
        {
            reverse_iterator tmp = *this;
            --current;
            return tmp;
        }

        reverse_iterator& operator--() 
        {
            ++current;
            return *this;
        }

        reverse_iterator operator--(int) 
        {
            reverse_iterator tmp = *this;
            ++current;
            return tmp;
        }

        reverse_iterator operator+(difference_type n) const 
        {
            return reverse_iterator(current - n);
        }

        reverse_iterator& operator+=(difference_type n) 
        {
            current -= n;
            return *this;
        }

        reverse_iterator operator-(difference_type n) const 
        {
            return reverse_iterator(current + n);
        }

        reverse_iterator& operator-=(difference_type n) 
        {
            current += n;
            return *this;
        }

        reference operator[](difference_type n) const 
        {
            return *(*this + n);
        }

        difference_type operator-(const reverse_iterator& other) const 
        {
            return other.base() - this->base();
        }

        difference_type operator+(const reverse_iterator& other) const 
        {
            return other.base() + this->base();
        }
    };

    // 反向迭代器的比较操作
    template<typename Iterator>
    bool operator==(const reverse_iterator<Iterator>& lhs, const reverse_iterator<Iterator>& rhs) 
    {
        return lhs.base() == rhs.base();
    }

    template<typename Iterator>
    bool operator!=(const reverse_iterator<Iterator>& lhs, const reverse_iterator<Iterator>& rhs) 
    {
        return !(lhs == rhs);
    }

    template<typename Iterator>
    bool operator<(const reverse_iterator<Iterator>& lhs, const reverse_iterator<Iterator>& rhs) 
    {
        return rhs.base() < lhs.base();
    }

    template<typename Iterator>
    bool operator<=(const reverse_iterator<Iterator>& lhs, const reverse_iterator<Iterator>& rhs) 
    {
        return !(rhs < lhs);
    }

    template<typename Iterator>
    bool operator>(const reverse_iterator<Iterator>& lhs, const reverse_iterator<Iterator>& rhs) 
    {
        return rhs < lhs;
    }

    template<typename Iterator>
    bool operator>=(const reverse_iterator<Iterator>& lhs, const reverse_iterator<Iterator>& rhs) 
    {
        return !(lhs < rhs);
    }

    // 对迭代器类型，使用其 base() 函数
    template<class Iter>
    typename iterator_traits<Iter>::pointer
    iterator_base(Iter iter, typename std::enable_if<!std::is_pointer<Iter>::value>::type* = 0) 
    {
        return iter.base();
    }

    // 对指针类型，直接返回指针本身
    template<class T>
    T* iterator_base(T* ptr, typename std::enable_if<std::is_pointer<T*>::value>::type* = 0) 
    {
        return ptr;
    }
} // namespace mystl 