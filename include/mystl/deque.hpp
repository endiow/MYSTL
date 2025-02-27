#pragma once

#include <initializer_list>
#include <limits>   // 包含numeric_limits

#include "allocator.hpp"
#include "iterator.hpp"
#include "uninitialized.hpp"    
#include "algorithm.hpp"

namespace mystl 
{

    // 设置 buffer 大小
    inline size_t deque_buf_size(size_t size) 
    {
        return size < 512 ? size_t(512 / size) : size_t(1);
    }

    // deque 的迭代器设计
    template <class T>
    class deque_iterator 
    {
        template <class U>
        friend class deque_iterator;  // 允许不同类型迭代器互相访问

        template <class Value, class Alloc>
        friend class deque;  // 保留 deque 作为友元

    public:
        // 迭代器类型定义
        using iterator_category = mystl::random_access_iterator_tag;
        using value_type = typename mystl::allocator<T>::value_type;  // 使用分配器定义的类型
        using pointer = typename mystl::allocator<T>::pointer;
        using reference = typename mystl::allocator<T>::reference;
        using difference_type = ptrdiff_t;
        using size_type = size_t;

        // 使用分配器的 rebind 获取指针的指针类型
        using map_pointer = typename mystl::allocator<T>::template rebind<pointer>::other::pointer;
        using const_pointer = typename mystl::allocator<T>::const_pointer;
        using const_map_pointer = typename mystl::allocator<T>::template rebind<const_pointer>::other::pointer;

        // 构造函数
        deque_iterator() noexcept
            : cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {}

        deque_iterator(pointer c, pointer f, pointer l, map_pointer n) noexcept
            : cur(c), first(f), last(l), node(n) {}

        deque_iterator(const deque_iterator& rhs) noexcept
            : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {}

        deque_iterator& operator=(const deque_iterator& rhs) noexcept
        {
            if (this != &rhs)
            {
                cur = rhs.cur;
                first = rhs.first;
                last = rhs.last;
                node = rhs.node;
            }
            return *this;
        }

        // 转换为const迭代器
        operator deque_iterator<const T>() const noexcept
        {
            return deque_iterator<const T>(static_cast<const T*>(cur), 
                                           static_cast<const T*>(first),
                                           static_cast<const T*>(last),
                                           const_cast<const_map_pointer>(node));
        }

        // 从const_iterator到iterator的隐式转换
        template <class U>
        deque_iterator(const deque_iterator<U>& other, typename enable_if<is_same<U, const T>::value>::type* = nullptr)
            : cur(const_cast<pointer>(other.cur)), 
              first(const_cast<pointer>(other.first)),
              last(const_cast<pointer>(other.last)),
              node(const_cast<map_pointer>(other.node)) {}

        // base() 函数
        pointer base() const noexcept
        {
            return cur;
        }

        // 获取内部状态的函数
        pointer get_cur() const noexcept { return cur; }
        pointer get_first() const noexcept { return first; }
        pointer get_last() const noexcept { return last; }
        map_pointer get_node() const noexcept { return node; }
        deque_iterator get_iterator() const noexcept{ return *this; }

        // 重载操作符
        reference operator*() const noexcept { return *cur; }
        pointer operator->() const noexcept { return cur; }

        difference_type operator-(const deque_iterator& x) const noexcept
        {
            return difference_type(buffer_size()) * (node - x.node - 1) + (cur - first) + (x.last - x.cur);
        }

        deque_iterator& operator++() noexcept
        {
            ++cur;
            if (cur == last)
            {
                set_node(node + 1);
                cur = first;
            }
            return *this;
        }

        deque_iterator operator++(int) noexcept
        {
            deque_iterator tmp = *this;
            ++*this;
            return tmp;
        }

        deque_iterator& operator--() noexcept
        {
            if (cur == first)
            {
                set_node(node - 1);
                cur = last;
            }
            --cur;
            return *this;
        }

        deque_iterator operator--(int) noexcept
        {
            deque_iterator tmp = *this;
            --*this;
            return tmp;
        }

        deque_iterator& operator+=(difference_type n) noexcept
        {
            difference_type offset = n + (cur - first);
            if (offset >= 0 && offset < difference_type(buffer_size()))
            {
                // 目标位置在同一缓冲区内
                cur += n;
            }
            else
            {
                difference_type node_offset = offset > 0 ? 
                    offset / difference_type(buffer_size()) : -difference_type((-offset - 1) / buffer_size()) - 1;
                set_node(node + node_offset);
                cur = first + (offset - node_offset * difference_type(buffer_size()));
            }
            return *this;
        }

        deque_iterator operator+(difference_type n) const noexcept
        {
            deque_iterator tmp = *this;
            return tmp += n;
        }

        deque_iterator& operator-=(difference_type n) noexcept
        {
            return *this += -n;
        }

        deque_iterator operator-(difference_type n) const noexcept
        {
            deque_iterator tmp = *this;
            return tmp -= n;
        }

        reference operator[](difference_type n) const noexcept
        {
            return *(*this + n);
        }

