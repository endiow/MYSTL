# 迭代器

## 迭代器标签

```cpp
// 五种迭代器类型的标签
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};
```

继承关系：
- input_iterator_tag ← forward_iterator_tag ← bidirectional_iterator_tag ← random_access_iterator_tag
- output_iterator_tag（独立）



## 迭代器基类

```cpp
template<class Category, class T, class Distance = ptrdiff_t,
         class Pointer = T*, class Reference = T&>
struct iterator 
{
    using iterator_category = Category;  // 迭代器类型
    using value_type = T;               // 元素类型
    using difference_type = Distance;    // 距离类型
    using pointer = Pointer;            // 指针类型
    using reference = Reference;        // 引用类型
};
```



## 迭代器特性萃取

### SFINAE 检测迭代器类型

```cpp
template<class T>
struct has_iterator_type 
{
private:
    // 第一个重载：如果 U 有 iterator_category 类型，这个会被选中
    template<class U> 
    static auto check(int) noexcept -> typename U::iterator_category;
    
    // 第二个重载：如果第一个重载失败，这个会被选中
    template<class U> 
    static void check(...) noexcept;

public:
    // 检查返回类型是否是 void，取反得到最终结果
    static constexpr bool value = !is_same_v<void, decltype(check<T>(0))>;
};
```

工作原理：
1. `check(int)` 尝试访问类型的 iterator_category
   - 如果类型有 iterator_category，返回该类型
   - 如果没有，这个重载会失败

2. `check(...)` 是后备方案
   - 当第一个重载失败时被选中
   - 返回 void

3. `decltype(check<T>(0))` 获取实际被选中的重载的返回类型

4. `!is_same_v<void, ...>` 检查结果
   - 如果返回 void（第二个重载），则不是迭代器
   - 如果返回其他类型（第一个重载），则是迭代器

`在这种 SFINAE 检测中，我们只需要声明，不需要定义，可以加上 noexcept 确保正确`



### iterator_traits 的实现

```cpp
// 基础实现，根据是否有迭代器类型选择不同的实现
template<class Iter, bool>
struct iterator_traits_impl {};

// 有迭代器类型的版本
template<class Iter>
struct iterator_traits_impl<Iter, true> 
{
    using iterator_category = typename Iter::iterator_category;
    using value_type = typename Iter::value_type;
    using difference_type = typename Iter::difference_type;
    using pointer = typename Iter::pointer;
    using reference = typename Iter::reference;
};

// 使用 has_iterator_type 来选择实现
template<class Iter>
struct iterator_traits : 
    public iterator_traits_impl<Iter, has_iterator_type<Iter>::value> {};
```

工作流程：
1. 使用 `has_iterator_type` 检测类型是否具有迭代器特性
2. 根据检测结果选择不同的实现：
   - true：使用类型自己定义的特性
   - false：使用特化版本（如指针特化）

3. 对于原生指针和 const 指针提供特化版本：
```cpp
// 原生指针特化
template<class T>
struct iterator_traits<T*> 
{
    using iterator_category = random_access_iterator_tag;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    using reference = T&;
};

// const 指针特化
template<class T>
struct iterator_traits<const T*> 
{
    using iterator_category = random_access_iterator_tag;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = const T*;
    using reference = const T&;
};
```

这样的设计可以：
1. 统一处理迭代器和指针
2. 在编译期完成类型检测和选择
3. 提供类型安全的特性萃取
4. 支持自定义迭代器类型




## 迭代器辅助函数

### advance

```cpp
// 迭代器前进 n 步
template<class InputIt, class Distance>
void advance(InputIt& it, Distance n) 
{
    using category = typename iterator_traits<InputIt>::iterator_category;
    advance_dispatch(it, n, category());
}

// 随机访问迭代器：直接加 n
template<class RandomIt, class Distance>
void advance_dispatch(RandomIt& it, Distance n, random_access_iterator_tag) 
{
    it += n;
}

// 双向迭代器：根据 n 的正负选择方向
template<class BidirIt, class Distance>
void advance_dispatch(BidirIt& it, Distance n, bidirectional_iterator_tag) 
{
    if (n >= 0) {
        while (n--) ++it;
    } else {
        while (n++) --it;
    }
}

// 前向迭代器：只能向前
template<class InputIt, class Distance>
void advance_dispatch(InputIt& it, Distance n, input_iterator_tag) 
{
    while (n--) ++it;
}
```



### distance

```cpp
// 计算两个迭代器之间的距离
template<class InputIt>
typename iterator_traits<InputIt>::difference_type
distance(InputIt first, InputIt last) 
{
    using category = typename iterator_traits<InputIt>::iterator_category;
    return distance_dispatch(first, last, category());
}

// 随机访问迭代器：直接相减
template<class RandomIt>
typename iterator_traits<RandomIt>::difference_type
distance_dispatch(RandomIt first, RandomIt last, random_access_iterator_tag) 
{
    return last - first;
}

// 其他迭代器：逐个遍历计数
template<class InputIt>
typename iterator_traits<InputIt>::difference_type
distance_dispatch(InputIt first, InputIt last, input_iterator_tag) 
{
    typename iterator_traits<InputIt>::difference_type n = 0;
    while (first != last) {
        ++first;
        ++n;
    }
    return n;
}
```



## 反向迭代器

```cpp
template<class Iterator>
class reverse_iterator 
{
private:
    Iterator current;  // 指向正向迭代器的下一个位置

public:
    using iterator_type = Iterator;
    using iterator_category = typename iterator_traits<Iterator>::iterator_category;
    using value_type = typename iterator_traits<Iterator>::value_type;
    using difference_type = typename iterator_traits<Iterator>::difference_type;
    using pointer = typename iterator_traits<Iterator>::pointer;
    using reference = typename iterator_traits<Iterator>::reference;

    // 反向迭代器的操作实现...
    reference operator*() const { Iterator tmp = current; return *--tmp; }
    pointer operator->() const { return &(operator*()); }
    
    reverse_iterator& operator++() { --current; return *this; }
    reverse_iterator& operator--() { ++current; return *this; }
    // ...
};
```

反向迭代器通过包装正向迭代器，实现反向遍历。其中：
1. `current` 总是指向要访问的元素的下一个位置
2. `operator*` 返回 current 前一个位置的元素
3. `operator++` 实际上是向后移动 current
4. `operator--` 实际上是向前移动 current