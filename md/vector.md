# Vector

动态数组容器的实现。



## 模板参数

- `T`: 元素类型
- `Alloc`: 分配器类型，默认为 `mystl::allocator<T>`



## 迭代器

提供随机访问迭代器，支持：
- 递增/递减(`++`,`--`)
- 随机访问(`+`,`-`,`+=`,`-=`,`[]`)
- 解引用(`*`)
- 成员访问(`->`)



## 异常安全保证

- 提供强异常安全保证的操作：
  - `emplace`/`emplace_back`
  - `insert`
  - `resize`
  - `reserve`
- 不抛出异常的操作：
  - 移动构造/移动赋值
  - `swap`
  - `pop_back`



## 成员类型

类型 | 定义
---|---
`value_type` | `T`
`allocator_type` | `Alloc`
`size_type` | `size_t`
`difference_type` | `ptrdiff_t`
`reference` | `T&`
`const_reference` | `const T&`
`pointer` | `T*`
`const_pointer` | `const T*`
`iterator` | 随机访问迭代器
`const_iterator` | 常量随机访问迭代器
`reverse_iterator` | 反向迭代器
`const_reverse_iterator` | 常量反向迭代器



## 成员函数

### 构造/析构函数

- `vector()`: 默认构造函数
- `vector(size_type n)`: 构造包含n个默认元素的向量
- `vector(size_type n, const T& value)`: 构造包含n个value的向量
- `vector(InputIt first, InputIt last)`: 范围构造函数
- `vector(const vector& other)`: 拷贝构造函数
- `vector(vector&& other)`: 移动构造函数
- `vector(std::initializer_list<T> ilist)`: 初始化列表构造函数
- `~vector()`: 析构函数



### 赋值操作

- `operator=(const vector& other)`: 拷贝赋值
- `operator=(vector&& other)`: 移动赋值
- `assign(size_type count, const T& value)`: 赋值count个value



### 元素访问

- `operator[]`: 下标访问
- `at()`: 带边界检查的下标访问
- `front()`: 返回第一个元素的引用
- `back()`: 返回最后一个元素的引用
- `data()`: 返回指向内存的指针



### 迭代器

- `begin()`/`end()`: 返回迭代器
- `cbegin()`/`cend()`: 返回常量迭代器
- `rbegin()`/`rend()`: 返回反向迭代器
- `crbegin()`/`crend()`: 返回常量反向迭代器



### 容量操作

- `empty()`: 检查容器是否为空
- `size()`: 返回容器中的元素数
- `max_size()`: 返回可容纳的最大元素数
- `capacity()`: 返回当前容量
- `reserve()`: 预留存储空间
- `shrink_to_fit()`: 减少容量以适应大小



### 修改器

- `clear()`: 清空容器
- `insert()`: 插入元素
- `emplace()`: 原位构造元素
- `erase()`: 删除元素
- `push_back()`/`emplace_back()`: 在末尾添加元素
- `pop_back()`: 删除末尾元素
- `resize()`: 改变容器大小
- `swap()`: 交换内容



## 非成员函数

- 比较运算符: `==`, `!=`, `<`, `<=`, `>`, `>=`
- `swap(vector& lhs, vector& rhs)`: 交换两个向量的内容



## 使用示例

```cpp
// 构造和插入
mystl::vector<int> vec;
vec.push_back(1);
vec.push_back(2);
vec.insert(vec.begin() + 1, 3);  // {1,3,2}

// 访问元素
int first = vec[0];      // 1
int last = vec.back();   // 2
int middle = vec.at(1);  // 3

// 容量操作
vec.reserve(10);         // 预留10个元素的空间
size_t cap = vec.capacity();  // >= 10
vec.shrink_to_fit();    // 释放多余空间

// 删除元素
vec.pop_back();         // {1,3}
vec.erase(vec.begin()); // {3}

// 算法
mystl::vector<int> vec2{4,1,3,2};
mystl::sort(vec2.begin(), vec2.end());  // {1,2,3,4}
```