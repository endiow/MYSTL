# 基本算法



## 比较算法

### `min`
返回两个值中的较小值
- `min(const T& a, const T& b)`
- `min(const T& a, const T& b, Compare comp)`

### `max`
返回两个值中的较大值
- `max(const T& a, const T& b)`
- `max(const T& a, const T& b, Compare comp)`

### `equal`
判断两个区间是否相等
- `equal(InputIt1 first1, InputIt1 last1, InputIt2 first2)`
- `equal(InputIt1 first1, InputIt1 last1, InputIt2 first2, BinaryPredicate pred)`

### `mismatch`
返回两个区间第一个不匹配的位置
- `mismatch(InputIt1 first1, InputIt1 last1, InputIt2 first2)`
- `mismatch(InputIt1 first1, InputIt1 last1, InputIt2 first2, BinaryPredicate pred)`

### `lexicographical_compare`
按字典序比较两个区间
- `lexicographical_compare(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2)`
- `lexicographical_compare(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, Compare comp)`



## 复制和移动算法

### `copy`
将区间 [first, last) 的元素复制到以 result 开始的位置
- `copy(InputIt first, InputIt last, OutputIt result)`

### `copy_backward`
从后往前复制元素
- `copy_backward(BidirIt1 first, BidirIt1 last, BidirIt2 result)`

### `copy_if`
仅复制满足条件的元素
- `copy_if(InputIt first, InputIt last, OutputIt result, UnaryPredicate pred)`

### `copy_n`
复制指定数量的元素
- `copy_n(InputIt first, Size count, OutputIt result)`

### `move`
将区间元素移动到新位置
- `move(InputIt first, InputIt last, OutputIt result)`

### `move_backward`
从后往前移动元素
- `move_backward(BidirIt1 first, BidirIt1 last, BidirIt2 result)`



## 填充算法

### `fill`
将区间内所有元素设为指定值
- `fill(ForwardIt first, ForwardIt last, const T& value)`

### `fill_n`
将从指定位置开始的 n 个元素设为指定值
- `fill_n(OutputIt first, Size count, const T& value)`



## 其他算法

### `iter_swap`
交换两个迭代器所指向的元素
- `iter_swap(ForwardIt1 a, ForwardIt2 b)`