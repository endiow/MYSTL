#pragma once
#include "allocator.hpp"
#include "vector.hpp"
#include "util.hpp"
#include "functional.hpp"

namespace mystl 
{

    // 哈希表节点
    template <class T>
    struct hashtable_node 
    {
        hashtable_node* next;   // 指向下一个节点
        T value;               // 节点值
        
        hashtable_node() = default;
        hashtable_node(const T& v) : next(nullptr), value(v) {}
    };

    // 前向声明哈希表
    template <class Value, class Key, class HashFcn,
              class ExtractKey, class EqualKey, class Alloc>
    class hashtable;

    // 迭代器基类
    template <class Value, class Key, class HashFcn,
              class ExtractKey, class EqualKey, class Alloc>
    struct hashtable_iterator_base
    {
        using hashtable = mystl::hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
        using iterator_category = forward_iterator_tag;
        using value_type = Value;
        using difference_type = ptrdiff_t;
        using size_type = size_t;
        using node = hashtable_node<Value>;

        node* cur;
        hashtable* ht;

        hashtable_iterator_base() : cur(nullptr), ht(nullptr) {}
        hashtable_iterator_base(node* n, hashtable* tab) : cur(n), ht(tab) {}
    };

    // 哈希表迭代器
    template <class Value, class Key, class HashFcn,
              class ExtractKey, class EqualKey, class Alloc>
    struct hashtable_iterator 
    {
        using node = hashtable_node<Value>;
        using self = hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
        using hashtable = mystl::hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
        using iterator_category = forward_iterator_tag;
        using value_type = Value;
        using pointer = Value*;
        using const_pointer = const Value*;
        using reference = Value&;
        using const_reference = const Value&;
        using size_type = size_t;
        using difference_type = ptrdiff_t;

        node* cur;        // 当前节点
        hashtable* ht;    // 指向哈希表

        hashtable_iterator() noexcept : cur(nullptr), ht(nullptr) {}
        hashtable_iterator(node* n, hashtable* tab) noexcept : cur(n), ht(tab) {}

        reference operator*() const { return cur->value; }
        pointer operator->() const { return &(operator*()); }

        self& operator++();
        self operator++(int);

        bool operator==(const self& rhs) const { return cur == rhs.cur; }
        bool operator!=(const self& rhs) const { return cur != rhs.cur; }
    };

    // const 迭代器
    template <class Value, class Key, class HashFcn,
              class ExtractKey, class EqualKey, class Alloc>
    struct hashtable_const_iterator : public hashtable_iterator_base<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>
    {
        using base = hashtable_iterator_base<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
        using hashtable = typename base::hashtable;
        using node = typename base::node;
        using reference = const Value&;
        using pointer = const Value*;
        using self = hashtable_const_iterator;

        using base::cur;
        using base::ht;

        hashtable_const_iterator() {}
        hashtable_const_iterator(const node* n, const hashtable* tab) 
            : base(const_cast<node*>(n), const_cast<hashtable*>(tab)) {}
        hashtable_const_iterator(const hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>& it)
            : base(it.cur, it.ht) {}

        reference operator*() const { return cur->value; }
        pointer operator->() const { return &(operator*()); }

        self& operator++();
        self operator++(int);

        bool operator==(const self& rhs) const { return cur == rhs.cur; }
        bool operator!=(const self& rhs) const { return cur != rhs.cur; }

        bool operator==(const hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>& rhs) const 
        { 
            return cur == rhs.cur; 
        }
        bool operator!=(const hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>& rhs) const 
        { 
            return cur != rhs.cur; 
        }
    };

    // 哈希表
    template <class Value, class Key, class HashFcn,
              class ExtractKey, class EqualKey,
              class Alloc = mystl::allocator<Value>>
    class hashtable 
    {
        friend struct hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
        friend struct hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;

    public:
        using key_type = Key;
        using value_type = Value;
        using hasher = HashFcn;
        using key_equal = EqualKey;
        
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;

        using node_type = hashtable_node<Value>;
        using node_allocator = typename Alloc::template rebind<node_type>::other;

        using iterator = hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
        using const_iterator = hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;

    public:
        // 构造函数
        explicit hashtable(size_type n = 100,
                          const hasher& hf = hasher(),
                          const key_equal& eql = key_equal())
            : hash_(hf), equals_(eql), get_key_(ExtractKey()), num_elements_(0)
        {
            initialize_buckets(n);
        }
        
        ~hashtable() { clear(); }

        // 迭代器相关
        iterator begin() 
        {
            for (size_type n = 0; n < buckets_.size(); ++n)
            {
                if (buckets_[n])
                    return iterator(buckets_[n], this);
            }
            return end();
        }

        const_iterator begin() const 
        {
            for (size_type n = 0; n < buckets_.size(); ++n)
            {
                if (buckets_[n])
                    return const_iterator(buckets_[n], this);
            }
            return end();
        }

