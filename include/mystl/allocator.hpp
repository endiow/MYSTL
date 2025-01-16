#pragma once

#include <cstdlib>  // for malloc, free
#include <cstdint>  // for uintptr_t
#include <new>      // for bad_alloc
#include "expectdef.hpp"
#include "util.hpp"
#include "algorithm.hpp"
#include "construct.hpp"

namespace mystl 
{
    // 内存块结构：用于管理空闲内存链表
    struct MemoryBlock 
    {
        MemoryBlock* next;  // 指向下一个内存块，形成链表
    };


    //------------------------------------------------------------------------------
    // 内存池实现：为特定类型T提供内存分配和管理
    //------------------------------------------------------------------------------
    template<class T, size_t BlockSize = 4096>
    class MemoryPool 
    {
    private:
        // 管理大块内存的结构：跟踪所有分配的内存块
        struct MemoryChunk 
        {
            char* memory;       // 实际内存块的起始地址
            MemoryChunk* next;  // 链表结构，指向下一个大块内存
        };

        // 常量定义
        static constexpr size_t NUM_FREE_LISTS = 16;  // 空闲列表数量，管理不同大小的内存块
        static constexpr size_t ALIGN = alignof(max_align_t);  // 内存对齐要求
        static constexpr size_t BLOCK_SIZE = BlockSize;  // 每次分配的大块内存大小
        static constexpr size_t MAX_BYTES = 256;  // 小对象的阈值，超过此大小直接使用malloc

        // 成员变量
        MemoryBlock* free_lists[NUM_FREE_LISTS]{};  // 空闲列表数组，每个列表管理特定大小的块
        MemoryChunk* chunks{nullptr};  // 已分配的大块内存链表
        char* memory_chunk{nullptr};   // 当前正在使用的内存块指针
        size_t chunk_size{0};          // 当前内存块剩余大小

        // 工具函数：计算对齐后的大小
        static size_t align_up(size_t n) noexcept 
        {
            return (n + ALIGN - 1) & ~(ALIGN - 1);  // 向上取整到ALIGN的倍数
        }

        // 工具函数：计算合适的空闲列表索引
        static size_t free_list_index(size_t bytes) noexcept 
        {
            return (bytes + sizeof(MemoryBlock) - 1) / sizeof(MemoryBlock) - 1;
        }

        // 分配新的内存块
        void allocate_chunk() 
        {
            // 分配额外的空间用于对齐
            char* new_memory = static_cast<char*>(std::malloc(BLOCK_SIZE + ALIGN));
            if (!new_memory) throw std::bad_alloc();

            // 计算对齐后的内存地址
            char* aligned_memory = reinterpret_cast<char*>((reinterpret_cast<std::uintptr_t>(new_memory) + ALIGN - 1) & ~(ALIGN - 1));

            // 保存并更新内存块信息
            chunks = new MemoryChunk{new_memory, chunks};
            memory_chunk = aligned_memory;
            chunk_size = BLOCK_SIZE;
        }

    public:
        // 使用默认构造，成员已在声明时初始化
        MemoryPool() = default;
        
        // 析构函数：释放所有分配的内存
        ~MemoryPool() 
        {
            while (chunks) 
            {
                MemoryChunk* next = chunks->next;
                std::free(chunks->memory);
                delete chunks;
                chunks = next;
            }
        }

        // 禁止拷贝操作，确保内存管理的唯一性
        MemoryPool(const MemoryPool&) = delete;
        MemoryPool& operator=(const MemoryPool&) = delete;

        // 分配内存
        T* allocate(size_t n = 1) 
        {
            if (n == 0) return nullptr;
            
            size_t bytes = align_up(n * sizeof(T));

            // 小对象从空闲列表分配
            if (bytes <= MAX_BYTES) 
            {
                size_t index = free_list_index(bytes);
                // 如果空闲列表有可用块，直接使用
                if (free_lists[index]) 
                {
                    T* result = reinterpret_cast<T*>(free_lists[index]);
                    free_lists[index] = free_lists[index]->next;
                    return result;
                }

                // 检查当前块是否有足够空间
                if (chunk_size < bytes || !memory_chunk) 
                {
                    // 如果请求的大小超过块大小，直接使用 malloc
                    if (bytes > BLOCK_SIZE) 
                    {
                        void* p = std::malloc(bytes);
                        if (!p) throw std::bad_alloc();
                        return static_cast<T*>(p);
                    }
                    
                    // 分配新的内存块
                    allocate_chunk();
                }

                T* result = reinterpret_cast<T*>(memory_chunk);
                memory_chunk += bytes;
                chunk_size -= bytes;
                return result;
            }

            // 大对象直接用malloc
            void* p = std::malloc(bytes);
            if (!p) throw std::bad_alloc();
            return static_cast<T*>(p);
        }

        // 释放内存
        void deallocate(T* p, size_t n = 1) noexcept 
        {
            if (!p) return;
            
            size_t bytes = align_up(n * sizeof(T));
            if (bytes <= MAX_BYTES) 
            {
                // 小对象放回对应的空闲列表
                size_t index = free_list_index(bytes);
                auto block = reinterpret_cast<MemoryBlock*>(p);
                block->next = free_lists[index];
                free_lists[index] = block;
            }
            else 
            {
                // 大对象直接释放
                std::free(p);
            }
        }
    };



    //------------------------------------------------------------------------------    
    // STL分配器封装：符合STL分配器要求的接口
    //------------------------------------------------------------------------------
    template<class T>
    class allocator 
    {
    private:
        static MemoryPool<T> pool;  // 每个类型使用独立的内存池

    public:
        // STL要求的类型定义
        using value_type = T;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;
        using size_type = size_t;
        using difference_type = ptrdiff_t;

        // 允许分配器在不同类型间转换
        template<typename U>
        struct rebind { using other = allocator<U>; };

        // 构造函数
        constexpr allocator() noexcept = default;
        constexpr allocator(const allocator&) noexcept = default;
        constexpr allocator& operator=(const allocator&) noexcept = default;
        ~allocator() = default;

        // 允许从其他类型的分配器构造
        template<typename U>
        constexpr allocator(const allocator<U>&) noexcept {}

        // 比较操作
        template <class U>
        bool operator==(const allocator<U>&) const noexcept 
        { 
            return true; 
        }

        template <class U>
        bool operator!=(const allocator<U>&) const noexcept 
        { 
            return false; 
        }

        // 内存分配
        pointer allocate(size_type n) 
        {
            if (n > max_size()) 
                throw length_error("allocator<T>::allocate() - Integer overflow.");
            return pool.allocate(n);
        }

        // 内存释放
        void deallocate(pointer p, size_type n) noexcept 
        {
            pool.deallocate(p, n);
        }

        // 返回最大可分配大小
        size_type max_size() const noexcept 
        {
            return size_type(-1) / sizeof(T);
        }
    };

    // 静态内存池实例
    template<class T>
    MemoryPool<T> allocator<T>::pool;

    // 计算最大可分配大小
    template <class Alloc>
    typename Alloc::size_type max_size(const Alloc& alloc) noexcept 
    {
        return mystl::min(
            typename Alloc::size_type(-1) / sizeof(typename Alloc::value_type),  // 防止内存大小溢出
            alloc.max_size()  // 分配器的限制
        );
    }

} // namespace mystl 