        // 比较操作符
        bool operator==(const deque_iterator& rhs) const noexcept
        {
            return cur == rhs.cur;
        }

        bool operator!=(const deque_iterator& rhs) const noexcept
        {
            return !(*this == rhs);
        }

        bool operator<(const deque_iterator& rhs) const noexcept
        {
            return node == rhs.node ? cur < rhs.cur : node < rhs.node;
        }

        bool operator>(const deque_iterator& rhs) const noexcept
        {
            return rhs < *this;
        }

        bool operator<=(const deque_iterator& rhs) const noexcept
        {
            return !(rhs < *this);
        }

        bool operator>=(const deque_iterator& rhs) const noexcept
        {
            return !(*this < rhs);
        }

    private:
        // map_(控制中心)
        //   │
        //   ▼
        // [ptr1]->[ptr2]->[ptr3]->[ptr4]->[ptr5]  // node 指向 ptr3
        //          │
        //          ▼
        //     [    buffer   ]
        //      ▲    ▲    ▲
        //    first  cur  last
        // 迭代器数据成员
        pointer cur;           // 指向当前元素
        pointer first;         // 指向当前缓冲区的头
        pointer last;          // 指向当前缓冲区的尾
        map_pointer node;      // 指向控制中心

        // 私有辅助函数
        static size_type buffer_size() noexcept
        {
            return deque_buf_size(sizeof(value_type));
        }

        void set_node(map_pointer new_node) noexcept
        {
            node = new_node;
            first = *new_node;
            last = first + buffer_size();
        }
    };



    /*****************************************************************************************/
    // deque 的实现
    /*****************************************************************************************/ 
    template <class T, class Allocator = mystl::allocator<T>>
    class deque 
    {
    public:
        // 类型定义
        using value_type = typename Allocator::value_type;  // 使用分配器定义的类型
        using allocator_type = Allocator;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using reference = typename allocator_type::reference;
        using const_reference = typename allocator_type::const_reference;
        using pointer = typename allocator_type::pointer;
        using const_pointer = typename allocator_type::const_pointer;
        
        using iterator = deque_iterator<value_type>;
        using const_iterator = deque_iterator<const value_type>;
        using reverse_iterator = mystl::reverse_iterator<iterator>;
        using const_reverse_iterator = mystl::reverse_iterator<const_iterator>;  
        
        
    private:
        using map_allocator = typename Allocator::template rebind<pointer>::other;
        using map_pointer = pointer*;  // 显式指定为指针的指针类型
        
        iterator start_;           // 指向第一个元素
        iterator finish_;          // 指向最后一个元素的下一个位置
        map_pointer map_;         // 控制中心，存储缓冲区的地址
        size_type map_size_;      // 控制中心大小
        
        // 分配器
        allocator_type alloc_;    // 元素分配器
        map_allocator map_alloc_; // map分配器


    public:
        //------------------------------------------------------------------------------    
        // 构造函数
        //------------------------------------------------------------------------------    
        deque() : map_(nullptr), map_size_(0) 
        {
            create_map_and_nodes(0);
        }

        explicit deque(size_type count) 
        {
            create_map_and_nodes(count);
            try 
            {
                mystl::uninitialized_fill(start_, finish_, value_type());
            }
            catch(...) 
            {
                // 清理已分配的资源
                for (map_pointer cur = start_.node; cur <= finish_.node; ++cur)
                {
                    if (*cur != nullptr)
                        alloc_.deallocate(*cur, buffer_size());
                }
                map_alloc_.deallocate(map_, map_size_);
                throw;
            }
        }

        deque(size_type count, const T& value) 
        {
            create_map_and_nodes(count);
            try 
            {
                mystl::uninitialized_fill(start_, finish_, value);
            }
            catch(...) 
            {
                // 清理已分配的资源
                for (map_pointer cur = start_.node; cur <= finish_.node; ++cur)
                {
                    alloc_.deallocate(*cur, buffer_size());
                }
                map_alloc_.deallocate(map_, map_size_);
                throw;
            }
        }

        // 迭代器范围构造函数
        template <class InputIt>
        deque(InputIt first, InputIt last,
              typename enable_if<!is_integral<InputIt>::value>::type* = nullptr)
        {
            create_map_and_nodes(mystl::distance(first, last));
            try 
            {
                mystl::uninitialized_copy(first, last, start_);
            }
            catch(...) 
            {
                // 清理已分配的资源
                for (map_pointer cur = start_.node; cur <= finish_.node; ++cur)
                {
                    alloc_.deallocate(*cur, buffer_size());
                }
                map_alloc_.deallocate(map_, map_size_);
                throw;
            }
        }

        // 添加初始化列表构造函数
        deque(std::initializer_list<T> init)
        {
            create_map_and_nodes(init.size());
            try 
            {
                mystl::uninitialized_copy(init.begin(), init.end(), start_);
            }
            catch(...) 
            {
                // 清理已分配的资源
                for (map_pointer cur = start_.node; cur <= finish_.node; ++cur)
                {
                    alloc_.deallocate(*cur, buffer_size());
                }
                map_alloc_.deallocate(map_, map_size_);
                throw;
            }
        }

