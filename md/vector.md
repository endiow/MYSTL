# vector

vector 是一个动态数组，能够自动扩展容量。



## 模板参数

- `T`: 元素类型
- `Alloc`: 分配器类型，默认使用 `mystl::allocator<T>`



## 迭代器

- `iterator`: 随机访问迭代器
- `const_iterator`: 常量随机访问迭代器
- `reverse_iterator`: 反向迭代器
- `const_reverse_iterator`: 常量反向迭代器



## 构造函数

- `vector()`: 默认构造函数
- `vector(size_type count)`: 构造包含 count 个默认元素的容器
- `vector(size_type count, const T& value)`: 构造包含 count 个值为 value 的元素的容器
- `vector(InputIt first, InputIt last)`: 用迭代器范围构造容器
- `vector(const vector& other)`: 拷贝构造函数
- `vector(vector&& other)`: 移动构造函数
- `vector(std::initializer_list<T> init)`: 使用初始化列表构造



## 赋值操作

- `vector& operator=(const vector& other)`: 拷贝赋值

- `vector& operator=(vector&& other)`: 移动赋值

- `void assign(size_type count, const T& value)`: 替换为 count 个值为 value 的元素

- `void assign(InputIt first, InputIt last)`: 替换为迭代器范围内的元素

- `void assign(std::initializer_list<T> ilist)`: 替换为初始化列表中的元素

`assign()提供异常安全保证`



## 元素访问

- `reference operator[](size_type pos)`: 访问指定位置的元素
- `const_reference operator[](size_type pos) const`: 访问指定位置的元素（常量版本）
- `reference at(size_type pos)`: 带边界检查的元素访问
- `const_reference at(size_type pos) const`: 带边界检查的元素访问（常量版本）
- `reference front()`: 访问第一个元素
- `const_reference front() const`: 访问第一个元素（常量版本）
- `reference back()`: 访问最后一个元素
- `const_reference back() const`: 访问最后一个元素（常量版本）
- `T* data()`: 返回指向内存的指针
- `const T* data() const`: 返回指向内存的指针（常量版本）



## 迭代器操作

- `iterator begin()`: 返回指向首元素的迭代器
- `const_iterator begin() const`: 返回指向首元素的常量迭代器
- `iterator end()`: 返回指向末元素后一位置的迭代器
- `const_iterator end() const`: 返回指向末元素后一位置的常量迭代器
- `reverse_iterator rbegin()`: 返回指向末元素的反向迭代器
- `const_reverse_iterator rbegin() const`: 返回指向末元素的常量反向迭代器
- `reverse_iterator rend()`: 返回指向首元素前一位置的反向迭代器
- `const_reverse_iterator rend() const`: 返回指向首元素前一位置的常量反向迭代器



## 容量操作

- `bool empty() const`: 检查容器是否为空

- `size_type size() const`: 返回容器中的元素数

- `size_type capacity() const`: 返回当前分配的存储容量

- `void reserve(size_type new_cap)`: 预留存储空间

- `void shrink_to_fit()`: 通过释放未使用的内存来减少内存的使用

- `void resize(size_type count)`: 改变容器中元素的个数

- `void resize(size_type count, const value_type& value)`: 改变容器中元素的个数，并指定新元素的值

`reserve(), resize()提供异常安全保证`



## 修改器

- `void clear()`: 清除所有元素
- `iterator insert(const_iterator pos, const T& value)`: 插入元素
- `iterator insert(const_iterator pos, T&& value)`: 插入右值元素
- `iterator insert(const_iterator pos, size_type count, const T& value)`: 插入多个元素
- `iterator insert(const_iterator pos, InputIt first, InputIt last)`: 插入范围内的元素
- `iterator insert(const_iterator pos, std::initializer_list<T> ilist)`: 插入初始化列表中的元素
- `template<class... Args> iterator emplace(const_iterator pos, Args&&... args)`: 在指定位置原地构造元素
- `template<class... Args> void emplace_back(Args&&... args)`: 在末尾原地构造元素
- `void push_back(const T& value)`: 在末尾添加元素
- `void push_back(T&& value)`: 在末尾添加右值元素
- `void pop_back()`: 移除末尾元素
- `iterator erase(const_iterator pos)`: 移除指定位置的元素
- `iterator erase(const_iterator first, const_iterator last)`: 移除范围内的元素
- `void swap(vector& other)`: 交换内容

`push_back(), emplace(), insert()提供异常安全保证。`



## 非成员函数

- `operator==`, `operator!=`: 相等/不相等比较
- `operator<`, `operator<=`, `operator>`, `operator>=`: 大小比较
- `swap(vector& lhs, vector& rhs)`: 交换两个 vector 的内容



## 异常安全

- 提供基本异常安全保证（不考虑move操作是否会改变数据时，提供强异常安全保证）
- 抛出异常时，原数据不变，内存不变
- 在内存分配失败时会抛出异常
- 在元素构造失败时会回滚操作