        const_iterator cbegin() const { return begin(); }
        
        iterator end() { return iterator(nullptr, this); }
        const_iterator end() const { return const_iterator(nullptr, this); }
        const_iterator cend() const { return end(); }

        // 容量相关
        size_type size() const { return num_elements_; }
        bool empty() const { return size() == 0; }
        size_type bucket_count() const { return buckets_.size(); }

        // 插入操作
        mystl::pair<iterator, bool> insert_unique(const value_type& obj)
        {
            resize(num_elements_ + 1);
            return insert_unique_noresize(obj);
        }

        iterator insert_equal(const value_type& obj)
        {
            resize(num_elements_ + 1);
            return insert_equal_noresize(obj);
        }

        // 清空操作
        void clear()
        {
            for (size_type i = 0; i < buckets_.size(); ++i)
            {
                node_type* cur = buckets_[i];
                while (cur != nullptr)
                {
                    node_type* next = cur->next;
                    delete_node(cur);
                    cur = next;
                }
                buckets_[i] = nullptr;
            }
            num_elements_ = 0;
        }

        // 查找操作
        iterator find(const key_type& key)
        {
            size_type n = bucket_num(key);
            node_type* first = buckets_[n];
            for (; first && !equals_(get_key_(first->value), key); first = first->next)
            {}
            return iterator(first, this);
        }

        const_iterator find(const key_type& key) const
        {
            size_type n = bucket_num(key);
            const node_type* first = buckets_[n];
            for (; first && !equals_(get_key_(first->value), key); first = first->next)
            {}
            return const_iterator(first, this);
        }

        // 计数操作
        size_type count(const key_type& key) const
        {
            size_type n = bucket_num(key);
            size_type result = 0;
            for (const node_type* cur = buckets_[n]; cur; cur = cur->next)
            {
                if (equals_(get_key_(cur->value), key))
                    ++result;
            }
            return result;
        }

        // 范围查找
        mystl::pair<iterator, iterator> equal_range(const key_type& key)
        {
            size_type n = bucket_num(key);
            for (node_type* first = buckets_[n]; first; first = first->next)
            {
                if (equals_(get_key_(first->value), key))
                {
                    // 找到第一个匹配的元素
                    for (node_type* cur = first->next; cur; cur = cur->next)
                    {
                        if (!equals_(get_key_(cur->value), key))
                            return mystl::pair<iterator, iterator>(iterator(first, this),
                                                                 iterator(cur, this));
                    }
                    // 如果所有后续元素都匹配
                    for (size_type m = n + 1; m < buckets_.size(); ++m)
                    {
                        if (buckets_[m])
                            return mystl::pair<iterator, iterator>(iterator(first, this),
                                                                 iterator(buckets_[m], this));
                    }
                    return mystl::pair<iterator, iterator>(iterator(first, this), end());
                }
            }
            return mystl::pair<iterator, iterator>(end(), end());
        }

        mystl::pair<const_iterator, const_iterator> equal_range(const key_type& key) const
        {
            size_type n = bucket_num(key);
            for (const node_type* first = buckets_[n]; first; first = first->next)
            {
                if (equals_(get_key_(first->value), key))
                {
                    for (const node_type* cur = first->next; cur; cur = cur->next)
                    {
                        if (!equals_(get_key_(cur->value), key))
                            return mystl::pair<const_iterator, const_iterator>(
                                const_iterator(first, this),
                                const_iterator(cur, this));
                    }
                    for (size_type m = n + 1; m < buckets_.size(); ++m)
                    {
                        if (buckets_[m])
                            return mystl::pair<const_iterator, const_iterator>(
                                const_iterator(first, this),
                                const_iterator(buckets_[m], this));
                    }
                    return mystl::pair<const_iterator, const_iterator>(
                        const_iterator(first, this), end());
                }
            }
            return mystl::pair<const_iterator, const_iterator>(end(), end());
        }

    private:
        // 内部工具函数
        void initialize_buckets(size_type n)
        {
            const size_type n_buckets = next_size(n);
            buckets_.reserve(n_buckets);
            buckets_.insert(buckets_.end(), n_buckets, nullptr);
            num_elements_ = 0;
        }

        size_type next_size(size_type n) const
        {
            for (int i = 0; i < num_primes; ++i)
            {
                if (prime_list[i] >= n)
                    return prime_list[i];
            }
            return prime_list[num_primes - 1];
        }

        node_type* new_node(const value_type& obj)
        {
            node_type* n = node_alloc_.allocate(1);
            n->next = nullptr;
            try 
            {
                mystl::construct(&n->value, obj);
                return n;
            }
            catch (...) 
            {
                node_alloc_.deallocate(n, 1);
                throw;
            }
        }