        deque(const deque& other)
        {
            create_map_and_nodes(other.size());
            try 
            {
                mystl::uninitialized_copy(other.begin(), other.end(), start_);
            }
            catch(...) 
            {
                // 清理已分配的资源
                for (map_pointer cur = start_.node; cur <= finish_.node; ++cur)
                {
                    alloc_.deallocate(*cur, buffer_size());
                }
                map_alloc_.deallocate(map_, map_size_);
                throw;
            }
        }

        deque(deque&& other) noexcept
            : start_(other.start_),
              finish_(other.finish_),
              map_(other.map_),
              map_size_(other.map_size_)
        {
            other.start_ = iterator();
            other.finish_ = iterator();
            other.map_ = nullptr;
            other.map_size_ = 0;
        }

        ~deque() 
        {
            clear();
            map_alloc_.deallocate(map_, map_size_);
        }   



        //------------------------------------------------------------------------------        
        // 迭代器相关操作
        //------------------------------------------------------------------------------    
        iterator begin() noexcept { return start_; }
        const_iterator begin() const noexcept { return const_iterator(start_); }
        const_iterator cbegin() const noexcept { return const_iterator(start_); }
        iterator end() noexcept { return finish_; }
        const_iterator end() const noexcept { return const_iterator(finish_); }
        const_iterator cend() const noexcept { return const_iterator(finish_); }
        
