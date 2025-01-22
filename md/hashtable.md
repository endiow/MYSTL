# hashtable 说明文档

## 简介

hashtable（哈希表）是一种基于哈希函数实现的关联容器，它可以在常数时间内完成元素的插入、删除和查找操作。

## 模板参数

- `Value`: 元素类型
- `Key`: 键类型
- `HashFcn`: 哈希函数，用于计算键的哈希值
- `ExtractKey`: 从元素中提取键的函数对象
- `EqualKey`: 判断键是否相等的函数对象
- `Alloc`: 内存分配器，默认使用 mystl::allocator

## 接口说明

### 构造函数

```cpp
explicit hashtable(size_type n = 100,
                  const hasher& hf = hasher(),
                  const key_equal& eql = key_equal());
```

- 创建一个空的哈希表
- n: 预计元素数量，用于确定初始桶数
- hf: 哈希函数对象
- eql: 键比较函数对象

### 迭代器相关

- `begin()`: 返回指向第一个元素的迭代器
- `end()`: 返回指向末尾的迭代器
- `cbegin()`: 返回 const 迭代器
- `cend()`: 返回 const 迭代器

### 容量相关

- `size()`: 返回元素个数
- `empty()`: 判断是否为空
- `bucket_count()`: 返回桶的个数

### 插入操作

```cpp
pair<iterator, bool> insert_unique(const value_type& obj);
iterator insert_equal(const value_type& obj);
```

- `insert_unique`: 插入不重复元素，返回是否插入成功
- `insert_equal`: 允许插入重复元素，返回新元素位置

### 删除操作

- `clear()`: 清空哈希表

### 查找操作

```cpp
iterator find(const key_type& key);
const_iterator find(const key_type& key) const;
size_type count(const key_type& key) const;
pair<iterator, iterator> equal_range(const key_type& key);
```

- `find`: 查找指定键的元素
- `count`: 统计指定键的元素个数
- `equal_range`: 返回指定键的元素范围

## 实现说明

### 内部结构

1. 节点结构
```cpp
struct hashtable_node 
{
    hashtable_node* next;   // 指向下一个节点
    T value;               // 节点值
};
```

2. 使用开链法处理冲突
- 每个桶维护一个单向链表
- 新元素插入到链表头部

### 哈希函数

- 使用模板参数 HashFcn 提供的哈希函数
- 通过取模运算将哈希值映射到桶索引

### 动态扩容

- 当元素数量增长时自动扩容
- 使用质数作为桶数量，提高散列效果
- 重新哈希保证元素分布均匀

### 异常安全

- 提供基本异常安全保证
- 在扩容和插入操作中处理异常

## 性能分析

- 平均情况下的时间复杂度：
  - 插入：O(1)
  - 查找：O(1)
  - 删除：O(1)
- 最坏情况（所有元素在同一个桶）：
  - 所有操作：O(n)

## 使用示例

```cpp
mystl::hashtable<int, int, mystl::hash<int>, 
                mystl::identity<int>, mystl::equal_to<int>> ht;

// 插入元素
ht.insert_unique(1);
ht.insert_unique(2);

// 查找元素
auto it = ht.find(1);
if (it != ht.end()) {
    // 找到元素
}

// 统计元素
size_t count = ht.count(1);
``` 