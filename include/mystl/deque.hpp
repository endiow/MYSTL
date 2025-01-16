#pragma once

#include <initializer_list>
#include <limits>   // 包含numeric_limits

#include "allocator.hpp"
#include "iterator.hpp"
#include "uninitialized.hpp"    
#include "algorithm.hpp"
#include "construct.hpp"

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

        template <class T, class Allocator>
        friend class deque;  // 保留 deque 作为友元

    public:
        // 迭代器类型定义
        using iterator_category = mystl::random_access_iterator_tag;
        using value_type = T;
        using pointer = typename mystl::allocator<T>::pointer;
        using reference = typename mystl::allocator<T>::reference;
        using difference_type = ptrdiff_t;
        using size_type = size_t;
        // 使用分配器的 rebind 获取指针的指针类型
        using map_pointer = typename mystl::allocator<T>::template rebind<pointer>::other::pointer;

        // 构造函数
        deque_iterator() noexcept
            : cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {}

        deque_iterator(pointer c, map_pointer n) noexcept
            : cur(c), first(*n), last(*n + buffer_size()), node(n) {}

        deque_iterator(const deque_iterator& rhs) noexcept
            : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {}

        deque_iterator(deque_iterator&& rhs) noexcept
            : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node)
        {
            rhs.cur = nullptr;
            rhs.first = nullptr;
            rhs.last = nullptr;
            rhs.node = nullptr;
        }

        // 赋值运算符
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

        deque_iterator& operator=(deque_iterator&& rhs) noexcept
        {
            if (this != &rhs)
            {
                cur = rhs.cur;
                first = rhs.first;
                last = rhs.last;
                node = rhs.node;
                rhs.cur = nullptr;
                rhs.first = nullptr;
                rhs.last = nullptr;
                rhs.node = nullptr;
            }
            return *this;
        }

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

        // 获取原生指针
        pointer base() const noexcept { return cur; }

        // 从非const迭代器到const迭代器的隐式转换
        operator deque_iterator<const T>() const noexcept
        { 
            return deque_iterator<const T>(this->cur);
        }

        // 允许从其他类型迭代器构造的模板构造函数
        template <class U>
        deque_iterator(const deque_iterator<U>& other) noexcept
            requires(std::is_convertible_v<U*, T*>)
        {
            // 使用友元类的访问方法
            cur = const_cast<pointer>(other.cur);
            first = const_cast<pointer>(other.first);
            last = const_cast<pointer>(other.last);
            node = const_cast<map_pointer>(other.node);
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
        map_pointer node;     // 指向控制中心

        // 私有辅助函数
        static size_type buffer_size() noexcept
        {
            return deque_buf_size(sizeof(T));
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
        using value_type = T;
        using allocator_type = Allocator;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using reference = typename allocator_type::reference;
        using const_reference = typename allocator_type::const_reference;
        using pointer = typename allocator_type::pointer;
        using const_pointer = typename allocator_type::const_pointer;
        
        using iterator = deque_iterator<T>;
        using const_iterator = deque_iterator<const T>;
        using reverse_iterator = mystl::reverse_iterator<iterator>;
        using const_reverse_iterator = mystl::reverse_iterator<const_iterator>;  
        
        
    private:
        using map_allocator = typename Allocator::template rebind<T*>::other;
        using map_pointer = typename map_allocator::pointer;
        
        iterator start_;           // 指向第一个元素
        iterator finish_;          // 指向最后一个元素的下一个位置
        T** map_;                  // 控制中心，存储缓冲区的地址
        size_type map_size_;       // 控制中心大小
        
        // 分配器
        allocator_type alloc_;     // 元素分配器
        map_allocator map_alloc_;  // map分配器


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
                for (map_pointer cur = start_.node; cur <= finish_.node; ++cur)
                {
                    data_allocator::deallocate(*cur, buffer_size());
                }
                map_allocator::deallocate(map_, map_size_);
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
                // 清理已分配的内存
                for (map_pointer cur = start_.node; cur <= finish_.node; ++cur)
                {
                    data_allocator::deallocate(*cur, buffer_size());
                }
                map_allocator::deallocate(map_, map_size_);
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
                // 如果构造失败，释放已分配的空间
                for (map_pointer cur = start_.node; cur <= finish_.node; ++cur)
                {
                    data_allocator::deallocate(*cur, buffer_size());
                }
                map_allocator::deallocate(map_, map_size_);
                throw;
            }
        }

        // 添加初始化列表构造函数
        deque(std::initializer_list<T> ilist)
        {
            create_map_and_nodes(ilist.size());
            try 
            {
                mystl::uninitialized_copy(ilist.begin(), ilist.end(), start_);
            }
            catch(...) 
            {
                for (map_pointer cur = start_.node; cur <= finish_.node; ++cur)
                {
                    data_allocator::deallocate(*cur, buffer_size());
                }
                map_allocator::deallocate(map_, map_size_);
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
                for (map_pointer cur = start_.node; cur <= finish_.node; ++cur)
                {
                    data_allocator::deallocate(*cur, buffer_size());
                }
                map_allocator::deallocate(map_, map_size_);
                throw;
            }
        }

        deque(deque&& other) noexcept
            : start_(other.start_), finish_(other.finish_),
              map_(other.map_), map_size_(other.map_size_)
        {
            other.start_ = iterator();
            other.finish_ = iterator();
            other.map_ = nullptr;
            other.map_size_ = 0;
        }

        ~deque() 
        {
            clear();
            map_allocator::deallocate(map_, map_size_);
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
            if (pos >= size_)
            {
                throw out_of_range("deque::at");
            }
            return start_[pos];
        }       

        const_reference at(size_type pos) const
        {
            if (pos >= size_)
            {
                throw out_of_range("deque::at");
            }
            return start_[pos];
        }

        reference front() { return *start_; }
        const_reference front() const { return *start_; }
        
        reference back() { return *(finish_ - 1); }   
        const_reference back() const { return *(finish_ - 1); }


        
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
            return mystl::min({alloc_max, diff_max, map_max * buf_size});
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
                map_pointer new_map = map_allocator::allocate(new_map_size);
                map_pointer new_nstart = new_map + 1;
                
                try 
                {
                    // 分配新的缓冲区并复制数据
                    for (size_type i = 0; i < need_buffer; ++i)
                    {
                        new_map[i + 1] = data_allocator::allocate(buffer_size());
                    }
                    
                    // 复制元素到新缓冲区
                    iterator new_start(new_map[1], new_map + 1);
                    mystl::uninitialized_copy(begin(), end(), new_start);
                    
                    // 释放原有内存
                    for (map_pointer cur = start_.node; cur <= finish_.node; ++cur)
                    {
                        data_allocator::deallocate(*cur, buffer_size());
                    }
                    map_allocator::deallocate(map_, map_size_);
                    
                    // 更新成员变量
                    map_ = new_map;
                    map_size_ = new_map_size;
                    start_ = iterator(new_map[1], new_map + 1);
                    finish_ = start_ + size();
                }
                catch (...)
                {
                    // 如果发生异常，释放已分配的新内存
                    for (size_type i = 0; i < need_buffer; ++i)
                    {
                        if (new_map[i + 1])
                        {
                            data_allocator::deallocate(new_map[i + 1], buffer_size());
                        }
                    }
                    map_allocator::deallocate(new_map, new_map_size);
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
            // 清空所有缓冲区
            for (map_pointer node = start_.node + 1; node < finish_.node; ++node) 
            {
                destroy_range(*node, *node + buffer_size());
                data_allocator::deallocate(*node, buffer_size());
            }

            if (start_.node != finish_.node) 
            {
                destroy_range(start_.cur, start_.last);
                destroy_range(finish_.first, finish_.cur);
                data_allocator::deallocate(*finish_.node, buffer_size());
            }
            else 
            {
                destroy_range(start_.cur, finish_.cur);
            }

            finish_ = start_;
        }
        

        // 在pos之前插入一个元素的通用实现
        template <class V>
        iterator insert_impl(const_iterator pos, V&& value)  // 使用万能引用
        {
            if (pos.cur == start_.cur) 
            {
                push_front(mystl::forward<V>(value));  // 完美转发
                return start_;
            }
            else if (pos.cur == finish_.cur) 
            {
                push_back(mystl::forward<V>(value));
                return finish_ - 1;
            }
            else 
            {
                return insert_aux(pos, mystl::forward<V>(value));
            }
        }

        // 在pos之前插入一个元素(拷贝版本)
        iterator insert(const_iterator pos, const T& value) 
        {
            return insert_impl(pos, value);
        }

        // 在pos之前插入一个元素(移动版本)
        iterator insert(const_iterator pos, T&& value)
        {
            return insert_impl(pos, mystl::move(value));
        }

        // 在pos之前插入n个元素
        iterator insert(const_iterator pos, size_type n, const T& value)
        {
            if (n == 0) return iterator(reinterpret_cast<node_type*>(pos.node));

            const size_type elems_before = pos - start_;
            if (elems_before < size() / 2) 
            {
                // 如果插入点之前的元素较少，在前端插入
                iterator new_start = reserve_elements_at_front(n);
                iterator old_start = start_;
                
                try 
                {
                    // 先在新空间中构造所有元素
                    mystl::uninitialized_fill(new_start, old_start, value);
                    
                    try 
                    {
                        mystl::uninitialized_fill(old_start, pos, value);
                        // 所有操作成功后才更新start_
                        start_ = new_start;
                    }
                    catch (...) 
                    {
                        // 清理第二次构造的元素
                        mystl::destroy(new_start, old_start);
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
                // 如果插入点之后的元素较少，在后端插入
                iterator new_finish = reserve_elements_at_back(n);
                iterator old_finish = finish_;
                
                try 
                {
                    const size_type elems_after = size() - elems_before;
                    if (elems_after > n) 
                    {
                        // 先复制到新空间
                        mystl::uninitialized_copy(finish_ - n, finish_, finish_);
                        
                        try 
                        {
                            mystl::copy_backward(pos, old_finish - n, old_finish);
                            mystl::fill(pos, pos + n, value);
                            // 所有操作成功后才更新finish_
                            finish_ = new_finish;
                        }
                        catch (...) 
                        {
                            // 清理已复制的元素
                            mystl::destroy(finish_, finish_ + n);
                            destroy_nodes_at_back(new_finish);
                            throw;
                        }
                    }
                    else 
                    {
                        // 先在新空间中构造元素
                        mystl::uninitialized_fill(finish_, pos + n, value);
                        
                        try 
                        {
                            mystl::uninitialized_copy(pos, finish_, pos + n);
                            mystl::fill(pos, old_finish, value);
                            // 所有操作成功后才更新finish_
                            finish_ = new_finish;
                        }
                        catch (...) 
                        {
                            // 清理已构造的元素
                            mystl::destroy_n(finish_, pos + n);
                            destroy_nodes_at_back(new_finish);
                            throw;
                        }
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

        // 在pos之前插入[first, last)范围内的元素
        template <class InputIt>
        iterator insert(const_iterator pos, InputIt first, InputIt last,
                        typename enable_if<!is_integral<InputIt>::value>::type* = nullptr)
        {
            difference_type n = mystl::distance(first, last);
            if (n == 0) return iterator(pos);

            const size_type elems_before = pos - start_;
            
            if (elems_before < size() / 2)
            {
                // 在前半段插入
                iterator new_start = reserve_elements_at_front(n);
                iterator old_start = start_;
                
                try 
                {
                    // 先复制新元素
                    auto dest = new_start.cur;
                    for (auto it = first; it != last; ++it) 
                    {
                        mystl::construct(dest, *it);
                        ++dest;
                    }
                    
                    // 再复制原有元素
                    dest = new_start.cur + n;
                    for (auto it = old_start; it != pos; ++it) 
                    {
                        mystl::construct(dest, *it);
                        ++dest;
                    }
                    start_ = new_start;  // 更新start_
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
                iterator new_finish = reserve_elements_at_back(n);
                iterator old_finish = finish_;
                
                try 
                {
                    const size_type elems_after = size() - elems_before;
                    if (elems_after > n) 
                    {
                        // 先复制到新空间
                        auto copy_dest = finish_;
                        for (auto it = finish_ - n; it != finish_; ++it)
                        {
                            mystl::construct(copy_dest.cur, *it);
                            ++copy_dest;
                        }
                        
                        try 
                        {
                            // 手动移动原有元素
                            auto move_dest = old_finish - n;
                            for (auto it = old_finish - n; it != pos; ++it)
                            {
                                *move_dest = *it;
                                ++move_dest;
                            }
                            
                            // 手动插入新元素
                            auto insert_dest = iterator(pos);
                            for (auto it = first; it != last; ++it)
                            {
                                *insert_dest = *it;
                                ++insert_dest;
                            }
                            
                            // 所有操作成功后才更新finish_
                            finish_ = new_finish;
                        }
                        catch (...) 
                        {
                            // 清理已复制的元素
                            mystl::destroy(finish_, finish_ + n);
                            destroy_nodes_at_back(new_finish);
                            throw;
                        }
                    }
                    else 
                    {
                        // 先在新空间中构造剩余元素
                        auto construct_dest = finish_;
                        for (size_type i = 0; i < n - elems_after; ++i)
                        {
                            auto it = first + elems_after + i;
                            mystl::construct(construct_dest.cur, *it);
                            ++construct_dest;
                        }
                        
                        try 
                        {
                            // 复制原有元素到新位置
                            auto copy_dest = construct_dest;
                            for (auto it = pos; it != finish_; ++it)
                            {
                                mystl::construct(copy_dest.cur, *it);
                                ++copy_dest;
                            }
                            
                            // 手动插入前面的新元素
                            auto insert_dest = iterator(pos);
                            for (auto it = first; it != first + elems_after; ++it)
                            {
                                *insert_dest = *it;
                                ++insert_dest;
                            }
                            
                            // 所有操作成功后才更新finish_
                            finish_ = new_finish;
                        }
                        catch (...) 
                        {
                            // 清理已构造的元素
                            mystl::destroy_n(finish_, n);
                            destroy_nodes_at_back(new_finish);
                            throw;
                        }
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

        // 在pos之前插入初始化列表中的元素
        iterator insert(const_iterator pos, std::initializer_list<T> ilist)
        {
            return insert(pos, ilist.begin(), ilist.end());
        }


        // 在指定位置就地构造元素
        template <class... Args>
        iterator emplace(const_iterator pos, Args&&... args)
        {
            if (pos.cur == start_.cur)
            {
                emplace_front(mystl::forward<Args>(args)...);
                return start_;
            }
            else if (pos.cur == finish_.cur)
            {
                emplace_back(mystl::forward<Args>(args)...);
                return finish_ - 1;
            }

            const size_type elems_before = pos - start_;
            if (elems_before < size() / 2)
            {
                // 在前半段插入
                iterator new_start = reserve_elements_at_front(1);
                iterator old_start = start_;
                
                try 
                {
                    // 先在新位置构造元素
                    mystl::construct(new_start.cur, mystl::forward<Args>(args)...);
                    
                    try 
                    {
                        // 再移动原有元素
                        mystl::uninitialized_copy(start_, iterator(pos), new_start + 1);
                        start_ = new_start;  // 所有操作成功后才更新start_
                    }
                    catch (...)
                    {
                        mystl::destroy(new_start.cur);
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
                iterator old_finish = finish_;
                
                try 
                {
                    // 先构造最后一个元素
                    mystl::construct(finish_.cur, back());
                    
                    try 
                    {
                        // 移动元素并在正确位置构造新元素
                        mystl::copy_backward(iterator(pos), finish_ - 1, finish_);
                        mystl::destroy(pos.cur);  // 销毁原位置的元素
                        mystl::construct(pos.cur, mystl::forward<Args>(args)...);  // 构造新元素
                        ++finish_;  // 所有操作成功后才更新finish_
                    }
                    catch (...)
                    {
                        mystl::destroy(finish_.cur);
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

        // 在头部就地构造元素
        template <class... Args>
        void emplace_front(Args&&... args)
        {
            if (start_.cur != start_.first)
            {
                // 当前缓冲区还有空间
                try 
                {
                    mystl::construct(start_.cur - 1, mystl::forward<Args>(args)...);
                    --start_;
                }
                catch (...)
                {
                    ++start_;  // 恢复原状
                    throw;
                }
            }
            else
            {
                // 需要新的缓冲区
                iterator new_start = reserve_elements_at_front(1);
                try 
                {
                    mystl::construct(new_start.cur, mystl::forward<Args>(args)...);
                    start_ = new_start;
                }
                catch (...)
                {
                    destroy_nodes_at_front(new_start);
                    throw;
                }
            }
        }

        // 在尾部就地构造元素
        template <class... Args>
        void emplace_back(Args&&... args)
        {
            if (finish_.cur != finish_.last - 1)
            {
                // 当前缓冲区还有空间
                try 
                {
                    mystl::construct(finish_.cur, mystl::forward<Args>(args)...);
                    ++finish_;
                }
                catch (...)
                {
                    mystl::destroy(finish_.cur);
                    throw;
                }
            }
            else
            {
                // 需要新的缓冲区
                iterator new_finish = reserve_elements_at_back(1);
                try 
                {
                    mystl::construct(finish_.cur, mystl::forward<Args>(args)...);
                    ++finish_;
                }
                catch (...)
                {
                    destroy_nodes_at_back(new_finish);
                    throw;
                }
            }
        }

        void push_back(const T& value) 
        {
            if (finish_.cur != finish_.last - 1) 
            {
                // 当前缓冲区还有空间
                try 
                {
                    mystl::construct(finish_.cur, value);
                    ++finish_;
                }
                catch (...) 
                {
                    mystl::destroy(finish_.cur);
                    throw;
                }
            }
            else 
            {
                // 需要新的缓冲区
                iterator new_finish = reserve_elements_at_back(1);
                try 
                {
                    mystl::construct(finish_.cur, value);
                    ++finish_;
                }
                catch (...) 
                {
                    destroy_nodes_at_back(new_finish);
                    throw;
                }
            }
        }
        
        void push_front(const T& value)
        {
            if (start_.cur != start_.first) 
            {
                // 当前缓冲区还有空间
                try 
                {
                    mystl::construct(start_.cur - 1, value);
                    --start_;
                }
                catch (...) 
                {
                    ++start_;  // 恢复原状
                    throw;
                }
            }
            else 
            {
                // 需要新的缓冲区
                iterator new_start = reserve_elements_at_front(1);
                try 
                {
                    mystl::construct(new_start.cur, value);
                    start_ = new_start;
                }
                catch (...) 
                {
                    destroy_nodes_at_front(new_start);
                    throw;
                }
            }
        }
        
        void pop_back() 
        {
            if (finish_.cur != finish_.first) 
            {
                // 当前缓冲区有多于一个元素
                --finish_;
                mystl::destroy(finish_.cur);
            }
            else 
            {
                // 需要释放当前缓冲区
                iterator new_finish = finish_;
                --new_finish;  // 指向前一个缓冲区的最后一个元素
                mystl::destroy(finish_.cur);
                data_allocator::deallocate(*finish_.node, buffer_size());
                finish_ = new_finish;
            }
        }
        
        void pop_front() 
        {
            if (start_.cur != start_.last - 1) 
            {
                // 当前缓冲区有多于一个元素
                mystl::destroy(start_.cur);
                ++start_;
            }
            else 
            {
                // 需要释放当前缓冲区
                iterator new_start = start_;
                ++new_start;  // 指向下一个缓冲区的第一个元素
                mystl::destroy(start_.cur);
                data_allocator::deallocate(*start_.node, buffer_size());
                start_ = new_start;
            }
        }
        

        iterator erase(const_iterator pos) 
        {
            iterator next = iterator(pos.cur);
            ++next;
            const difference_type index = pos - begin();
            
            if (index < (size() >> 1)) 
            {
                // 如果删除点之前的元素较少，向后移动前面的元素
                mystl::copy_backward(begin(), iterator(pos), next);
                pop_front();
            }
            else 
            {
                // 如果删除点之后的元素较少，向前移动后面的元素
                mystl::copy(next, end(), iterator(pos));
                pop_back();
            }
            return begin() + index;
        }

        // 添加范围删除
        iterator erase(const_iterator first, const_iterator last) 
        {
            if (first == begin() && last == end()) 
            {
                clear();
                return end();
            }

            const difference_type n = last - first;
            const difference_type elems_before = first - begin();
            
            if (elems_before < (size() - n) / 2) 
            {
                // 如果前面的元素较少，向后移动前面的元素
                mystl::copy_backward(begin(), iterator(first), iterator(last));
                iterator new_start = begin() + n;
                destroy_range(begin(), new_start);
                for (map_pointer cur = start_.node; cur < new_start.node; ++cur)
                {
                    data_allocator::deallocate(*cur, buffer_size());
                    *cur = nullptr;
                }
                start_ = new_start;
            }
            else 
            {
                // 如果后面的元素较少，向前移动后面的元素
                mystl::copy(iterator(last), end(), iterator(first));
                iterator new_finish = end() - n;
                destroy_range(new_finish, end());
                for (map_pointer cur = new_finish.node + 1; cur <= finish_.node; ++cur)
                {
                    data_allocator::deallocate(*cur, buffer_size());
                    *cur = nullptr;
                }
                finish_ = new_finish;
            }
            return begin() + elems_before;
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
        // 创建map和节点
        void create_map_and_nodes(size_type num_elements) 
        {
            // 需要的节点数 = (元素个数 / 每个缓冲区可容纳的元素个数) + 1
            size_type num_nodes = num_elements / deque_buf_size(sizeof(T)) + 1;
            
            // map至少管理8个节点，最多是所需节点数加2
            map_size_ = mystl::max(size_type(8), num_nodes + 2);
            map_ = map_alloc_.allocate(map_size_);
            
            // 让 nstart 和 nfinish 都指向 map_ 最中央的区域
            T** nstart = map_ + (map_size_ - num_nodes) / 2;
            T** nfinish = nstart + num_nodes - 1;
            
            try 
            {
                // 为每个节点配置缓冲区
                for (T** cur = nstart; cur <= nfinish; ++cur)
                    *cur = alloc_.allocate(deque_buf_size(sizeof(T)));
            }
            catch(...) 
            {
                // 若配置失败，释放已配置的空间
                for (T** cur = nstart; cur < nfinish; ++cur)
                    alloc_.deallocate(*cur, deque_buf_size(sizeof(T)));
                map_alloc_.deallocate(map_, map_size_);
                throw;
            }
            
            // 设置迭代器
            start_.node = nstart;
            finish_.node = nfinish;
            start_.cur = start_.first = *nstart;
            start_.last = start_.first + deque_buf_size(sizeof(T));
            finish_.first = *nfinish;
            finish_.last = finish_.first + deque_buf_size(sizeof(T));
            finish_.cur = finish_.first;
        }

        

        // 在pos位置插入一个元素的辅助函数(通用版本)
        template <class V>
        iterator insert_aux(const_iterator pos, V&& value)
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
                        // 使用显式转换和手动拷贝
                        auto dest = new_start.cur + 1;
                        for (auto it = start_; it != pos; ++it) 
                        {
                            // 直接使用值构造
                            mystl::construct(dest, *it);
                            ++dest;
                        }
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
                        // 使用显式转换和手动拷贝
                        auto dest = finish_.cur;
                        for (auto it = finish_ - 1; it >= pos; --it) 
                        {
                            // 直接使用值构造
                            mystl::construct(dest, *it);
                            --dest;
                        }
                        
                        // 直接在 pos 位置赋值，而不是使用 const_cast
                        *(start_ + (pos - cbegin())) = mystl::forward<V>(value);
                        
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


        // 在前端预留n个元素的空间
        iterator reserve_elements_at_front(size_type n)
        {
            size_type vacancies = start_.cur - start_.first;  // 当前缓冲区前端的空闲空间
            if (n > vacancies)
            {
                // 需要的空间大于当前缓冲区前端的空闲空间
                size_type new_nodes = (n - vacancies) / buffer_size() + 1;
                reserve_map_at_front(new_nodes);  // 确保map前端有足够空间
                
                // 分配新的缓冲区
                size_type i;
                try
                {
                    for (i = 1; i <= new_nodes; ++i)
                        *(start_.node - i) = data_allocator::allocate(buffer_size());
                }
                catch (...)
                {
                    // 如果分配失败，释放已分配的缓冲区
                    for (size_type j = 1; j < i; ++j)
                        data_allocator::deallocate(*(start_.node - j), buffer_size());
                    throw;
                }
            }
            return start_ - n;  // 返回新的起始位置
        }

        // 在后端预留n个元素的空间
        iterator reserve_elements_at_back(size_type n)
        {
            size_type vacancies = (finish_.last - finish_.cur) - 1;  // 当前缓冲区后端的空闲空间
            if (n > vacancies)
            {
                // 需要的空间大于当前缓冲区后端的空闲空间
                size_type new_nodes = (n - vacancies) / buffer_size() + 1;
                reserve_map_at_back(new_nodes);  // 确保map后端有足够空间
                
                // 分配新的缓冲区
                size_type i;
                try
                {
                    for (i = 1; i <= new_nodes; ++i)
                        *(finish_.node + i) = data_allocator::allocate(buffer_size());
                }
                catch (...)
                {
                    // 如果分配失败，释放已分配的缓冲区
                    for (size_type j = 1; j < i; ++j)
                        data_allocator::deallocate(*(finish_.node + j), buffer_size());
                    throw;
                }
            }
            return finish_ + n;  // 返回新的结束位置
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
                // 现有map空间足够，只需要移动数据
                new_nstart = map_ + (map_size_ - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
                if (new_nstart < start_.node)
                {
                    mystl::copy(start_.node, finish_.node + 1, new_nstart);
                    // 只有复制成功后才更新节点指针
                    start_.set_node(new_nstart);
                    finish_.set_node(new_nstart + old_num_nodes - 1);
                }
                else
                {
                    mystl::copy_backward(start_.node, finish_.node + 1, new_nstart + old_num_nodes);
                    // 只有复制成功后才更新节点指针
                    start_.set_node(new_nstart);
                    finish_.set_node(new_nstart + old_num_nodes - 1);
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
                data_allocator::deallocate(*n, buffer_size());
                *n = nullptr;
            }
        }

        // 在后端释放节点
        void destroy_nodes_at_back(iterator after_finish)
        {
            for (map_pointer n = after_finish.node; n > finish_.node; --n)
            {
                data_allocator::deallocate(*n, buffer_size());
                *n = nullptr;
            }
        }

        // 销毁[first, last)范围内的元素
        void destroy_range(iterator first, iterator last)
        {
            // 如果区间在同一个缓冲区内
            if (first.node == last.node)
            {
                mystl::destroy(first.cur, last.cur);
            }
            else
            {
                // 处理第一个缓冲区
                mystl::destroy(first.cur, first.last);
                
                // 处理中间的完整缓冲区
                for (map_pointer node = first.node + 1; node < last.node; ++node)
                {
                    mystl::destroy(*node, *node + buffer_size());
                }
                
                // 处理最后一个缓冲区
                if (first.node != last.node)  // 确保有最后一个缓冲区
                {
                    mystl::destroy(last.first, last.cur);
                }
            }
        }

        static size_type buffer_size() noexcept
        {
            return deque_buf_size(sizeof(T));
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