        // 反向迭代器
        reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
        const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
        const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }
        reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
        const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
        const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); } 



        //------------------------------------------------------------------------------            
        // 元素访问
        //------------------------------------------------------------------------------     
        reference operator[](size_type pos) { return start_[pos]; }
        const_reference operator[](size_type pos) const { return start_[pos]; }   

        // 带边界检查的访问
        reference at(size_type pos)
        {
            if (pos >= size())
            {
                throw out_of_range("deque::at");
            }
            return start_[pos];
        }       

        const_reference at(size_type pos) const
        {
            if (pos >= size())
            {
                throw out_of_range("deque::at");
            }
            return start_[pos];
        }

        reference front() 
        {
            EXPECT(!empty(), "deque is empty");
            return *start_;
        }

        const_reference front() const 
        {
            EXPECT(!empty(), "deque is empty");
            return *start_;
        }
        
        reference back() 
        {
            EXPECT(!empty(), "deque is empty");
            iterator tmp = finish_;
            --tmp;
            return *tmp;
        }

        const_reference back() const 
        {
            EXPECT(!empty(), "deque is empty");
            const_iterator tmp = finish_;
            --tmp;
            return *tmp;
        }


        
        //------------------------------------------------------------------------------            
        // 容量相关
        //------------------------------------------------------------------------------        
        bool empty() const { return start_ == finish_; }
        size_type size() const { return finish_ - start_; }
        size_type max_size() const noexcept
        {
            // 考虑三个限制因素：
            // 1. 分配器的限制
            size_type alloc_max = mystl::allocator<T>().max_size();
            
            // 2. difference_type的限制（通常是ptrdiff_t）
            size_type diff_max = std::numeric_limits<difference_type>::max();
            
            // 3. 缓冲区大小的限制
            size_type buf_size = buffer_size();
            size_type map_max = std::numeric_limits<difference_type>::max() / buf_size;
            
            // 返回三者中的最小值
            return mystl::min(alloc_max, min(diff_max, map_max * buf_size));
        }
        
        // 调整容器大小
        void resize(size_type count, const value_type& value) 
        {
            if (count < size()) 
            {
                erase(begin() + count, end());
            }
            else 
            {
                insert(end(), count - size(), value);
            }
        }

        void resize(size_type count) 
        {
            resize(count, value_type());
        }

        // 收缩容量 
        void shrink_to_fit() 
        {
            // 计算需要的最小缓冲区数
            size_type need_buffer = size() / buffer_size() + 1;
            
            // 如果当前缓冲区数大于需要的数量，则进行收缩
            if (finish_.node - start_.node + 1 > need_buffer)
            {
                // 创建新的map和缓冲区
                size_type new_map_size = need_buffer + 2;  // 多分配2个用于扩展
                map_pointer new_map = map_alloc_.allocate(new_map_size);
                map_pointer new_nstart = new_map + 1;
                
                try 
                {
                    // 分配新的缓冲区并复制数据
                    for (size_type i = 0; i < need_buffer; ++i)
                    {
                        new_map[i + 1] = alloc_.allocate(buffer_size());
                    }
                    
                    // 复制元素到新缓冲区
                    iterator new_start(new_map[1], new_map + 1);
                    try 
                    {
                        mystl::uninitialized_copy(begin(), end(), new_start);
                    }
                    catch (...) 
                    {
                        // 如果复制失败，释放新分配的缓冲区
                        for (size_type i = 0; i < need_buffer; ++i)
                        {
                            alloc_.deallocate(new_map[i + 1], buffer_size());
                        }
                        map_alloc_.deallocate(new_map, new_map_size);
                        throw;
                    }
                    
                    // 销毁原有对象并释放内存
                    clear();  // 先销毁原有对象
                    map_alloc_.deallocate(map_, map_size_);  // 再释放map
                    
                    // 更新成员变量
                    map_ = new_map;
                    map_size_ = new_map_size;
                    start_ = iterator(new_map[1], new_map + 1);
                    finish_ = start_ + size();
                }
                catch (...) 
                {
                    map_alloc_.deallocate(new_map, new_map_size);
                    throw;
                }
            }
        }


        //------------------------------------------------------------------------------        
        // 赋值操作
        //------------------------------------------------------------------------------    
        deque& operator=(const deque& other) 
        {
            if (this != &other) 
            {
                assign(other.begin(), other.end());
            }
            return *this;
        }

        deque& operator=(deque&& other) noexcept 
        {
            clear();
            swap(other);
            return *this;
        }

        void assign(size_type count, const T& value) 
        {
            clear();
            if (count > 0) 
            {
                insert(begin(), count, value);
            }
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
            insert(begin(), ilist.begin(), ilist.end());
        }
        
        
        //------------------------------------------------------------------------------        
        // 修改器
        //------------------------------------------------------------------------------    
        // 清空操作
        void clear() 
        {
            if (empty()) return;  // 处理空 deque

            // 清空所有中间缓冲区
            for (map_pointer node = start_.node + 1; node < finish_.node; ++node) 
            {
                mystl::destroy(*node, *node + buffer_size());  // 直接使用 destroy
                alloc_.deallocate(*node, buffer_size());
            }

            if (start_.node != finish_.node) 
            {
                // 有多个缓冲区时
                mystl::destroy(start_.cur, start_.last);
                mystl::destroy(finish_.first, finish_.cur);
                alloc_.deallocate(*finish_.node, buffer_size());
            }
            else 
            {
                // 只有一个缓冲区时
                mystl::destroy(start_.cur, finish_.cur);
            }

            finish_ = start_;  // 重置迭代器
        }
        

        

        // 在pos位置插入元素
        template <class V>
        iterator insert(const_iterator pos, V&& value)
        {
            // 在头部插入
            if (pos.cur == start_.cur)
            {
                push_front_aux(mystl::forward<V>(value));
                return start_;
            }
            // 在尾部插入
            else if (pos.cur == finish_.cur)
            {
                push_back_aux(mystl::forward<V>(value));
                iterator tmp = finish_;
                --tmp;
                return tmp;
            }
            // 在中间插入
            return insert_aux(pos, mystl::forward<V>(value));
        }



        // 在pos位置插入n个元素
        iterator insert(iterator pos, size_type n, const T& value)
        {
            if (n == 0) return pos;
            
            const size_type elems_before = pos - start_;
            if (elems_before < size() / 2)
            {
                // 在前半段插入
                iterator new_start = reserve_elements_at_front(n);
                iterator old_start = start_;
                
                try 
                {
                    // 先复制原有元素
                    mystl::uninitialized_copy(old_start, pos, new_start);
                    try 
                    {
                        // 再填充新元素
                        mystl::fill_n(new_start + elems_before, n, value);
                    }
                    catch (...) 
                    {
                        // 如果填充失败：
                        // 把原始数据写回原来的位置
                        mystl::copy(new_start, new_start + elems_before, old_start);
                        // 销毁新位置的数据
                        mystl::destroy(new_start, new_start + elems_before);
                        // 释放新分配的内存
                        destroy_nodes_at_front(new_start);
                        throw;
                    }
                }
                catch (...) 
                {
                    destroy_nodes_at_front(new_start);
                    throw;
                }
                start_ = new_start;  // 所有操作都成功后才更新start_
            }
            else 
            {
                // 在后半段插入
                iterator new_finish = reserve_elements_at_back(n);
                iterator old_finish = finish_;
                
                try 
                {
                    // 先移动后面的元素
                    mystl::uninitialized_copy(pos, old_finish, finish_ + n);
                    try 
                    {
                        // 再填充新元素
                        mystl::fill_n(pos, n, value);
                    }
                    catch (...) 
                    {
                        // 如果填充失败：
                        // 把原始数据写回原来的位置
                        mystl::copy(finish_ + n, finish_ + n + (old_finish - pos), iterator(pos));
                        // 销毁新位置的数据
                        mystl::destroy(finish_ + n, finish_ + n + (old_finish - pos));
                        // 释放新分配的内存
                        destroy_nodes_at_back(new_finish);
                        throw;
                    }
                }
                catch (...) 
                {
                    destroy_nodes_at_back(new_finish);
                    throw;
                }
                
                finish_ = finish_ + n;  // 所有操作都成功后才更新finish_
            }
            
            return start_ + elems_before;
        }

        // 在pos之前插入[first, last)范围内的元素
        template <class InputIt>
        iterator insert(iterator pos, InputIt first, InputIt last,
                        typename enable_if<!is_integral<InputIt>::value>::type* = nullptr)
        {
            if (first == last) return pos;
            
            const size_type n = mystl::distance(first, last);
            const size_type elems_before = pos - start_;
            
            if (elems_before < size() / 2)
            {
                // 在前半段插入
                iterator new_start = reserve_elements_at_front(n);
                iterator old_start = start_;
                
                try 
                {
                    // 先复制原有元素
                    mystl::uninitialized_copy(old_start, pos, new_start);
                    try 
                    {
                        // 再插入新元素
                        mystl::uninitialized_copy(first, last, new_start + elems_before);
                    }
                    catch (...) 
                    {
                        // 把原始数据写回原来的位置
                        mystl::copy(new_start, new_start + elems_before, old_start);
                        // 销毁新位置的数据
                        mystl::destroy(new_start, new_start + elems_before);
                        // 释放新分配的内存
                        destroy_nodes_at_front(new_start);
                        throw;
                    }
                }
                catch (...) 
                {
                    destroy_nodes_at_front(new_start);
                    throw;
                }
                
                start_ = new_start;  // 所有操作都成功后才更新start_
            }
            else 
            {
                // 在后半段插入
                iterator new_finish = reserve_elements_at_back(n);
                iterator old_finish = finish_;
                
                try 
                {
                    // 先移动后面的元素
                    mystl::uninitialized_copy(pos, old_finish, finish_ + n);
                    try 
                    {
                        // 再插入新元素
                        mystl::uninitialized_copy(first, last, pos);
                    }
                    catch (...) 
                    {
                        // 把原始数据写回原来的位置
                        mystl::copy(finish_ + n, finish_ + n + (old_finish - pos), pos);
                        // 销毁新位置的数据
                        mystl::destroy(finish_ + n, finish_ + n + (old_finish - pos));
                        // 释放新分配的内存
                        destroy_nodes_at_back(new_finish);
                        throw;
                    }
                }
                catch (...) 
                {
                    destroy_nodes_at_back(new_finish);
                    throw;
                }
                
                finish_ = finish_ + n;  // 所有操作都成功后才更新finish_
            }
            
            return start_ + elems_before;
        }

        // 在pos之前插入初始化列表中的元素
        iterator insert(iterator pos, std::initializer_list<T> ilist)
        {
            return insert(pos, ilist.begin(), ilist.end());
        }


        // 在指定位置就地构造元素
        template <class... Args>
        iterator emplace(iterator pos, Args&&... args)
        {
            const size_type elems_before = pos - start_;
            if (elems_before < size() / 2)
            {
                // 在前半段插入
                iterator new_start = reserve_elements_at_front(1);
                iterator old_start = start_;
                
                try 
                {
                    // 先复制原有元素
                    mystl::uninitialized_copy(old_start, pos, new_start);
                    try 
                    {
                        // 直接在位置构造新元素
                        mystl::construct(new_start.cur + elems_before, mystl::forward<Args>(args)...);
                    }
                    catch (...) 
                    {
                        // 把原始数据写回原来的位置
                        mystl::copy(new_start, new_start + elems_before, old_start);
                        // 销毁新位置的数据
                        mystl::destroy(new_start, new_start + elems_before);
                        // 释放新分配的内存
                        destroy_nodes_at_front(new_start);
                        throw;
                    }
                }
                catch (...) 
                {
                    destroy_nodes_at_front(new_start);
                    throw;
                }
                
                start_ = new_start;  // 所有操作都成功后才更新start_
            }
            else 
            {
                // 在后半段插入
                iterator new_finish = reserve_elements_at_back(1);
                iterator old_finish = finish_;
                
                try 
                {
                    // 先移动后面的元素
                    mystl::uninitialized_copy(pos, old_finish, finish_ + 1);
                    try 
                    {
                        // 直接在位置构造新元素
                        mystl::construct(pos.cur, mystl::forward<Args>(args)...);
                    }
                    catch (...) 
                    {
                        // 把原始数据写回原来的位置
                        mystl::copy(finish_ + 1, finish_ + 1 + (old_finish - pos), pos);
                        // 销毁新位置的数据
                        mystl::destroy(finish_ + 1, finish_ + 1 + (old_finish - pos));
                        // 释放新分配的内存
                        destroy_nodes_at_back(new_finish);
                        throw;
                    }
                }
                catch (...) 
                {
                    destroy_nodes_at_back(new_finish);
                    throw;
                }
                
                finish_ = finish_ + 1;  // 所有操作都成功后才更新finish_
            }
            
            return start_ + elems_before;
        }

        // 在尾部就地构造元素
        template <class... Args>
        iterator emplace_back(Args&&... args)
        {
            if (empty())
                return emplace(begin(), mystl::forward<Args>(args)...);
            else
                return emplace(end(), mystl::forward<Args>(args)...);
        }


        // 在头部就地构造元素
        template <class... Args>
        iterator emplace_front(Args&&... args)
        {
            return emplace(begin(), mystl::forward<Args>(args)...);
        }

        // 在头部添加元素
        template <class V>
        void push_front(V&& value)
        {
            push_front_aux(mystl::forward<V>(value));
        }

        // 在尾部添加元素
        template <class V>
        void push_back(V&& value)
        {
            push_back_aux(mystl::forward<V>(value));
        }

        // 删除尾部元素
        void pop_back() 
        {
            if (empty()) return;
            
            // 销毁最后一个元素
            mystl::destroy_at(finish_.cur-1);
            
            // 如果当前缓冲区为空，需要释放它
            if (finish_.cur == finish_.first)
            {
                // 释放缓冲区
                alloc_.deallocate(finish_.first, buffer_size());
                // 移动到前一个缓冲区
                finish_.set_node(finish_.node - 1);
                finish_.cur = finish_.last;
            }
            
            // 这里需要先判断是否为空，再--finish_.cur
            --finish_.cur;
        }

        // 删除头部元素
        void pop_front() 
        {
            if (empty()) return;
            
            mystl::destroy_at(start_.cur);
            //这里需要先++start_.cur，再判断是否为空
            ++start_.cur;
            
            // 如果当前缓冲区为空，需要释放
            if (start_.cur == start_.last) 
            {
                alloc_.deallocate(start_.first, buffer_size());
                start_.set_node(start_.node + 1);
                start_.cur = start_.first;
            } 
        }
        

        // 删除pos位置的元素
        iterator erase(iterator pos)
        {
            iterator next = pos;
            ++next;
            
            difference_type elems_after = end() - next;
            if (elems_after > 0)
            {
                // 如果不是最后一个元素，需要逐个移动后面的元素
                for (iterator curr = pos; curr != end() - 1; ++curr)
                {
                    *curr = mystl::move(*(curr + 1));
                }
            }
            
            // 销毁最后一个元素
            mystl::destroy_at(finish_.cur - 1);
            --finish_;
            
            return next == end() ? end() : pos;  // 如果删除的是最后一个元素，返回end()，否则返回pos
        }

        // 删除[first, last)范围内的元素
        iterator erase(iterator first, iterator last)
        {
            if (first == last)
                return first;

            // 计算要移动的元素个数和要删除的元素个数
            difference_type elems_after = end() - last;
            difference_type n = last - first;
            iterator new_finish = first + elems_after;
            
            if (elems_after > 0)
            {
                // 如果last后还有元素，需要逐个移动它们
                for (iterator curr = first; curr != first + elems_after; ++curr)
                {
                    *curr = mystl::move(*(curr + n));
                }
            }
            
            // 销毁多余元素
            for (iterator it = new_finish; it != end(); ++it)
            {
                mystl::destroy_at(it.cur);
            }
            
            // 更新finish_位置
            finish_ = new_finish;
            
            return first;  // 返回被删除范围的起始位置
        }

        
        // 添加交换操作
        void swap(deque& other) noexcept 
        {
            mystl::swap(start_, other.start_);
            mystl::swap(finish_, other.finish_);
            mystl::swap(map_, other.map_);
            mystl::swap(map_size_, other.map_size_);
        }

        

    private:
        //------------------------------------------------------------------------------        
        // 辅助函数
        //------------------------------------------------------------------------------   

        // 获取缓冲区大小
        static size_type buffer_size() noexcept
        {
            return deque_buf_size(sizeof(value_type));
        }
        
         
        // 创建map和节点
        void create_map_and_nodes(size_type num_elements) 
        {
            // 需要的节点数 = (元素个数 / 每个缓冲区可容纳的元素个数) + 1
            size_type num_nodes = num_elements / buffer_size() + 1;
            
            // map至少管理8个节点，最多是所需节点数加2
            map_size_ = mystl::max(size_type(8), num_nodes + 2);
            map_ = map_alloc_.allocate(map_size_);
            
            // 让 nstart 和 nfinish 都指向 map_ 的中间位置
            map_pointer nstart = map_ + (map_size_ - num_nodes) / 2;
            map_pointer nfinish = nstart + num_nodes - 1;
            
            try 
            {
                // 为每个节点配置缓冲区
                for (map_pointer cur = nstart; cur <= nfinish; ++cur)
                    *cur = alloc_.allocate(buffer_size());
            }
            catch(...) 
            {
                // 若配置失败，释放已配置的空间
                for (map_pointer cur = nstart; cur < nfinish; ++cur)
                    alloc_.deallocate(*cur, buffer_size());
                map_alloc_.deallocate(map_, map_size_);
                throw;
            }
            
            // 设置迭代器
            start_.set_node(nstart);
            finish_.set_node(nfinish);
            start_.cur = start_.first;
            finish_.cur = start_.first + num_elements;  // 修改这里：直接设置为起始位置加上元素个数
        }


        // 在前端预留n个元素的空间
        iterator reserve_elements_at_front(size_type n)
        {
            size_type vacancies = start_.cur - start_.first;
            if (n > vacancies)
            {
                size_type new_nodes = (n - vacancies) / buffer_size() + 1;
                
                // 保存原始状态
                map_pointer old_start_node = start_.node;
                
                try
                {
                    // 先确保 map 有足够空间
                    reserve_map_at_front(new_nodes);
                    
                    // 分配新的缓冲区
                    size_type i;
                    try
                    {
                        for (i = 1; i <= new_nodes; ++i)
                            *(start_.node - i) = alloc_.allocate(buffer_size());
                    }
                    catch (...)
                    {
                        // 清理已分配的缓冲区
                        for (size_type j = 1; j < i; ++j)
                            alloc_.deallocate(*(start_.node - j), buffer_size());
                        
                        // 恢复原始状态
                        start_.node = old_start_node;
                        throw;
                    }

                    iterator new_start;
                    new_start.set_node(start_.node - new_nodes);
                    new_start.cur = new_start.last - (n - vacancies);
                    return new_start;
                }
                catch (...)
                {
                    // 恢复原始状态
                    start_.node = old_start_node;
                    throw;
                }
            }
            else
                return iterator(start_.cur - n, start_.first, start_.last, start_.node);
        }

        // 在后端预留n个元素的空间
        iterator reserve_elements_at_back(size_type n)
        {
            size_type vacancies = (finish_.last - finish_.cur) - 1;  // 当前缓冲区后端的空闲空间
            if (n > vacancies)
            {
                size_type new_nodes = (n - vacancies) / buffer_size() + 1;
                
                // 保存原始状态
                map_pointer old_finish_node = finish_.node;
                
                try
                {
                    // 先确保 map 有足够空间
                    reserve_map_at_back(new_nodes);
                    
                    // 分配新的缓冲区
                    size_type i;
                    try
                    {
                        for (i = 1; i <= new_nodes; ++i)
                            *(finish_.node + i) = alloc_.allocate(buffer_size());
                    }
                    catch (...)
                    {
                        // 清理已分配的缓冲区
                        for (size_type j = 1; j < i; ++j)
                            alloc_.deallocate(*(finish_.node + j), buffer_size());
                        
                        // 恢复原始状态
                        finish_.node = old_finish_node;
                        throw;
                    }
                    
                    return iterator(finish_.cur + n, finish_.first, finish_.last, finish_.node);  // 返回新的结束位置
                }
                catch (...)
                {
                    // 恢复原始状态
                    finish_.node = old_finish_node;
                    throw;
                }
            }
            return iterator(finish_.cur + n, finish_.first, finish_.last, finish_.node);  // 当前缓冲区空间足够
        }

        // 确保map后端有足够空间
        void reserve_map_at_back(size_type nodes_to_add = 1) 
        {
            if (nodes_to_add + 1 > map_size_ - (finish_.node - map_))
                reallocate_map(nodes_to_add, false);
        }

        // 确保map前端有足够空间
        void reserve_map_at_front(size_type nodes_to_add = 1) 
        {
            if (nodes_to_add > size_type(start_.node - map_))
                reallocate_map(nodes_to_add, true);
        }

        // 重新分配map  
        void reallocate_map(size_type nodes_to_add, bool add_at_front) 
        {
            size_type old_num_nodes = finish_.node - start_.node + 1;
            size_type new_num_nodes = old_num_nodes + nodes_to_add;

            map_pointer new_nstart;
            if (map_size_ > 2 * new_num_nodes) 
            {
                new_nstart = map_ + (map_size_ - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
                
                // 保存原始状态
                map_pointer old_start = start_.node;
                map_pointer old_finish = finish_.node;
                
                try 
                {
                    if (new_nstart < start_.node)
                    {
                        mystl::copy(start_.node, finish_.node + 1, new_nstart);
                    }
                    else
                    {
                        mystl::copy_backward(start_.node, finish_.node + 1, new_nstart + old_num_nodes);
                    }
                    // 只有复制成功后才更新节点指针
                    start_.set_node(new_nstart);
                    finish_.set_node(new_nstart + old_num_nodes - 1);
                }
                catch (...) 
                {
                    // 恢复原始状态
                    start_.set_node(old_start);
                    finish_.set_node(old_finish);
                    throw;
                }
            }
            else 
            {
                // 需要分配新的map
                size_type new_map_size = map_size_ + mystl::max(map_size_, nodes_to_add) + 2;
                map_pointer new_map = map_alloc_.allocate(new_map_size);
                
                try
                {
                    new_nstart = new_map + (new_map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
                    mystl::copy(start_.node, finish_.node + 1, new_nstart);
                    
                    // 只有所有操作都成功后才释放旧的map并更新指针
                    map_pointer old_map = map_;
                    size_type old_map_size = map_size_;
                    
                    map_ = new_map;
                    map_size_ = new_map_size;
                    start_.set_node(new_nstart);
                    finish_.set_node(new_nstart + old_num_nodes - 1);
                    
                    map_alloc_.deallocate(old_map, old_map_size);
                }
                catch(...)
                {
                    map_alloc_.deallocate(new_map, new_map_size);
                    throw;
                }
            }
        }

        // 在前端释放节点
        void destroy_nodes_at_front(iterator before_start)
        {
            for (map_pointer n = before_start.node; n < start_.node; ++n)
            {
                if (*n != nullptr)  // 检查指针是否有效
                {
                    mystl::destroy(*n, *n + buffer_size());  // 先销毁对象
                    alloc_.deallocate(*n, buffer_size());    // 再释放内存
                    *n = nullptr;                            // 设置为空指针
                }
            }
        }

        // 在后端释放节点
        void destroy_nodes_at_back(iterator after_finish)
        {
            for (map_pointer n = after_finish.node; n > finish_.node; --n)
            {
                if (*n != nullptr)  // 检查指针是否有效
                {
                    mystl::destroy(*n, *n + buffer_size());  // 先销毁对象
                    alloc_.deallocate(*n, buffer_size());    // 再释放内存
                    *n = nullptr;                            // 设置为空指针
                }
            }
        }

        // 在pos位置插入元素的辅助函数
        template <class V>  // V可以是T&或T&&
        iterator insert_aux(iterator pos, V&& value)
        {
            const size_type elems_before = pos - start_;
            
            if (elems_before < size() / 2)
            {
                // 在前半段插入
                iterator new_start = reserve_elements_at_front(1);
                
                try 
                {
                    // 先构造新元素
                    mystl::construct(new_start.cur, mystl::forward<V>(value));
                    
                    try 
                    {
                        // 再复制原有元素
                        mystl::uninitialized_copy(start_, pos, new_start + 1);
                        start_ = new_start;  // 更新start_
                    }
                    catch (...) 
                    {
                        mystl::destroy_at(new_start.cur);
                        destroy_nodes_at_front(new_start);
                        throw;
                    }
                }
                catch (...) 
                {
                    destroy_nodes_at_front(new_start);
                    throw;
                }
            }
            else 
            {
                // 在后半段插入
                iterator new_finish = reserve_elements_at_back(1);
                
                try 
                {
                    // 先在新位置构造最后一个元素
                    mystl::construct(finish_.cur, back());
                    
                    try 
                    {
                        // 移动元素
                        mystl::uninitialized_move_backward(pos, finish_ - 1, finish_);
                        
                        // 在pos位置构造新元素
                        mystl::construct(start_ + elems_before, mystl::forward<V>(value));
                        
                        ++finish_;     // 更新finish_
                    }
                    catch (...) 
                    {
                        mystl::destroy_at(finish_.cur);
                        destroy_nodes_at_back(new_finish);
                        throw;
                    }
                }
                catch (...) 
                {
                    destroy_nodes_at_back(new_finish);
                    throw;
                }
            }
            return start_ + elems_before;
        }


        // 在头部添加元素
        template <class V>
        void push_front_aux(V&& value)
        {
            if (start_.cur != start_.first) 
            {
                // 缓冲区还有空间
                mystl::construct(start_.cur - 1, mystl::forward<V>(value));
                --start_.cur;
            }
            else 
            {
                // 需要新的缓冲区
                iterator new_start = reserve_elements_at_front(1);
                try 
                {
                    mystl::construct(new_start.cur, mystl::forward<V>(value));
                    start_ = new_start;
                }
                catch (...) 
                {
                    destroy_nodes_at_front(new_start);
                    throw;
                }
            }
        }

        // 在尾部添加元素
        template <class V>
        void push_back_aux(V&& value)
        {
            if (finish_.cur != finish_.last - 1) 
            {
                // 缓冲区还有空间
                mystl::construct(finish_.cur, mystl::forward<V>(value));
                ++finish_.cur;
            }
            else 
            {
                // 需要新的缓冲区
                iterator new_finish = reserve_elements_at_back(1);
                try 
                {
                    mystl::construct(finish_.cur, mystl::forward<V>(value));
                    ++finish_;
                }
                catch (...) 
                {
                    destroy_nodes_at_back(new_finish);
                    throw;
                }
            }
        }


    };

    

    //------------------------------------------------------------------------------        
    // 非成员函数
    //------------------------------------------------------------------------------        
    // 比较运算符
    template <class T, class Allocator>
    bool operator==(const deque<T, Allocator>& lhs, const deque<T, Allocator>& rhs)
    {
        return lhs.size() == rhs.size() && mystl::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template <class T, class Allocator>
    bool operator!=(const deque<T, Allocator>& lhs, const deque<T, Allocator>& rhs)
    {
        return !(lhs == rhs);
    }

    template <class T, class Allocator>
    bool operator<(const deque<T, Allocator>& lhs, const deque<T, Allocator>& rhs)
    {
        return mystl::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template <class T, class Allocator>
    bool operator<=(const deque<T, Allocator>& lhs, const deque<T, Allocator>& rhs)
    {
        return !(rhs < lhs);
    }

    template <class T, class Allocator>
    bool operator>(const deque<T, Allocator>& lhs, const deque<T, Allocator>& rhs)
    {
        return rhs < lhs;
    }

    template <class T, class Allocator>
    bool operator>=(const deque<T, Allocator>& lhs, const deque<T, Allocator>& rhs)
    {
        return !(lhs < rhs);
    }

    // 交换两个deque
    template <class T, class Allocator>
    void swap(deque<T, Allocator>& lhs, deque<T, Allocator>& rhs) noexcept
    {
        lhs.swap(rhs);
    }

} // namespace mystl 

