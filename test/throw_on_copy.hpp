#pragma once
#include <stdexcept>

// 用于测试异常安全的类
class ThrowOnCopy 
{
public:
    static inline bool should_throw = false;
    static inline int copy_count = 0;
    int value;
    
    ThrowOnCopy(int v = 0) noexcept : value(v) {}
    
    ThrowOnCopy(const ThrowOnCopy& other) 
    {
        if (should_throw) 
            throw std::runtime_error("Copy failed");
        value = other.value;
        ++copy_count;
    }

    ThrowOnCopy& operator=(const ThrowOnCopy& other) 
    {
        if (should_throw)
            throw std::runtime_error("Copy failed");
        value = other.value;
        ++copy_count;
        return *this;
    }

    ThrowOnCopy(ThrowOnCopy&& other) noexcept
        : value(other.value) {}

    ThrowOnCopy& operator=(ThrowOnCopy&& other) noexcept 
    {
        value = other.value;
        return *this;
    }
    
    bool operator==(const ThrowOnCopy& other) const 
    {
        return value == other.value;
    }

    bool operator<(const ThrowOnCopy& other) const 
    {
        return value < other.value;
    }
    
    static void reset() 
    {
        should_throw = false;
        copy_count = 0;
    }
};

namespace mystl 
{
    template <>
    struct hash<ThrowOnCopy>
    {
        size_t operator()(const ThrowOnCopy& x) const 
        { 
            return hash<int>()(x.value); 
        }
    };
} 