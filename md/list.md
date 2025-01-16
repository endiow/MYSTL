# List

双向链表容器的实现。



## 模板参数

- `T`: 元素类型
- `Alloc`: 分配器类型，默认为 `mystl::allocator<T>`



## 迭代器

提供双向迭代器，支持：
- 递增(`++`)
- 递减(`--`) 
- 解引用(`*`)
- 成员访问(`->`)



## 异常安全保证

- 提供强异常安全保证的操作：
  - `emplace`/`emplace_back`/`emplace_front`
  - `resize`
  - `insert`
- 不抛出异常的操作：
  - 移动构造/移动赋值
  - `swap`
  - `erase`/`pop_back`/`pop_front`



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
`iterator` | 双向迭代器
`const_iterator` | 常量双向迭代器
`reverse_iterator` | 反向迭代器
`const_reverse_iterator` | 常量反向迭代器



## 成员函数

### 构造/析构函数

- `list()`: 默认构造函数
- `list(size_type n)`: 构造包含n个默认元素的链表
- `list(size_type n, const T& value)`: 构造包含n个value的链表
- `list(InputIt first, InputIt last)`: 范围构造函数
- `list(const list& other)`: 拷贝构造函数
- `list(list&& other)`: 移动构造函数
- `list(std::initializer_list<T> ilist)`: 初始化列表构造函数
- `~list()`: 析构函数



### 赋值操作

- `operator=(const list& other)`: 拷贝赋值
- `operator=(list&& other)`: 移动赋值
- `assign(size_type count, const T& value)`: 赋值count个value



### 元素访问

- `front()`: 返回第一个元素的引用
- `back()`: 返回最后一个元素的引用



### 迭代器

- `begin()`/`end()`: 返回迭代器
- `cbegin()`/`cend()`: 返回常量迭代器
- `rbegin()`/`rend()`: 返回反向迭代器
- `crbegin()`/`crend()`: 返回常量反向迭代器



### 容量操作

- `empty()`: 检查容器是否为空
- `size()`: 返回容器中的元素数
- `max_size()`: 返回可容纳的最大元素数



### 修改器

- `clear()`: 清空容器
- `insert()`: 插入元素
- `emplace()`: 原位构造元素
- `erase()`: 删除元素
- `push_back()`/`emplace_back()`: 在末尾添加元素
- `pop_back()`: 删除末尾元素
- `push_front()`/`emplace_front()`: 在头部添加元素
- `pop_front()`: 删除头部元素
- `resize()`: 改变容器大小
- `swap()`: 交换内容



### 链表操作

- `splice()`: 拼接另一个链表
- `remove()`/`remove_if()`: 移除满足条件的元素
- `unique()`: 删除连续重复元素
- `merge()`: 合并两个有序链表
- `sort()`: 排序
- `reverse()`: 反转链表



## 非成员函数

- 比较运算符: `==`, `!=`, `<`, `<=`, `>`, `>=`
- `swap(list& lhs, list& rhs)`: 交换两个链表的内容



## 使用示例

```cpp
// 构造和插入
mystl::list<int> lst;
lst.push_back(1);
lst.push_front(0);
lst.insert(++lst.begin(), 2);  // {0,2,1}

// 访问元素
int front = lst.front();  // 0
int back = lst.back();   // 1

// 删除元素
lst.pop_back();    // {0,2}
lst.pop_front();   // {2}

// 算法
mystl::list<int> lst2{4,1,3,2};
lst2.sort();         // {1,2,3,4}
lst2.reverse();      // {4,3,2,1}
lst2.remove(3);      // {4,2,1}

// 拼接
mystl::list<int> lst3{5,6};
lst2.splice(lst2.begin(), lst3);  // lst2: {5,6,4,2,1}, lst3: {}
```
