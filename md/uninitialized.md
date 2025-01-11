# 未初始化内存操作

提供在未初始化内存上构造对象的算法。



## 主要功能

### 复制操作

- `uninitialized_copy`: 在未初始化内存空间上复制一个序列
- `uninitialized_copy_n`: 在未初始化内存空间上复制 n 个元素
- `uninitialized_move`: 在未初始化内存空间上移动一个序列
- `uninitialized_move_n`: 在未初始化内存空间上移动 n 个元素



### 填充操作

- `uninitialized_fill`: 在未初始化内存空间上填充元素
- `uninitialized_fill_n`: 在未初始化内存空间上填充 n 个元素



### 构造操作

- `uninitialized_default_construct`: 在未初始化内存空间上默认构造元素
- `uninitialized_value_construct`: 在未初始化内存空间上值构造元素



## 异常安全

所有操作提供基本异常安全保证：
- 如果构造过程中抛出异常，已构造的对象会被正确析构
- 不会发生内存泄漏
- 但可能会留下部分已构造的对象



## 优化策略

### 平凡类型优化

对于可平凡复制/移动的类型：
```cpp
template<class InputIt, class ForwardIt>
ForwardIt 
uninitialized_copy_aux(InputIt first, InputIt last, ForwardIt d_first, true_type) 
{
    return mystl::copy(first, last, d_first);  // 直接使用 copy
}
```



### 单字节类型优化

对于 char、signed char、unsigned char：
```cpp
template<class ForwardIt, class T>
typename enable_if<
    is_byte_type<typename iterator_traits<ForwardIt>::value_type>::value &&
    is_byte_type<T>::value, void>::type
uninitialized_fill(ForwardIt first, ForwardIt last, const T& value) 
{
    std::memset(addressof(*first), static_cast<unsigned char>(value), last - first);
}
```



### 异常处理

对于可能抛出异常的操作：
```cpp
template<class InputIt, class ForwardIt>
ForwardIt 
uninitialized_copy_aux(InputIt first, InputIt last, ForwardIt d_first, false_type) 
{
    ForwardIt current = d_first;
    try 
    {
        for (; first != last; ++first, ++current)
        {
            mystl::construct(addressof(*current), *first);
        }
        return current;
    }
    catch (...) 
    {
        destroy(d_first, current);  // 清理已构造的对象
        throw;
    }
}
```



## 实现细节

### 类型萃取

使用 type_traits 判断类型特性：
- `is_trivially_copy_assignable`: 是否可平凡复制
- `is_trivially_move_assignable`: 是否可平凡移动
- `is_byte_type`: 是否是单字节类型



### 内存操作

- `construct`: 在已分配内存上构造对象
- `destroy`: 析构对象但不释放内存
- `destroy_at`: 析构单个对象
- `addressof`: 获取对象的实际地址



### SFINAE 技术

使用 enable_if 在编译期选择最优实现：
```cpp
template<class ForwardIt, class T>
typename enable_if<!
    (is_byte_type<typename iterator_traits<ForwardIt>::value_type>::value &&
     is_byte_type<T>::value), void>::type
uninitialized_fill(ForwardIt first, ForwardIt last, const T& value);
```



## 使用示例

```cpp
// 在未初始化内存上构造对象
alignas(T) unsigned char buffer[sizeof(T) * n];
T* ptr = reinterpret_cast<T*>(buffer);

// 使用 uninitialized_fill 填充值
mystl::uninitialized_fill(ptr, ptr + n, value);

// 使用完后需要手动析构
mystl::destroy(ptr, ptr + n);
```