# String

字符串容器的实现。



## 模板参数

无模板参数，固定使用 `char` 类型作为字符类型。



## 迭代器

提供随机访问迭代器，支持：
- 递增/递减(`++`,`--`)
- 随机访问(`+`,`-`,`+=`,`-=`,`[]`)
- 解引用(`*`)
- 成员访问(`->`)



## 异常安全保证

- 提供强异常安全保证的操作：
  - `insert`
  - `replace`
  - `resize`
  - `reserve`
- 不抛出异常的操作：
  - 移动构造/移动赋值
  - `swap`
  - `pop_back`
  - 所有的查找操作



## 成员类型

类型 | 定义
---|---
`value_type` | `char`
`allocator_type` | `mystl::allocator<char>`
`size_type` | `size_t`
`difference_type` | `ptrdiff_t`
`reference` | `char&`
`const_reference` | `const char&`
`pointer` | `char*`
`const_pointer` | `const char*`
`iterator` | 随机访问迭代器
`const_iterator` | 常量随机访问迭代器
`reverse_iterator` | 反向迭代器
`const_reverse_iterator` | 常量反向迭代器



## 成员函数

### 构造/析构函数

- `string()`: 默认构造函数
- `string(const char* str)`: C风格字符串构造函数
- `string(const char* str, size_type count)`: 部分C风格字符串构造函数
- `string(size_type count, char ch)`: 填充构造函数
- `string(InputIt first, InputIt last)`: 范围构造函数
- `string(const string& other)`: 拷贝构造函数
- `string(string&& other)`: 移动构造函数
- `~string()`: 析构函数



### 赋值操作

- `operator=(const string& other)`: 拷贝赋值
- `operator=(string&& other)`: 移动赋值
- `operator=(const char* str)`: C风格字符串赋值
- `operator=(char ch)`: 字符赋值
- `assign()`: 赋值操作的多个重载版本



### 元素访问

- `operator[]`: 下标访问
- `at()`: 带边界检查的下标访问
- `front()`: 返回第一个字符的引用
- `back()`: 返回最后一个字符的引用
- `data()/c_str()`: 返回C风格字符串



### 迭代器

- `begin()`/`end()`: 返回迭代器
- `cbegin()`/`cend()`: 返回常量迭代器
- `rbegin()`/`rend()`: 返回反向迭代器
- `crbegin()`/`crend()`: 返回常量反向迭代器



### 容量操作

- `empty()`: 检查字符串是否为空
- `size()/length()`: 返回字符串长度
- `capacity()`: 返回当前容量
- `reserve()`: 预留存储空间
- `shrink_to_fit()`: 减少容量以适应大小



### 修改器

- `clear()`: 清空字符串
- `insert()`: 插入字符/字符串
- `erase()`: 删除字符/子串
- `push_back()/pop_back()`: 在末尾添加/删除字符
- `append()/operator+=`: 追加字符串
- `replace()`: 替换子串
- `swap()`: 交换内容



### 字符串操作

- `substr()`: 获取子串
- `find()`: 查找子串
- `rfind()`: 反向查找子串
- `find_first_of()`: 查找第一个匹配的字符
- `find_last_of()`: 查找最后一个匹配的字符
- `find_first_not_of()`: 查找第一个不匹配的字符
- `find_last_not_of()`: 查找最后一个不匹配的字符
- `compare()`: 字符串比较



## 非成员函数

- 比较运算符: `==`, `!=`, `<`, `<=`, `>`, `>=`
- `operator+`: 字符串连接
- `swap(string& lhs, string& rhs)`: 交换两个字符串的内容
- `hash<string>`: 字符串的哈希特化



## 使用示例

```cpp
// 构造和插入
mystl::string str;
str = "hello";
str.insert(5, " world");  // "hello world"

// 访问元素
char first = str[0];      // 'h'
char last = str.back();   // 'd'
char middle = str.at(6);  // 'w'

// 容量操作
str.reserve(20);          // 预留20个字符的空间
size_t cap = str.capacity();  // >= 20
str.shrink_to_fit();     // 释放多余空间

// 字符串操作
mystl::string sub = str.substr(0, 5);  // "hello"
size_t pos = str.find("world");        // 6
str.replace(0, 5, "hi");              // "hi world"

// 字符串连接
mystl::string s1("hello"), s2(" world");
mystl::string s3 = s1 + s2;           // "hello world"
``` 