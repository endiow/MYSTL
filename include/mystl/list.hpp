#pragma once

#include <initializer_list>

#include "allocator.hpp"
#include "algorithm.hpp"

namespace mystl 
{
    /*****************************************************************************************/
    // list 的节点设计
    /*****************************************************************************************/
    // list节点的基类   专门处理节点之间的链接关系    都是无异常的
    template <class T>
    struct list_node_base 
    {
        list_node_base* prev;  // 前一个节点
        list_node_base* next;  // 后一个节点
        
        list_node_base() noexcept : prev(nullptr), next(nullptr) {}
    };

    // list节点类   专门存储数据
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
    // list迭代器基类   专门处理迭代器的移动    都是无异常的
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

    // list迭代器   专门处理数据访问
    template <class T>
    struct list_iterator : public list_iterator_base<T> 
    {
        using iterator_base = list_iterator_base<typename mystl::allocator<T>::value_type>;
        using iterator_category = mystl::bidirectional_iterator_tag;
        using value_type = typename mystl::allocator<T>::value_type;
        using pointer = typename mystl::allocator<T>::pointer;
        using reference = typename mystl::allocator<T>::reference;
        using difference_type = ptrdiff_t;
        using node_ptr = list_node<typename remove_const<value_type>::type>*;
        
        using iterator_base::node;
        
        list_iterator() noexcept = default;
        explicit list_iterator(node_ptr x) noexcept : iterator_base(x) {}

        // 从非const迭代器到const迭代器的隐式转换
        operator list_iterator<const T>() const noexcept
        { 
            return list_iterator<const T>(static_cast<const node_ptr>(get_node()));
        }

        // 返回底层指针 (reverse_iterator 需要使用此函数)
        node_ptr base() const noexcept
        {
            return get_node();
        }

        // 获取当前节点
        node_ptr get_node() const noexcept
        {
            return static_cast<node_ptr>(this->node);
        }
        
        
        reference operator*() const noexcept 
        { 
            return get_node()->data; 
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

        // 处理 const node_base* 的版本
        node_type* as_node(const node_base* ptr) const noexcept
        {
            return static_cast<node_type*>(const_cast<node_base*>(ptr));
        }

        // 处理 node_base* 的版本
        node_type* as_node(node_base* ptr) noexcept
        {
            return static_cast<node_type*>(ptr);
        }

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
                alloc_.deallocate(as_node(node_), 1);
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
                alloc_.deallocate(as_node(node_), 1);
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
                alloc_.deallocate(as_node(node_), 1);
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
                alloc_.deallocate(as_node(node_), 1);
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
                alloc_.deallocate(as_node(node_), 1);
                throw;
            }
        }

        list(list&& other) noexcept
            : node_(other.node_), 
              size_(other.size_),
              alloc_(mystl::move(other.alloc_))  // 移动分配器
        {
            // 创建一个空节点给 other
            other.node_ = other.alloc_.allocate(1);
            other.node_->next = other.node_;
            other.node_->prev = other.node_;
            other.size_ = 0;
        }

        // 析构函数
        ~list()
        {
            clear();
            alloc_.deallocate(as_node(node_), 1);
        }



        /*****************************************************************************************/
        // 迭代器相关操作
        /*****************************************************************************************/
        iterator begin() noexcept 
        { 
            return iterator(as_node(node_->next)); 
        }
        
        const_iterator begin() const noexcept 
        { 
            return const_iterator(as_node(node_->next)); 
        }

        const_iterator cbegin() const noexcept 
        { 
            return const_iterator(as_node(node_->next)); 
        }
        
        iterator end() noexcept 
        { 
            return iterator(as_node(node_)); 
        }
        
        const_iterator end() const noexcept 
        { 
            return const_iterator(as_node(node_)); 
        }

        const_iterator cend() const noexcept 
        { 
            return const_iterator(as_node(node_)); 
        }

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
                // 清理当前资源
                clear();
                alloc_.deallocate(node_, 1);

                // 移动资源
                node_ = other.node_;
                size_ = other.size_;
                alloc_ = mystl::move(other.alloc_);