        void delete_node(node_type* n)
        {
            mystl::destroy_at(&n->value);
            node_alloc_.deallocate(n, 1);
        }

        size_type bucket_num(const value_type& obj) const
        {
            return bucket_num_key(get_key_(obj));
        }

        size_type bucket_num_key(const key_type& key) const
        {
            return hash_(key) % buckets_.size();
        }

        void resize(size_type num_elements_hint)
        {
            const size_type old_n = buckets_.size();
            if (num_elements_hint > old_n) 
            {
                const size_type n = next_size(num_elements_hint);
                if (n > old_n) 
                {
                    mystl::vector<node_type*> tmp(n, nullptr);
                    try 
                    {
                        for (size_type bucket = 0; bucket < old_n; ++bucket) 
                        {
                            node_type* first = buckets_[bucket];
                            while (first) 
                            {
                                size_type new_bucket = hash_(get_key_(first->value)) % n;
                                buckets_[bucket] = first->next;
                                first->next = tmp[new_bucket];
                                tmp[new_bucket] = first;
                                first = buckets_[bucket];
                            }
                        }
                        buckets_.swap(tmp);
                    }
                    catch (...) 
                    {
                        for (size_type bucket = 0; bucket < tmp.size(); ++bucket) 
                        {
                            while (tmp[bucket]) 
                            {
                                node_type* next = tmp[bucket]->next;
                                delete_node(tmp[bucket]);
                                tmp[bucket] = next;
                            }
                        }
                        throw;
                    }
                }
            }
        }

        mystl::pair<iterator, bool> insert_unique_noresize(const value_type& obj)
        {
            const size_type n = bucket_num(obj);
            node_type* first = buckets_[n];

            for (node_type* cur = first; cur; cur = cur->next)
            {
                if (equals_(get_key_(cur->value), get_key_(obj)))
                    return mystl::pair<iterator, bool>(iterator(cur, this), false);
            }

            node_type* tmp = new_node(obj);
            tmp->next = first;
            buckets_[n] = tmp;
            ++num_elements_;
            return mystl::pair<iterator, bool>(iterator(tmp, this), true);
        }

        iterator insert_equal_noresize(const value_type& obj)
        {
            const size_type n = bucket_num(obj);
            node_type* first = buckets_[n];

            node_type* tmp = new_node(obj);
            tmp->next = first;
            buckets_[n] = tmp;
            ++num_elements_;
            return iterator(tmp, this);
        }

    private:
        hasher hash_;
        key_equal equals_;
        ExtractKey get_key_;
        
        node_allocator node_alloc_;
        mystl::vector<node_type*> buckets_;
        size_type num_elements_;

        static const int num_primes = 28;
        static const unsigned long prime_list[num_primes];
    };

    // 在类外定义质数表
    template <class V, class K, class HF, class ExK, class EqK, class A>
    const unsigned long hashtable<V, K, HF, ExK, EqK, A>::prime_list[num_primes] = 
    {
        53,         97,           193,         389,       769,
        1543,       3079,         6151,        12289,     24593,
        49157,      98317,        196613,      393241,    786433,
        1572869,    3145739,      6291469,     12582917,  25165843,
        50331653,   100663319,    201326611,   402653189, 805306457,
        1610612741, 3221225473ul, 4294967291ul
    };

    // 在迭代器类中实现自增操作：
    template <class Value, class Key, class HashFcn,
              class ExtractKey, class EqualKey, class Alloc>
    typename hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::self&
    hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::operator++()
    {
        const node* old = cur;
        cur = cur->next;  // 先走链表
        if (!cur) 
        {
            // 如果链表走完了，就找下一个非空的桶
            size_type bucket = ht->bucket_num(old->value);
            while (!cur && ++bucket < ht->buckets_.size())
                cur = ht->buckets_[bucket];
        }
        return *this;
    }

    template <class Value, class Key, class HashFcn,
              class ExtractKey, class EqualKey, class Alloc>
    typename hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::self
    hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::operator++(int)
    {
        self tmp = *this;
        ++*this;
        return tmp;
    }

    // 在文件末尾添加 const_iterator 的自增操作实现
    template <class Value, class Key, class HashFcn,
              class ExtractKey, class EqualKey, class Alloc>
    typename hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::self&
    hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::operator++()
    {
        using size_type = typename base::size_type;  // 使用基类的 size_type
        const node* old = cur;
        cur = cur->next;
        if (!cur) 
        {
            size_type bucket = ht->bucket_num(old->value);
            while (!cur && ++bucket < ht->buckets_.size())
                cur = ht->buckets_[bucket];
        }
        return *this;
    }

    template <class Value, class Key, class HashFcn,
              class ExtractKey, class EqualKey, class Alloc>
    typename hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::self
    hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::operator++(int)
    {
        self tmp = *this;
        ++*this;
        return tmp;
    }
} // namespace mystl