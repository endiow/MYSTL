# MySTL

MySTL 是一个基于 C++17 的 STL 实现，旨在深入理解 STL 的设计思想和实现细节。该项目包含了 STL 的核心组件，并提供了完整的单元测试。
用于找工作的小玩具，代码很烂，请勿参考


## 特性

- 基于 C++17 标准
- 提供异常安全保证
- 使用内存池优化内存分配
- 完整的单元测试覆盖
- 详细的文档说明



## 主要组件



### 类型萃取 (Type Traits)

- 类型特性检测
- 类型转换
- 类型关系判断



### 迭代器系统

- 迭代器类别标签
- 迭代器特性萃取
- 迭代器适配器
- 迭代器辅助函数



### 内存管理

- 内存分配器
- 内存池实现
- 未初始化内存操作
- 对象构造/析构



### 基础算法

- 复制和移动
- 填充和初始化
- 比较和查找
- 排序和堆操作



### 容器

- vector (动态数组)
- 提供强异常安全保证
- 高效的内存管理



## 构建要求

- C++17 兼容的编译器
- CMake 3.14 或更高版本
- Google Test (用于单元测试)



## 构建步骤

```bash
# 克隆仓库
git clone https://github.com/endiow/MYSTL.git
cd mystl

# 创建构建目录
mkdir build
cd build

# 配置和构建
cmake ..
cmake --build .

# 运行测试
ctest
```



## 项目结构

```
mystl/
├── include/mystl/         # 头文件
│   ├── algorithm.hpp      # 算法
│   ├── allocator.hpp      # 分配器
│   ├── vector.hpp         # 容器
│   └── ...
├── test/                  # 测试文件
│   ├── algorithm_test.cpp
│   ├── allocator_test.cpp
│   └── ...
├── md/                    # 文档
│   ├── algorithm.md
│   ├── allocator.md
│   └── ...
└── CMakeLists.txt        # CMake 构建文件
```



## 文档

详细的实现说明和设计文档位于 `md/` 目录下：
- [迭代器系统](md/iterator.md)
- [内存分配器](md/allocator.md)
- [算法实现](md/algorithm.md)
- [容器实现](md/vector.md)



## 示例

```cpp
#include <mystl/vector.hpp>
#include <mystl/algorithm.hpp>

int main() {
    // 创建 vector
    mystl::vector<int> vec{1, 2, 3, 4, 5};
    
    // 使用算法
    auto it = mystl::find(vec.begin(), vec.end(), 3);
    
    // 使用迭代器
    for (const auto& elem : vec) {
        // 处理元素...
    }
    
    return 0;
}
```



## 特别说明

1. 异常安全
- 所有容器操作提供基本异常安全保证
- 大部分操作提供强异常安全保证
- 详细的异常安全说明在各个组件的文档中

2. 内存管理
- 使用内存池优化小对象分配
- 自动处理内存对齐
- 提供内存泄漏检测

3. 性能优化
- 特化处理平凡类型
- 优化内存分配策略
- 减少不必要的拷贝


## 许可

本项目采用 MIT 许可证。详见 [LICENSE](LICENSE) 文件。 