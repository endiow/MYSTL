# RB Tree

红黑树的实现。红黑树是一种自平衡的二叉搜索树,它在插入和删除操作时通过变色和旋转来保持树的平衡。



## 模板参数

- `T`: 存储的元素类型
- `Compare`: 比较函数类型,默认为 `mystl::less<T>`
- `Alloc`: 分配器类型,默认为 `mystl::allocator<T>`



## 迭代器

提供双向迭代器,支持:
- 递增(`++`): 中序遍历的下一个节点
- 递减(`--`): 中序遍历的上一个节点
- 解引用(`*`): 访问节点值
- 成员访问(`->`)



## 异常安全保证

- 提供强异常安全保证的操作:
  - `insert_unique`/`insert_equal`
  - `erase`
  - `clear`
- 不抛出异常的操作:
  - 移动构造/移动赋值
  - `swap`
  - 迭代器操作



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

- `rb_tree()`: 默认构造函数
- `rb_tree(const Compare& comp)`: 使用比较函数构造
- `rb_tree(const rb_tree& rhs)`: 拷贝构造函数
- `rb_tree(rb_tree&& rhs)`: 移动构造函数
- `~rb_tree()`: 析构函数



### 赋值操作

- `operator=(const rb_tree& rhs)`: 拷贝赋值
- `operator=(rb_tree&& rhs)`: 移动赋值



### 迭代器相关

- `begin()`/`end()`: 返回迭代器
- `cbegin()`/`cend()`: 返回常量迭代器
- `rbegin()`/`rend()`: 返回反向迭代器
- `crbegin()`/`crend()`: 返回常量反向迭代器



### 容量相关

- `empty()`: 判断是否为空
- `size()`: 返回节点个数
- `max_size()`: 返回可容纳的最大节点数



### 插入操作

- `insert_unique()`: 插入不重复的值
- `insert_equal()`: 插入可重复的值



### 删除操作

- `erase(iterator)`: 删除指定位置的节点
- `erase(const value_type&)`: 删除指定值的节点
- `erase(iterator, iterator)`: 删除指定范围的节点
- `clear()`: 清空树



### 查找操作

- `find()`: 查找指定值的节点
- `count()`: 返回指定值的节点个数
- `lower_bound()`: 返回第一个不小于指定值的位置
- `upper_bound()`: 返回第一个大于指定值的位置
- `equal_range()`: 返回等于指定值的范围



### 其他操作

- `swap()`: 交换两棵树的内容
- `key_comp()`: 返回比较函数



## 非成员函数

- 比较运算符: `==`, `!=`, `<`, `<=`, `>`, `>=`
- `swap(rb_tree& lhs, rb_tree& rhs)`: 交换两棵树的内容



## 红黑树性质

1. 每个节点要么是红色,要么是黑色
2. 根节点是黑色
3. 所有叶节点(NIL)是黑色
4. 如果一个节点是红色,则其子节点必须是黑色
5. 从任一节点到其每个叶子的所有路径都包含相同数目的黑色节点



## 使用示例

```cpp
// 构造和插入
mystl::rb_tree<int> tree;
tree.insert_unique(1);
tree.insert_unique(2);
tree.insert_unique(3);

// 查找
auto it = tree.find(2);
if(it != tree.end()) {
    std::cout << *it << std::endl;  // 输出: 2
}

// 范围查找
auto range = tree.equal_range(2);
for(auto it = range.first; it != range.second; ++it) {
    std::cout << *it << std::endl;
}

// 删除
tree.erase(2);
``` 