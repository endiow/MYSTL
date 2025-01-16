#pragma once

#include <initializer_list>

#include "allocator.hpp"
#include "algorithm.hpp"

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
        
        // 默认构造函数
        list_node() : list_node_base<T>() , data() {}  
        
        // 可变参数构造函数
        template <class... Args>
        explicit list_node(Args&&... args) 
            : list_node_base<T>() , data(mystl::forward<Args>(args)...) {}
    };



    /*****************************************************************************************/
    // list 的迭代器设计
    /*****************************************************************************************/
    // list迭代器基类
    template <class T>
    struct list_iterator_base 
    {
        using node_base_ptr = list_node_base<typename remove_const<T>::type>*;
        node_base_ptr node;  // 指向当前节点的指针
        
        list_iterator_base() noexcept : node(nullptr) {}
        explicit list_iterator_base(node_base_ptr x) noexcept : node(x) {}
        
        // 迭代器的前进和后退操作
        void increment() noexcept { node = node->next; }
        void decrement() noexcept { node = node->prev; }
    };

    // list迭代器
    template <class T>
    struct list_iterator : public list_iterator_base<T> 
    {
        using base = list_iterator_base<typename mystl::allocator<T>::value_type>;
        using iterator_category = mystl::bidirectional_iterator_tag;
        using value_type = typename mystl::allocator<T>::value_type;
        using pointer = typename mystl::allocator<T>::pointer;
        using reference = typename mystl::allocator<T>::reference;
        using difference_type = ptrdiff_t;
        using node_ptr = list_node<typename remove_const<value_type>::type>*;
        
        using base::node;
        
        list_iterator() noexcept = default;
        explicit list_iterator(node_ptr x) noexcept : base(x) {}
        
        reference operator*() const noexcept 
        { 
            return static_cast<node_ptr>(this->node)->data; 
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

        // 从非const迭代器到const迭代器的隐式转换
        operator list_iterator<const T>() const noexcept
        { 
            return list_iterator<const T>(static_cast<node_ptr>(this->node));
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
        using value_type = typename Allocator::value_type;
        using allocator_type = Allocator;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using reference = typename allocator_type::reference;
        using const_reference = typename allocator_type::const_reference;
        using pointer = typename allocator_type::pointer;
        using const_pointer = typename allocator_type::const_pointer;
        
        using iterator = list_iterator<value_type>;
        using const_iterator = list_iterator<const value_type>;
        using reverse_iterator = mystl::reverse_iterator<iterator>;
        using const_reverse_iterator = mystl::reverse_iterator<const_iterator>;
        
    private:
        using node_type = list_node<value_type>;
        using node_base = list_node_base<value_type>;
        //分配器类型绑定为node_type
        using node_allocator = typename Allocator::template rebind<node_type>::other;
        
        node_type* node_;     // 指向末尾的空节点
        size_type size_;      // 大小
        node_allocator alloc_;// 节点分配器

    

    public:
        /*****************************************************************************************/
        // 构造、析构函数
        /*****************************************************************************************/
        list() 
        {
            empty_initialize();
        }

        explicit list(size_type n)
        {
            empty_initialize();
            try 
            {
                for (size_type i = 0; i < n; ++i)
                {
                    node_type* new_node = create_node();
                    link_node_at_back(new_node);
                }
            }
            catch (...)
            {
                clear();
                alloc_.deallocate(static_cast<node_type*>(node_), 1);
                throw;
            }
        }

        list(size_type n, const T& value)
        {
            empty_initialize();
            try 
            {
                for (size_type i = 0; i < n; ++i)
                {
                    node_type* new_node = create_node(value);
                    link_node_at_back(new_node);
                }
            }
            catch (...)
            {
                clear();
                alloc_.deallocate(static_cast<node_type*>(node_), 1);
                throw;
            }
        }

        // 初始化列表构造函数
        list(std::initializer_list<T> ilist)
        {
            empty_initialize();
            try 
            {
                for (const auto& value : ilist)
                {
                    node_type* new_node = create_node(value);
                    link_node_at_back(new_node);
                }
            }
            catch (...)
            {
                clear();
                alloc_.deallocate(static_cast<node_type*>(node_), 1);
                throw;
            }
        }

        // 构造函数区域
        template <class InputIt>
        list(InputIt first, InputIt last,
             typename enable_if<!is_integral<InputIt>::value>::type* = nullptr)  
        {
            empty_initialize();
            try 
            {
                for (; first != last; ++first)
                {
                    node_type* new_node = create_node(*first);
                    link_node_at_back(new_node);
                }
            }
            catch (...)
            {
                clear();
                alloc_.deallocate(static_cast<node_type*>(node_), 1);
                throw;
            }
        }

        list(const list& other) 
        {
            empty_initialize();
            try 
            {
                for (const auto& value : other)
                {
                    node_type* new_node = create_node(value);
                    link_node_at_back(new_node);
                }
            }
            catch (...)
            {
                clear();
                alloc_.deallocate(static_cast<node_type*>(node_), 1);
                throw;
            }
        }

        list(list&& other) noexcept
            : node_(other.node_), size_(other.size_)
        {
            other.node_ = nullptr;
            other.size_ = 0;
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

        const_iterator cbegin() const noexcept 
        { return const_iterator(static_cast<node_type*>(node_->next)); }
        
        iterator end() noexcept 
        { return iterator(static_cast<node_type*>(node_)); }
        
        const_iterator end() const noexcept 
        { return const_iterator(static_cast<node_type*>(node_)); }

        const_iterator cend() const noexcept 
        { return const_iterator(static_cast<node_type*>(node_)); }

        // 反向迭代器
        reverse_iterator rbegin() noexcept 
        { return reverse_iterator(end()); }

        const_reverse_iterator rbegin() const noexcept 
        { return const_reverse_iterator(end()); }

        const_reverse_iterator crbegin() const noexcept 
        { return const_reverse_iterator(end()); }

        reverse_iterator rend() noexcept 
        { return reverse_iterator(begin()); }

        const_reverse_iterator rend() const noexcept 
        { return const_reverse_iterator(begin()); }

        const_reverse_iterator crend() const noexcept 
        { return const_reverse_iterator(begin()); }



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
        // 容量相关操作
        /*****************************************************************************************/
        bool empty() const noexcept { return size_ == 0; }
        size_type size() const noexcept { return size_; }
        size_type max_size() const noexcept { return mystl::max_size(alloc_); }
        // 调整大小
        void resize(size_type count, const T& value)
        {
            if (count > size_)
            {
                size_type n = count - size_;
                list tmp;  // 创建临时链表存储新节点
                
                try 
                {
                    // 先在临时链表中构造所有新节点
                    for (size_type i = 0; i < n; ++i)
                    {
                        tmp.push_back(value);
                    }
                    // 如果全部构造成功，再将临时链表拼接到当前链表
                    splice(end(), tmp);
                }
                catch (...) 
                {
                    // tmp 的析构函数会自动清理已构造的节点
                    throw;
                }
            }
            else
            {
                // 缩小不会抛出异常
                while (size_ > count)
                {
                    pop_back();
                }
            }
        }

        void resize(size_type count)
        {
            resize(count, T());
        }



        /*****************************************************************************************/
        // 赋值操作
        /*****************************************************************************************/
        list& operator=(const list& other) 
        {
            if (this != &other) 
            {
                clear();  // 清空当前内容
                try 
                {
                    for (const auto& value : other)
                    {
                        node_type* new_node = create_node(value);
                        link_node_at_back(new_node);
                    }
                }
                catch (...)
                {
                    clear();
                    throw;
                }
            }
            return *this;
        }

        list& operator=(list&& other) noexcept
        {
            if (this != &other)
            {
                clear();
                node_ = other.node_;
                size_ = other.size_;
                other.node_ = nullptr;
                other.size_ = 0;
            }
            return *this;
        }

        void assign(size_type count, const T& value)
        {
            clear();
            try 
            {
                for (size_type i = 0; i < count; ++i)
                {
                    node_type* new_node = create_node(value);
                    link_node_at_back(new_node);
                }
            }
            catch (...)
            {
                clear();
                throw;
            }
        }

        template <class InputIt>
        void assign(InputIt first, InputIt last,
                   typename enable_if<!is_integral<InputIt>::value>::type* = nullptr)
        {
            clear();
            try 
            {
                for (; first != last; ++first)
                {
                    node_type* new_node = create_node(*first);
                    link_node_at_back(new_node);
                }
            }
            catch (...)
            {
                clear();
                throw;
            }
        }

        void assign(std::initializer_list<T> ilist)
        {
            clear();
            try 
            {
                for (const auto& value : ilist)
                {
                    node_type* new_node = create_node(value);
                    link_node_at_back(new_node);
                }
            }
            catch (...)
            {
                clear();
                throw;
            }
        }



        /*****************************************************************************************/
        // 修改器
        /*****************************************************************************************/
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

        // 在pos之前插入一个元素
        iterator insert(const_iterator pos, const T& value)
        {
            node_type* p = insert_node(reinterpret_cast<node_type*>(pos.node), value);
            return iterator(p);
        }

        // 在pos之前插入一个元素(移动版本)
        iterator insert(const_iterator pos, T&& value)
        {
            node_type* p = insert_node(reinterpret_cast<node_type*>(pos.node), mystl::move(value));
            return iterator(p);
        }

        // 在pos之前插入n个元素
        iterator insert(const_iterator pos, size_type n, const T& value)
        {
            if (n == 0)
                return iterator(reinterpret_cast<node_type*>(pos.node));
            
            list tmp;  // 创建临时链表
            try 
            {
                // 先在临时链表中构造所有新节点
                for (size_type i = 0; i < n; ++i)
                {
                    tmp.push_back(value);
                }
                // 如果全部构造成功，再将临时链表拼接到指定位置
                iterator result(reinterpret_cast<node_type*>(pos.node));
                tmp.transfer(tmp.begin(), tmp.end(), pos);
                return result;
            }
            catch (...) 
            {
                // tmp 的析构函数会自动清理已构造的节点
                throw;
            }
        }

        // 在pos之前插入[first, last)范围内的元素
        template <class InputIt>
        iterator insert(const_iterator pos, InputIt first, InputIt last, 
                        typename enable_if<!is_integral<InputIt>::value>::type* = nullptr)
        {
            list tmp;  // 创建临时链表
            try 
            {
                // 先在临时链表中构造所有新节点
                for (; first != last; ++first)
                {
                    tmp.push_back(*first);
                }
                // 如果全部构造成功，再将临时链表拼接到指定位置
                iterator result(reinterpret_cast<node_type*>(pos.node));
                tmp.transfer(tmp.begin(), tmp.end(), pos);
                return result;
            }
            catch (...) 
            {
                // tmp 的析构函数会自动清理已构造的节点
                throw;
            }
        }

        // 在pos之前插入初始化列表中的元素
        iterator insert(const_iterator pos, std::initializer_list<T> ilist)
        {
            return insert(pos, ilist.begin(), ilist.end());
        }


        // 在pos之前原地构造一个元素
        template <class... Args>
        iterator emplace(const_iterator pos, Args&&... args)
        {
            // 创建一个临时节点
            node_type* new_node = nullptr;
            try 
            {
                // 先创建并构造新节点
                new_node = create_node(mystl::forward<Args>(args)...);
                
                // 如果节点创建成功，将其插入到链表中
                new_node->next = pos.node;
                new_node->prev = pos.node->prev;
                pos.node->prev->next = new_node;
                pos.node->prev = new_node;
                ++size_;
                
                return iterator(new_node);
            }
            catch (...) 
            {
                // 如果发生异常，清理已分配的资源
                if (new_node)
                {
                    destroy_node(new_node);
                }
                throw;
            }
        }

        // 在末尾原地构造一个元素
        template <class... Args>
        reference emplace_back(Args&&... args)
        {
            return *emplace(end(), mystl::forward<Args>(args)...);
        }

        // 在开头原地构造一个元素
        template <class... Args>
        reference emplace_front(Args&&... args)
        {
            return *emplace(begin(), mystl::forward<Args>(args)...);
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


        // 删除pos位置的元素
        iterator erase(const_iterator pos)
        {
            node_type* node = reinterpret_cast<node_type*>(pos.node);
            node_type* next_node = reinterpret_cast<node_type*>(node->next);
            node->prev->next = node->next;
            node->next->prev = node->prev;
            destroy_node(node);
            --size_;
            return iterator(next_node);
        }

        // 删除[first, last)范围内的元素，返回最后一个被删除元素的下一个位置
        iterator erase(const_iterator first, const_iterator last)
        {
            if (first == last)
            {
                return iterator(reinterpret_cast<node_type*>(first.node));
            }

            // 保存last的前一个节点，因为last指向的是要保留的节点
            node_base* prev = first.node->prev;
            
            // 删除范围内的节点
            while (first != last)
            {
                node_type* tmp = static_cast<node_type*>(first.node);
                ++first;  // 先递增迭代器，再删除节点
                destroy_node(tmp);
                --size_;
            }
            
            // 重新链接
            prev->next = last.node;
            last.node->prev = prev;
            
            return iterator(reinterpret_cast<node_type*>(last.node));
        }


        // 交换两个list
        void swap(list& other) noexcept
        {
            mystl::swap(node_, other.node_);
            mystl::swap(size_, other.size_);
            mystl::swap(alloc_, other.alloc_);
        }

        

        /*****************************************************************************************/
        // list相关算法 
        /*****************************************************************************************/
        // 合并两个链表
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

        
        // 排序
        template <class Compare>
        void sort(Compare comp) 
        {
            if (size_ <= 1) return;
            list carry;
            list counter[64];
            int fill = 0;
            while (!empty()) 
            {
                carry.splice(carry.begin(), *this, begin());
                int i = 0;
                while (i < fill && !counter[i].empty()) 
                {
                    counter[i].merge(carry, comp);
                    carry.swap(counter[i++]);
                }
                carry.swap(counter[i]);
                if (i == fill) ++fill;
            }
            for (int i = 1; i < fill; ++i)
                counter[i].merge(counter[i-1], comp);
            swap(counter[fill-1]);
        }

        void sort() 
        {
            sort(mystl::less<T>());
        }


        // 拼接
        void splice(const_iterator pos, list& other) 
        {
            if (!other.empty()) 
            {
                transfer(pos, other.begin(), other.end());
                size_ += other.size_;
                other.size_ = 0;
            }
        }

        void splice(const_iterator pos, list& other, const_iterator it) 
        {
            const_iterator next = it;
            ++next;
            if (pos != it && pos != next) 
            {
                transfer(pos, it, next);
                ++size_;
                --other.size_;
            }
        }

        void splice(const_iterator pos, list& other, const_iterator first, const_iterator last) 
        {
            if (first != last) 
            {
                size_ += mystl::distance(first, last);
                other.size_ -= mystl::distance(first, last);
                transfer(pos, first, last);
            }
        }



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
                alloc_.construct(&p->data, mystl::forward<Args>(args)...);  // 只构造数据部分
                p->next = p;  // 初始化为自环
                p->prev = p;
                return p;
            }
            catch(...)
            {
                alloc_.deallocate(p, 1);
                throw;
            }
        }

        void destroy_node(node_type* p)
        {
            alloc_.destroy(p);
            alloc_.deallocate(p, 1);
        }

        // 初始化空链表
        void empty_initialize()
        {
            node_ = alloc_.allocate(1);  // 分配空节点
            node_->next = node_;  // 空链表时指向自身
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
        void transfer(const_iterator pos, const_iterator first, const_iterator last)
        {
            if (pos != last) 
            {
                // 计算要移动的节点数量
                size_type n = mystl::distance(first, last);
                
                // 重新链接指针
                last.node->prev->next = pos.node;
                first.node->prev->next = last.node;
                pos.node->prev->next = first.node;
                
                node_base* tmp = pos.node->prev;
                pos.node->prev = last.node->prev;
                last.node->prev = first.node->prev;
                first.node->prev = tmp;
                
                // 更新size
                size_ += n;
            }
        }

        // 在末尾插入节点的辅助函数
        void link_node_at_back(node_type* new_node)
        {
            // 不需要重置指针，因为构造时就是nullptr
            new_node->next = node_;
            new_node->prev = node_->prev;
            node_->prev->next = new_node;
            node_->prev = new_node;
            ++size_;
        }
    };



    //------------------------------------------------------------------------------
    // 非成员函数
    //------------------------------------------------------------------------------    
    template <class T>
    bool operator==(const list<T>& lhs, const list<T>& rhs)
    {
        if (lhs.size() != rhs.size())
            return false;
        return mystl::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template <class T>  
    bool operator!=(const list<T>& lhs, const list<T>& rhs)
    {
        return !(lhs == rhs);
    }

    template <class T>
    bool operator<(const list<T>& lhs, const list<T>& rhs)
    {
        return mystl::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template <class T>
    bool operator<=(const list<T>& lhs, const list<T>& rhs)
    {
        return !(rhs < lhs);
    }   

    template <class T>
    bool operator>(const list<T>& lhs, const list<T>& rhs)
    {
        return rhs < lhs;
    }   

    template <class T>
    bool operator>=(const list<T>& lhs, const list<T>& rhs)
    {
        return !(lhs < rhs);
    }   

    // 交换两个list 
    template <class T>
    void swap(list<T>& lhs, list<T>& rhs) noexcept
    {
        lhs.swap(rhs);
    }   

} // namespace mystl 