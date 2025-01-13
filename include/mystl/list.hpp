#pragma once

#include "allocator.hpp"
#include "iterator.hpp"
#include "util.hpp"
#include "functional.hpp"

namespace mystl 
{
    /*****************************************************************************************/
    // list 的节点设计
    /*****************************************************************************************/
    // list节点的基类
    template <class T>
    struct list_node_base 
    {
        list_node_base* prev;  // 前一个节点
        list_node_base* next;  // 后一个节点
        
        list_node_base() noexcept : prev(nullptr), next(nullptr) {}
    };

    // list节点类
    template <class T>
    struct list_node : public list_node_base<T> 
    {
        T data;  // 数据域
        
        list_node() = default;
        explicit list_node(const T& value) : data(value) {}
        explicit list_node(T&& value) noexcept : data(mystl::move(value)) {}
    };



    /*****************************************************************************************/
    // list 的迭代器设计
    /*****************************************************************************************/
    // list迭代器基类
    template <class T>
    struct list_iterator_base 
    {
        using node_ptr = list_node_base<T>*;
        node_ptr node;  // 指向当前节点的指针
        
        list_iterator_base() noexcept : node(nullptr) {}
        explicit list_iterator_base(node_ptr x) noexcept : node(x) {}
        
        void increment() noexcept { node = node->next; }  // 前进一个节点
        void decrement() noexcept { node = node->prev; }  // 后退一个节点
    };

    // list迭代器
    template <class T>
    struct list_iterator : public list_iterator_base<T> 
    {
        using base = list_iterator_base<T>;
        using iterator_category = mystl::bidirectional_iterator_tag;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using difference_type = ptrdiff_t;
        using node_ptr = list_node<T>*;
        
        using base::node;
        
        list_iterator() noexcept = default;
        explicit list_iterator(node_ptr x) noexcept : base(x) {}
        
        reference operator*() const noexcept 
        { 
            return static_cast<node_ptr>(node)->data; 
        }
        
        pointer operator->() const noexcept 
        { 
            return &(operator*()); 
        }
        
        list_iterator& operator++() noexcept 
        {
            this->increment();
            return *this;
        }
        
        list_iterator operator++(int) noexcept 
        {
            list_iterator tmp = *this;
            this->increment();
            return tmp;
        }
        
        list_iterator& operator--() noexcept 
        {
            this->decrement();
            return *this;
        }
        
        list_iterator operator--(int) noexcept 
        {
            list_iterator tmp = *this;
            this->decrement();
            return tmp;
        }
        
        bool operator==(const list_iterator& rhs) const noexcept 
        { 
            return node == rhs.node; 
        }
        
        bool operator!=(const list_iterator& rhs) const noexcept 
        { 
            return node != rhs.node; 
        }
    };



    /*****************************************************************************************/
    // list 类的定义
    /*****************************************************************************************/
    template <class T, class Allocator = mystl::allocator<T>>
    class list 
    {
    public:
        // 类型定义
        using value_type = T;
        using allocator_type = Allocator;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = T*;                    // 直接使用原生指针
        using const_pointer = const T*;
        using iterator = list_iterator<T>;
        using const_iterator = list_iterator<const T>;
        using reverse_iterator = mystl::reverse_iterator<iterator>;
        using const_reverse_iterator = mystl::reverse_iterator<const_iterator>;
        
    private:
        using node_type = list_node<T>;
        using node_base = list_node_base<T>;
        using node_allocator = Allocator;      // 直接使用传入的分配器
        
        node_type* node_;     // 指向末尾的空节点
        size_type size_;      // 大小
        node_allocator alloc_;// 节点分配器

    private:
        /*****************************************************************************************/
        // 辅助函数
        /*****************************************************************************************/
        // 创建节点
        template <class... Args>
        node_type* create_node(Args&&... args)
        {
            node_type* p = alloc_.allocate(1);
            try 
            {
                alloc_.construct(p, mystl::forward<Args>(args)...);
                p->next = p;
                p->prev = p;
            }
            catch(...) 
            {
                alloc_.deallocate(p);
                throw;
            }
            return p;
        }