                // 重置 other
                other.node_ = other.alloc_.allocate(1);
                other.node_->next = other.node_;
                other.node_->prev = other.node_;
                other.size_ = 0;
            }
            return *this;
        }

        void assign(size_type count, const T& value)
        {
            clear();
            insert(begin(), count, value);
        }

        template <class InputIt>
        void assign(InputIt first, InputIt last,
                   typename enable_if<!is_integral<InputIt>::value>::type* = nullptr)
        {
            clear();
            insert(begin(), first, last);
        }

        void assign(std::initializer_list<T> ilist)
        {
            clear();
            insert(begin(), ilist);
        }



        /*****************************************************************************************/
        // 修改器
        /*****************************************************************************************/
        // 清空列表
        void clear() noexcept
        {
            node_type* cur = as_node(node_->next);
            while (cur != node_) 
            {
                node_type* tmp = cur;
                cur = as_node(cur->next);
                destroy_node(tmp);
            }
            node_->next = node_;
            node_->prev = node_;
            size_ = 0;
        }

        // 在pos之前插入一个元素
        iterator insert(const_iterator pos, const T& value)
        {
            EXPECT(pos.node != nullptr, "iterator is null");
            
            // 使用辅助函数创建和插入节点
            node_type* new_node = insert_node(pos.get_node(), value);
            return iterator(new_node);
        }

        // 在pos之前插入一个元素(移动版本)
        iterator insert(const_iterator pos, T&& value)
        {
            node_type* p = insert_node(pos.get_node(), mystl::move(value));
            return iterator(p);
        }

        // 在pos位置插入 n 个值为 value 的元素
        iterator insert(const_iterator pos, size_type n, const value_type& value)
        {
            if (n == 0)
                return iterator(as_node(pos.node));

            // 先创建一个临时链表存储所有新节点
            list tmp;
            try 
            {
                for (size_type i = 0; i < n; ++i)
                {
                    tmp.push_back(value);  // 可能抛出异常
                }
                // 如果全部创建成功，再一次性插入
                iterator result = iterator(as_node(pos.node));
                splice(pos, tmp);
                return result;
            }
            catch (...)
            {
                // tmp 会自动析构，清理所有已创建的节点
                throw;
            }
        }

        // 在pos之前插入[first, last)范围内的元素
        template <class InputIt>
        iterator insert(const_iterator pos, InputIt first, InputIt last,
                        typename enable_if<!is_integral<InputIt>::value>::type* = nullptr)
        {
            // 先创建一个临时链表存储所有新节点
            list tmp;
            try 
            {
                for (; first != last; ++first)
                {
                    tmp.push_back(*first);  // 可能抛出异常
                }
                // 如果全部创建成功，再一次性插入
                iterator result = iterator(as_node(pos.node));
                splice(pos, tmp);
                return result;
            }
            catch (...)
            {
                // tmp 会自动析构，清理所有已创建的节点
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
            node_type* new_node = nullptr;
            try 
            {
                // 先分配内存
                new_node = alloc_.allocate(1);
                
                // 在已分配的内存上构造对象，这里可能会抛出异常
                mystl::construct(&new_node->data, mystl::forward<Args>(args)...);
                
                // 如果构造成功，再进行指针操作
                new_node->next = pos.node;
                new_node->prev = pos.node->prev;
                pos.node->prev->next = new_node;
                pos.node->prev = new_node;
                ++size_;
                
                return iterator(new_node);
            }
            catch (...)
            {
                // 如果发生异常，清理已分配的内存
                if (new_node)
                {
                    alloc_.deallocate(new_node, 1);
                }
                throw;  // 重新抛出异常
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
            node_type* node = pos.get_node();
            node_type* next_node = as_node(node->next);
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
                return iterator(as_node(first.node));
            }

            // 保存last的前一个节点，因为last指向的是要保留的节点
            node_base* prev = first.node->prev;
            
            // 删除范围内的节点
            while (first != last)
            {
                node_type* tmp = as_node(first.node);
                ++first;  // 先递增迭代器，再删除节点
                destroy_node(tmp);
                --size_;
            }
            
            // 重新链接
            prev->next = last.node;
            last.node->prev = prev;
            
            return iterator(as_node(last.node));
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
                    if (comp(*first2, *first1))  // 使用比较器
                    {
                        iterator next = first2;
                        ++next;
                        // 移动一个节点
                        node_base* tmp = first2.node;
                        // 从other中断开
                        tmp->prev->next = tmp->next;
                        tmp->next->prev = tmp->prev;
                        // 插入到当前位置
                        node_base* pos = first1.node;
                        tmp->next = pos;
                        tmp->prev = pos->prev;
                        pos->prev->next = tmp;
                        pos->prev = tmp;
                        
                        first2 = next;
                        ++size_;
                        --other.size_;
                    }
                    else
                    {
                        ++first1;
                    }
                }

                // 如果other还有剩余节点，全部接到末尾
                if (first2 != last2)
                {
                    splice(end(), other);
                }
            }
        }

        void merge(list& other)
        {
            merge(other, mystl::less<T>());
        }

        // 反转链表
        void reverse() noexcept
        {
            if (size_ <= 1)
                return;

            // 从第一个节点开始，依次反转指针
            node_base* curr = node_->next;
            node_base* temp = node_;  // 保存头节点
            
            while (curr != node_)
            {
                node_base* next = curr->next;
                // 交换前后指针
                curr->next = temp;
                curr->prev = next;
                // 移动到下一个节点
                temp = curr;
                curr = next;
            }

            // 更新头节点的指针
            node_->next = temp;
            node_->prev = curr->prev;
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
            if (size_ <= 1)
                return;

            // 将链表分成两半
            list left;
            list right;
            iterator mid = begin();
            for (size_type i = 0; i < size_ / 2; ++i)
                ++mid;

            // 分割链表
            left.splice(left.begin(), *this, begin(), mid);
            right.splice(right.begin(), *this);

            // 递归排序
            left.sort(comp);
            right.sort(comp);

            // 合并有序链表
            merge(left, comp);
            merge(right, comp);
        }

        void sort() 
        {
            sort(mystl::less<T>());
        }


        // 将整个链表移动
        void splice(const_iterator pos, list& other)
        {
            if (this != &other && !other.empty())
            {
                size_ += other.size_;
                node_base* first = other.node_->next;
                node_base* last = other.node_->prev;
                
                other.node_->next = other.node_;
                other.node_->prev = other.node_;
                other.size_ = 0;

                node_base* pos_prev = pos.node->prev;
                first->prev = pos_prev;
                pos_prev->next = first;
                last->next = pos.node;
                pos.node->prev = last;
            }
        }

        // 移动单个节点
        void splice(const_iterator pos, list& other, const_iterator it)
        {
            if (pos != it && (this != &other || pos != ++const_iterator(it)))
            {
                node_base* node = it.node;
                node->prev->next = node->next;
                node->next->prev = node->prev;
                --other.size_;

                node_base* pos_prev = pos.node->prev;
                node->prev = pos_prev;
                node->next = pos.node;
                pos_prev->next = node;
                pos.node->prev = node;
                ++size_;
            }
        }

        // 移动范围
        void splice(const_iterator pos, list& other, const_iterator first, const_iterator last)
        {
            if (first == last || (this == &other && pos == last))
                return;

            // 计算节点数量
            size_type n = 0;
            const_iterator it = first;
            while (it != last)
            {
                ++n;
                ++it;
            }

            // 从源链表断开
            node_base* begin = first.node;
            node_base* end = last.node;
            node_base* before_first = begin->prev;
            node_base* last_node = end->prev;

            // 更新源链表的连接
            before_first->next = end;
            end->prev = before_first;
            other.size_ -= n;

            // 插入到目标位置
            node_base* pos_prev = pos.node->prev;

            // 更新目标链表的连接
            begin->prev = pos_prev;
            pos_prev->next = begin;
            last_node->next = pos.node;
            pos.node->prev = last_node;
            size_ += n;
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
                mystl::construct(&p->data, mystl::forward<Args>(args)...);  // 只构造数据部分
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
            mystl::destroy_at(&p->data);
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