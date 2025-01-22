#pragma once
#include "rb_tree.hpp"
#include "functional.hpp"

namespace mystl 
{

    template <class Key, class Compare = mystl::less<Key>, class Alloc = mystl::allocator<Key>>
    class multiset 
    {
    public:
        using key_type = Key;
        using value_type = Key;
        using key_compare = Compare;
        using value_compare = Compare;
        
    private:
        using rb_tree_type = mystl::rb_tree<value_type, key_compare, Alloc>;
        rb_tree_type tree_;  // 底层红黑树
        
    public:
        using pointer = typename rb_tree_type::pointer;
        using const_pointer = typename rb_tree_type::const_pointer;
        using reference = typename rb_tree_type::reference;
        using const_reference = typename rb_tree_type::const_reference;
        using iterator = typename rb_tree_type::const_iterator;
        using const_iterator = typename rb_tree_type::const_iterator;
        using reverse_iterator = typename rb_tree_type::const_reverse_iterator;
        using const_reverse_iterator = typename rb_tree_type::const_reverse_iterator;
        using size_type = typename rb_tree_type::size_type;
        using difference_type = typename rb_tree_type::difference_type;
        using allocator_type = typename rb_tree_type::allocator_type;

    public:
        // 构造函数
        multiset() = default;
        
        template <class InputIterator>
        multiset(InputIterator first, InputIterator last)
        { 
            tree_.insert_equal(first, last); 
        }
        
        multiset(std::initializer_list<value_type> ilist)
        { 
            tree_.insert_equal(ilist.begin(), ilist.end()); 
        }
        
        multiset(const multiset& rhs) 
            : tree_(rhs.tree_) 
        {
        }
        
        multiset(multiset&& rhs) noexcept
            : tree_(mystl::move(rhs.tree_)) 
        {
        }

        // 赋值操作
        multiset& operator=(const multiset& rhs)
        {
            tree_ = rhs.tree_;
            return *this;
        }
        
        multiset& operator=(multiset&& rhs) noexcept
        {
            tree_ = mystl::move(rhs.tree_);
            return *this;
        }
        
        multiset& operator=(std::initializer_list<value_type> ilist)
        {
            tree_.clear();
            tree_.insert_equal(ilist.begin(), ilist.end());
            return *this;
        }

        // 迭代器相关
        iterator begin() noexcept { return tree_.begin(); }
        const_iterator begin() const noexcept { return tree_.begin(); }
        iterator end() noexcept { return tree_.end(); }
        const_iterator end() const noexcept { return tree_.end(); }
        reverse_iterator rbegin() noexcept { return tree_.rbegin(); }
        const_reverse_iterator rbegin() const noexcept { return tree_.rbegin(); }
        reverse_iterator rend() noexcept { return tree_.rend(); }
        const_reverse_iterator rend() const noexcept { return tree_.rend(); }
        const_iterator cbegin() const noexcept { return begin(); }
        const_iterator cend() const noexcept { return end(); }
        const_reverse_iterator crbegin() const noexcept { return rbegin(); }
        const_reverse_iterator crend() const noexcept { return rend(); }

        // 容量相关
        bool empty() const noexcept { return tree_.empty(); }
        size_type size() const noexcept { return tree_.size(); }
        size_type max_size() const noexcept { return tree_.max_size(); }

        // 插入删除操作
        iterator insert(const value_type& value)
        { 
            return tree_.insert_equal(value); 
        }
        
        iterator insert(iterator hint, const value_type& value)
        { 
            return tree_.insert_equal(hint, value); 
        }
        
        template <class InputIterator>
        void insert(InputIterator first, InputIterator last)
        { 
            tree_.insert_equal(first, last); 
        }

        void erase(iterator position) { tree_.erase(position); }
        size_type erase(const key_type& key) { return tree_.erase(key); }
        void erase(iterator first, iterator last) { tree_.erase(first, last); }
        void clear() noexcept { tree_.clear(); }

        // 查找相关
        iterator find(const key_type& key) { return tree_.find(key); }
        const_iterator find(const key_type& key) const { return tree_.find(key); }
        size_type count(const key_type& key) const { return tree_.count(key); }
        iterator lower_bound(const key_type& key) { return tree_.lower_bound(key); }
        const_iterator lower_bound(const key_type& key) const { return tree_.lower_bound(key); }
        iterator upper_bound(const key_type& key) { return tree_.upper_bound(key); }
        const_iterator upper_bound(const key_type& key) const { return tree_.upper_bound(key); }
        mystl::pair<iterator,iterator> equal_range(const key_type& key)
        { return tree_.equal_range(key); }
        mystl::pair<const_iterator,const_iterator> equal_range(const key_type& key) const
        { return tree_.equal_range(key); }

        // 获取比较函数
        key_compare key_comp() const { return tree_.key_comp(); }
        value_compare value_comp() const { return tree_.key_comp(); }

        // 交换操作
        void swap(multiset& rhs) noexcept { tree_.swap(rhs.tree_); }
    };

    // 重载比较操作符
    template <class Key, class Compare, class Alloc>
    bool operator==(const multiset<Key, Compare, Alloc>& lhs,
                    const multiset<Key, Compare, Alloc>& rhs)
    {
        return lhs.tree_ == rhs.tree_;
    }

    template <class Key, class Compare, class Alloc>
    bool operator<(const multiset<Key, Compare, Alloc>& lhs,
                const multiset<Key, Compare, Alloc>& rhs)
    {
        return lhs.tree_ < rhs.tree_;
    }

    // 重载 mystl 的 swap
    template <class Key, class Compare, class Alloc>
    void swap(multiset<Key, Compare, Alloc>& lhs, 
            multiset<Key, Compare, Alloc>& rhs) noexcept
    {
        lhs.swap(rhs);
    }

} // namespace mystl 