        void destroy_node(node_type* p)
        {
            alloc_.destroy(p);
            alloc_.deallocate(p);
        }

        // 初始化空链表
        void empty_initialize()
        {
            node_ = alloc_.allocate(1);  // 分配空节点
            node_->next = node_;
            node_->prev = node_;
            size_ = 0;
        }

        // 在pos之前插入一个节点
        template <class... Args>
        node_type* insert_node(node_type* pos, Args&&... args)
        {
            node_type* new_node = create_node(mystl::forward<Args>(args)...);
            new_node->next = pos;
            new_node->prev = pos->prev;
            pos->prev->next = new_node;
            pos->prev = new_node;
            ++size_;
            return new_node;
        }

        // 将[first, last)范围内的元素移动到pos之前
        void transfer(iterator pos, iterator first, iterator last)
        {
            if (pos != last) 
            {
                last.node->prev->next = pos.node;
                first.node->prev->next = last.node;
                pos.node->prev->next = first.node;
                
                node_base* tmp = pos.node->prev;
                pos.node->prev = last.node->prev;
                last.node->prev = first.node->prev;
                first.node->prev = tmp;
            }
        }

    public:
        /*****************************************************************************************/
        // 构造、析构函数
        /*****************************************************************************************/
        list() : size_(0)
        {
            empty_initialize();
        }

        explicit list(size_type n)
        {
            empty_initialize();
            insert(begin(), n, T());
        }

        list(size_type n, const T& value)
        {
            empty_initialize();
            insert(begin(), n, value);
        }

        // 析构函数
        ~list()
        {
            clear();
            alloc_.deallocate(static_cast<node_type*>(node_), 1);
        }



        /*****************************************************************************************/
        // 迭代器相关操作
        /*****************************************************************************************/
        iterator begin() noexcept 
        { return iterator(static_cast<node_type*>(node_->next)); }
        
        const_iterator begin() const noexcept 
        { return const_iterator(static_cast<node_type*>(node_->next)); }
        
        iterator end() noexcept 
        { return iterator(static_cast<node_type*>(node_)); }
        
        const_iterator end() const noexcept 
        { return const_iterator(static_cast<node_type*>(node_)); }



        /*****************************************************************************************/
        // 容量相关操作
        /*****************************************************************************************/
        bool empty() const noexcept { return size_ == 0; }
        size_type size() const noexcept { return size_; }



        /*****************************************************************************************/
        // 访问元素操作
        /*****************************************************************************************/
        reference front() 
        { 
            EXPECT(!empty(), "list is empty");
            return *begin(); 
        }
        
        const_reference front() const 
        { 
            EXPECT(!empty(), "list is empty");
            return *begin(); 
        }
        
        reference back() 
        { 
            EXPECT(!empty(), "list is empty");
            return *(--end()); 
        }
        
        const_reference back() const 
        { 
            EXPECT(!empty(), "list is empty");
            return *(--end()); 
        }



        /*****************************************************************************************/
        // 修改容器操作
        /*****************************************************************************************/
        iterator insert(const_iterator pos, const T& value)
        {
            node_type* p = insert_node(static_cast<node_type*>(pos.node), value);
            return iterator(p);
        }

        // 在pos之前插入一个元素(移动版本)
        iterator insert(const_iterator pos, T&& value)
        {
            node_type* p = insert_node(static_cast<node_type*>(pos.node), mystl::move(value));
            return iterator(p);
        }

        // 在pos之前插入n个元素
        iterator insert(const_iterator pos, size_type n, const T& value)
        {
            if (n == 0)
                return iterator(const_cast<node_type*>(static_cast<const node_type*>(pos.node)));
            
            iterator result(pos.node);
            for (size_type i = 0; i < n; ++i)
                result = insert(pos, value);
            return result;
        }

        // 在pos之前插入[first, last)范围内的元素
        template <class InputIt>
        iterator insert(const_iterator pos, InputIt first, InputIt last)
        {
            iterator result(pos.node);
            for (; first != last; ++first)
                result = insert(pos, *first);
            return result;
        }

