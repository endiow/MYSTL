#pragma once
#include "hashtable.hpp"
#include "util.hpp"

namespace mystl 
{

    template <class Key, class T,
            class HashFcn = mystl::hash<Key>,
            class EqualKey = mystl::equal_to<Key>,
            class Alloc = mystl::allocator<mystl::pair<const Key, T>>>
    class unordered_map 
    {
    private:
        using ht = hashtable<mystl::pair<const Key, T>,
                            Key, HashFcn,
                            mystl::select1st<mystl::pair<const Key, T>>,
                            EqualKey, Alloc>;
        ht rep;

    public:
        using key_type = typename ht::key_type;
        using data_type = T;
        using mapped_type = T;
        using value_type = typename ht::value_type;
        using hasher = typename ht::hasher;
        using key_equal = typename ht::key_equal;
        using size_type = typename ht::size_type;
        using difference_type = typename ht::difference_type;
        using pointer = typename ht::pointer;
        using const_pointer = typename ht::const_pointer;
        using reference = typename ht::reference;
        using const_reference = typename ht::const_reference;
        using iterator = typename ht::iterator;
        using const_iterator = typename ht::const_iterator;

        // 构造函数
        unordered_map() : rep(100, hasher(), key_equal()) {}
        explicit unordered_map(size_type n) : rep(n, hasher(), key_equal()) {}
        unordered_map(size_type n, const hasher& hf) : rep(n, hf, key_equal()) {}
        unordered_map(size_type n, const hasher& hf, const key_equal& eql)
            : rep(n, hf, eql) {}

        // 迭代器相关
        iterator begin() { return rep.begin(); }
        const_iterator begin() const { return rep.begin(); }
        iterator end() { return rep.end(); }
        const_iterator end() const { return rep.end(); }
        const_iterator cbegin() const { return rep.begin(); }
        const_iterator cend() const { return rep.end(); }

        // 容量相关
        bool empty() const { return rep.empty(); }
        size_type size() const { return rep.size(); }
        size_type bucket_count() const { return rep.bucket_count(); }

        // 修改容器操作
        mystl::pair<iterator, bool> insert(const value_type& obj)
        { return rep.insert_unique(obj); }

        T& operator[](const key_type& key) 
        {
            return rep.insert_unique(value_type(key, T())).first->second;
        }

        void clear() { rep.clear(); }

        // 查找操作
        iterator find(const key_type& key) { return rep.find(key); }
        const_iterator find(const key_type& key) const { return rep.find(key); }
        size_type count(const key_type& key) const { return rep.count(key); }
        mystl::pair<iterator, iterator> equal_range(const key_type& key)
        { return rep.equal_range(key); }
        mystl::pair<const_iterator, const_iterator> equal_range(const key_type& key) const
        { return rep.equal_range(key); }
    };

} // namespace mystl 