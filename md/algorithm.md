# Algorithm

STL算法库的实现，提供一系列常用算法。



## 查找算法

### 基础查找
- `find(first, last, value)`: 查找等于value的第一个元素
- `find_if(first, last, pred)`: 查找满足谓词的第一个元素
- `find_if_not(first, last, pred)`: 查找不满足谓词的第一个元素
- `find_first_of(first1, last1, first2, last2)`: 查找第一个出现在第二个序列中的元素
- `adjacent_find(first, last)`: 查找相邻的重复元素

### 计数

- `count(first, last, value)`: 统计等于value的元素个数
- `count_if(first, last, pred)`: 统计满足谓词的元素个数

### 搜索

- `search(first1, last1, first2, last2)`: 查找子序列
- `search_n(first, last, count, value)`: 查找连续count个值为value的子序列



## 修改序列算法

### 替换
- `replace(first, last, old_value, new_value)`: 替换等于old_value的元素
- `replace_if(first, last, pred, new_value)`: 替换满足谓词的元素
- `replace_copy(first, last, d_first, old_value, new_value)`: 替换并复制
- `replace_copy_if(first, last, d_first, pred, new_value)`: 替换并复制满足谓词的元素

### 删除

- `remove(first, last, value)`: 移除等于value的元素
- `remove_if(first, last, pred)`: 移除满足谓词的元素
- `remove_copy(first, last, d_first, value)`: 移除并复制
- `remove_copy_if(first, last, d_first, pred)`: 移除并复制满足谓词的元素

### 去重和变换

- `unique(first, last)`: 移除连续重复元素
- `unique_copy(first, last, d_first)`: 移除重复元素并复制
- `reverse(first, last)`: 将序列反转
- `reverse_copy(first, last, d_first)`: 反转并复制
- `rotate(first, middle, last)`: 旋转序列
- `rotate_copy(first, middle, last, d_first)`: 旋转并复制
- `random_shuffle(first, last)`: 随机重排序列



## 排序算法

### 稳定排序
- `insertion_sort(first, last)`: 插入排序
- `merge_sort(first, last)`: 归并排序
- `bubble_sort(first, last)`: 冒泡排序



### 不稳定排序

- `quick_sort(first, last)`: 快速排序
- `heap_sort(first, last)`: 堆排序
- `selection_sort(first, last)`: 选择排序
- `shell_sort(first, last)`: 希尔排序
- `introsort(first, last)`: 内省排序（快排、堆排和插入排序的混合）



### 通用排序

- `sort(first, last)`: 排序，内部使用内省排序实现



### 排序检查

- `is_sorted(first, last)`: 检查序列是否已排序
- `is_sorted_until(first, last)`: 找到最长的已排序前缀



所有排序算法都提供使用自定义比较器的重载版本：`algorithm(first, last, comp)`



## 二分查找

- `lower_bound(first, last, value)`: 查找第一个不小于value的位置
- `upper_bound(first, last, value)`: 查找第一个大于value的位置
- `binary_search(first, last, value)`: 二分查找value是否存在
- `equal_range(first, last, value)`: 查找value的范围



## 划分操作

- `partition(first, last, pred)`: 按谓词划分序列
- `stable_partition(first, last, pred)`: 稳定划分
- `partition_copy(first, last, d_true, d_false, pred)`: 划分并复制



## 排列组合

- `next_permutation(first, last)`: 获取下一个排列
- `prev_permutation(first, last)`: 获取上一个排列



## 集合操作

- `merge(first1, last1, first2, last2, d_first)`: 合并有序序列
- `inplace_merge(first, middle, last)`: 原地合并有序序列
- `set_union(first1, last1, first2, last2, d_first)`: 求并集
- `set_intersection(first1, last1, first2, last2, d_first)`: 求交集
- `set_difference(first1, last1, first2, last2, d_first)`: 求差集
- `set_symmetric_difference(first1, last1, first2, last2, d_first)`: 求对称差



## 数值算法

- `accumulate(first, last, init)`: 序列求和
- `inner_product(first1, last1, first2, init)`: 计算内积
- `partial_sum(first, last, d_first)`: 计算部分和
- `adjacent_difference(first, last, d_first)`: 计算相邻差