        // 在pos之前插入初始化列表中的元素
        iterator insert(const_iterator pos, std::initializer_list<T> ilist)
        {
            return insert(pos, ilist.begin(), ilist.end());
        }

        // 清空列表
        void clear() noexcept
        {
            node_type* cur = static_cast<node_type*>(node_->next);
            while (cur != node_) 
            {
                node_type* tmp = cur;
                cur = static_cast<node_type*>(cur->next);
                destroy_node(tmp);
            }
            node_->next = node_;
            node_->prev = node_;
            size_ = 0;
        }

        // 删除pos位置的元素
        iterator erase(const_iterator pos)
        {
            node_type* node = static_cast<node_type*>(pos.node);
            node_type* next_node = static_cast<node_type*>(node->next);
            node->prev->next = node->next;
            node->next->prev = node->prev;
            destroy_node(node);
            --size_;
            return iterator(next_node);
        }

        // 在末尾添加元素
        void push_back(const T& value)
        {
            insert(end(), value);
        }

        void push_back(T&& value)
        {
            insert(end(), mystl::move(value));
        }

        // 在开头添加元素
        void push_front(const T& value)
        {
            insert(begin(), value);
        }

        void push_front(T&& value)
        {
            insert(begin(), mystl::move(value));
        }

        // 删除末尾元素
        void pop_back()
        {
            EXPECT(!empty(), "list is empty");
            erase(--end());
        }

        // 删除开头元素
        void pop_front()
        {
            EXPECT(!empty(), "list is empty");
            erase(begin());
        }

        // 调整大小
        void resize(size_type count)
        {
            resize(count, T());
        }

        void resize(size_type count, const T& value)
        {
            if (count > size_)
                insert(end(), count - size_, value);
            else
                while (size_ > count)
                    pop_back();
        }

        // 交换两个list
        void swap(list& other) noexcept
        {
            mystl::swap(node_, other.node_);
            mystl::swap(size_, other.size_);
            mystl::swap(alloc_, other.alloc_);
        }

        

        /*****************************************************************************************/
        // list 相关算法
        /*****************************************************************************************/
        template <class Compare>
        void merge(list& other, Compare comp)
        {
            if (this != &other) 
            {
                iterator first1 = begin();
                iterator last1 = end();
                iterator first2 = other.begin();
                iterator last2 = other.end();

                while (first1 != last1 && first2 != last2) 
                {
                    if (comp(*first2, *first1)) 
                    {
                        iterator next = first2;
                        transfer(first1, first2, ++next);
                        first2 = next;
                    }
                    else
                        ++first1;
                }

                if (first2 != last2)
                    transfer(last1, first2, last2);

                size_ += other.size_;
                other.size_ = 0;
            }
        }

        void merge(list& other)
        {
            merge(other, mystl::less<T>());
        }


        // 反转链表
        void reverse() noexcept
        {
            if (size_ <= 1) return;
            iterator first = begin();
            ++first;
            while (first != end()) 
            {
                iterator old = first;
                ++first;
                transfer(begin(), old, first);
            }
        }

        
        // 移除所有满足谓词的元素
        template <class UnaryPredicate>
        void remove_if(UnaryPredicate pred)
        {
            iterator first = begin();
            iterator last = end();
            while (first != last) 
            {
                iterator next = first;
                ++next;
                if (pred(*first))
                    erase(first);
                first = next;
            }
        }

        // 移除所有值为value的元素
        void remove(const T& value)
        {
            remove_if([&](const T& x) { return x == value; });
        }


        // 移除连续重复元素
        template <class BinaryPredicate>
        void unique(BinaryPredicate pred)
        {
            iterator first = begin();
            iterator last = end();
            if (first == last) return;
            iterator next = first;
            while (++next != last) 
            {
                if (pred(*first, *next))
                    erase(next);
                else
                    first = next;
                next = first;
            }
        }

        void unique()
        {
            unique(mystl::equal_to<T>());
        }
    };

} // namespace mystl 