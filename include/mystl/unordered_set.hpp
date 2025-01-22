#pragma once
#include "hashtable.hpp"

namespace mystl 
{

    template <class Value,
            class HashFcn = mystl::hash<Value>,
            class EqualKey = mystl::equal_to<Value>,
            class Alloc = mystl::allocator<Value>>
    class unordered_set 
    {
    private:
        using ht = hashtable<Value, Value, HashFcn, mystl::identity<Value>, EqualKey, Alloc>;
        ht rep;  // 底层哈希表

    public:
        using key_type = typename ht::key_type;
        using value_type = typename ht::value_type;
        using hasher = typename ht::hasher;
        using key_equal = typename ht::key_equal;
        using size_type = typename ht::size_type;
        using difference_type = typename ht::difference_type;
        using pointer = typename ht::pointer;
        using const_pointer = typename ht::const_pointer;
        using reference = typename ht::reference;
        using const_reference = typename ht::const_reference;
        using iterator = typename ht::const_iterator;
        using const_iterator = typename ht::const_iterator;

        // 构造函数
        unordered_set() : rep(100, hasher(), key_equal()) {}
        explicit unordered_set(size_type n) : rep(n, hasher(), key_equal()) {}
        unordered_set(size_type n, const hasher& hf) : rep(n, hf, key_equal()) {}
        unordered_set(size_type n, const hasher& hf, const key_equal& eql)
            : rep(n, hf, eql) {}

        // 迭代器相关
        iterator begin() const { return rep.begin(); }
        iterator end() const { return rep.end(); }
        const_iterator cbegin() const { return rep.begin(); }
        const_iterator cend() const { return rep.end(); }

        // 容量相关
        bool empty() const { return rep.empty(); }
        size_type size() const { return rep.size(); }
        size_type bucket_count() const { return rep.bucket_count(); }

        // 修改容器操作
        mystl::pair<iterator, bool> insert(const value_type& obj)
        { return rep.insert_unique(obj); }

        void clear() { rep.clear(); }

        // 查找操作
        iterator find(const key_type& key) const { return rep.find(key); }
        size_type count(const key_type& key) const { return rep.count(key); }
        mystl::pair<iterator, iterator> equal_range(const key_type& key) const
        { return rep.equal_range(key); }
    };

} // namespace mystl 