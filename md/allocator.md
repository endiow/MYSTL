# 分配器



## 内存池 MemoryPool

### 主要组件

- MemoryBlock：内存块结构，用于管理空闲内存链表
  - next：指向下一个内存块

- MemoryChunk：管理大块内存的结构
  - memory：实际内存块的起始地址
  - next：指向下一个大块内存



### 关键常量

- NUM_FREE_LISTS = 16：空闲列表数量

- ALIGN = alignof(max_align_t)：内存对齐要求

  - max_align_t：编译器提供的类型，表示平台上要求最严格的对齐类型

  - 通常是 long double 或 max_align_t 的对齐要求

  - 在大多数平台上是 8 或 16 字节

  - 保证所有标量类型都能正确对齐

- BLOCK_SIZE = 4096：每次分配的大块内存大小

- MAX_BYTES = 256：小对象阈值

  

### 内存管理策略

1. **小对象**（<= MAX_BYTES）：
   - 使用空闲列表管理
   - 按大小分类存储
   - 减少内存碎片
   - 提高分配效率

2. **大对象**（> MAX_BYTES）：
   - 直接使用 malloc 分配
   
   - 避免内存池管理大块内存的开销
   
     

### 关键函数

- align_up：计算对齐后的大小

  ```c++
  static size_t align_up(size_t n) noexcept 
  {
    return (n + ALIGN - 1) & ~(ALIGN - 1);  // 向上取整到ALIGN的倍数
  }
  ```

  ``原理说明：`

  1. `n + (ALIGN-1)：确保结果大于等于n，且足够大以便向下取整`
  2. `~(ALIGN-1)：创建掩码，末尾有log2(ALIGN)个0`
  3. `& 操作：将结果向下取整到ALIGN的倍数`

  `例如，当ALIGN=8时：`

  - `n=5：(5+7) & ~7 = 12 & 0xFFF8 = 8`

  - `n=9：(9+7) & ~7 = 16 & 0xFFF8 = 16`

    

- free_list_index：计算合适的空闲列表索引

- allocate_chunk：分配新的内存块

  ```cpp
  void allocate_chunk() 
  {
      char* new_memory = static_cast<char*>(std::malloc(BLOCK_SIZE + ALIGN));
      
      // 使用 uintptr_t 进行指针到整数的转换
      char* aligned_memory = reinterpret_cast<char*>(
          (reinterpret_cast<std::uintptr_t>(new_memory) + ALIGN - 1) & ~(ALIGN - 1));
      // ...
  }
  ```
  
  `std::uintptr_t 说明：`
  1. `保证大小足够存储指针值的无符号整数类型`
  2. `用于指针和整数之间的转换`
  3. `保证不会丢失指针信息`
  4. `常用于指针对齐计算`
  
  `使用场景：`
  - `需要将指针转换为整数进行位运算`
  
  - `需要保存指针的数值表示`
  
  - `进行指针对齐计算`
  
    

## STL分配器 allocator

### 特点

- 封装 MemoryPool

- 符合 STL 分配器要求

- 提供类型定义和内存管理接口

- 支持不同类型间的转换

  

### 主要接口

```cpp
template<class T>
class allocator 
{
    // 类型定义
    using value_type = T;
    using pointer = T*;
    using size_type = size_t;

    // 内存管理
    pointer allocate(size_type n);
    void deallocate(pointer p, size_type n);

    // 对象构造/析构
    template<typename... Args>
    void construct(pointer p, Args&&... args);
    void destroy(pointer p);
};

```

### 使用示例

```cpp
mystl::allocator<int> alloc;

// 分配内存
int* p = alloc.allocate(1);

// 构造对象
alloc.construct(p, 42);

// 销毁对象
alloc.destroy(p);

// 释放内存
alloc.deallocate(p, 1);

```



## 依赖说明

- cstdlib：提供 malloc、free 等基础内存管理函数

- cstdint：提供 uintptr_t 类型

- new：提供 bad_alloc 异常类

- expectdef.hpp：提供基础类型定义

  

### 类型转换说明

在 allocate_chunk 中使用了 reinterpret_cast：
```cpp
char* aligned_memory = reinterpret_cast<char*>(
    (reinterpret_cast<std::uintptr_t>(new_memory) + ALIGN - 1) & ~(ALIGN - 1));
```

`reinterpret_cast 特点：`
1. `最危险的类型转换，直接重新解释内存的二进制表示`
2. `可以在任意指针类型之间转换`
3. `可以在指针和整数类型之间转换`
4. `结果取决于平台的实现`

`使用场景：`
- `指针和整数之间的转换`
- `不同类型指针之间的转换`
- `底层内存操作`
- `位操作和对齐计算`

`为什么在这里使用：`
1. `需要将指针转换为整数进行对齐计算`

2. `需要将计算结果转回指针类型`

3. `处理内存对齐时必须进行的底层操作`

   

### 对象构造说明

在 allocator 中使用了 placement new：
```cpp
template<typename U, typename... Args>
void construct(U* p, Args&&... args) 
{
    ::new(static_cast<void*>(p)) U(std::forward<Args>(args)...);
}
```

`placement new 说明：`
1. `::new 是全局作用域的 new 操作符`
2. `(static_cast<void*>(p)) 指定要在哪个地址构造对象`
3. `U(std::forward<Args>(args)...) 调用构造函数`

`使用场景：`
- `在已分配的内存上构造对象`
- `避免内存的重复分配`
- `实现内存和对象构造的分离`
- `提高性能和灵活性`

`工作流程：`
1. `不分配新内存`
2. `在指定地址构造对象`
3. `完美转发构造参数`
4. `返回对象指针`

`为什么使用 static_cast<void*>：`
1. `确保指针类型安全`
2. `符合 placement new 的参数要求`
3. `支持不同类型的指针转换`
