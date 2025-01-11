# 基本算法

## 比较算法

- min 	

- max	

- equal（返回两区间是否相等）

- mismatch（返回一对迭代器，指向第一处不同）

- lexicographical_compare（字典序比较）

  

## 操作内存

- copy

- copy_backward（从后往前复制）

- copy_if（满足条件的元素才复制）   

- copy_n（复制n个元素）

- move

- move_backward（从后往前移动）

- fill（填充区间值为value）

- fill_n

  

### 优化：

#### **iterator_base 统一处理迭代器和指针**

为了同时支持迭代器和原生指针，提供了 iterator_base 函数：

```cpp
// 对迭代器类型，使用其 base() 函数
template<class Iter>
typename iterator_traits<Iter>::pointer
iterator_base(Iter iter) {
    return iter.base();
}

// 对指针类型，直接返回指针本身
template<class T>
T* iterator_base(T* ptr) {
    return ptr;
}
```

使用 SFINAE 技术在编译期选择正确的版本。



#### **one_byte类型（一字节）优化fill/fill_n**     	

`one_byte_type即char、signed char、unsigned char` 

1. 为 char、signed char、unsigned char 提供特化

2. 使用 memset 优化单字节类型的操作
3. 其他类型使用普通的赋值操作
4. 保持了类型安全

可以直接memset填充。

`stl标准库实现：`

1. `直接对原生指针类型进行特化`
2. `使用 inline 函数而不是 enable_if`
3. `特化版本直接使用原生指针`
4. `一般版本使用迭代器`

我的实现更通用，同时支持迭代器和指针。



#### **trivially_copy_assignable优化copy/copy_n**

trivially_copy_assignable 是一个类型特征（type trait），用于判断一个类型是否可以通过简单的内存复制来完成赋值操作。

`一个类型满足 trivially_copy_assignable 需要：`

1. `有一个平凡的拷贝赋值运算符`
2. `不能有虚函数`
3. `不能有虚基类`
4. `所有非静态成员也必须是 trivially_copy_assignable`

可以直接用memmove赋值。

`为什么用memmove而不是memcpy？`
- memcpy：源和目标区域不能重叠
- memmove：正确处理重叠区域
- 在不重叠时，编译器通常会优化 memmove 为 memcpy



#### **trivially_move_assignable优化move/move_n**

trivially_move_assignable，判断类型是否可以通过简单的内存移动来完成移动赋值。

可以直接用memmove赋值。



#### **is_trivially_move_assignable和is_trivially_copy_assignable区别**

```c++
class Example {
public:
    // 平凡拷贝赋值运算符
    Example& operator=(const Example&) = default;  // 满足 is_trivially_copy_assignable

    // 平凡移动赋值运算符
    Example& operator=(Example&&) = default;       // 满足 is_trivially_move_assignable
};
```



## 其他算法

iter_swap（交换迭代器）