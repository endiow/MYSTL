#pragma once

#include <initializer_list>
#include "expectdef.hpp"
#include "uninitialized.hpp"
#include "allocator.hpp"
#include "algorithm.hpp"

namespace mystl 
{
    /*****************************************************************************************/
    // vector 的迭代器设计
    /*****************************************************************************************/
    template <class T>
    class vector_iterator 
    {
    public:
        // 类型定义
        using iterator_category = mystl::random_access_iterator_tag;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using difference_type = ptrdiff_t;
        using iterator_type = vector_iterator<T>;

    private:
        pointer curr_;  // 当前指向的元素

    public:
        // 构造函数
        vector_iterator() noexcept : curr_(nullptr) {}
        explicit vector_iterator(pointer ptr) noexcept : curr_(ptr) {}

        // 基本操作
        reference operator*() const noexcept { return *curr_; }
        pointer operator->() const noexcept { return curr_; }
        reference operator[](difference_type n) const noexcept { return curr_[n]; }
        pointer base() const noexcept { return curr_; }

        // 迭代器移动
        iterator_type& operator++() noexcept 
        {
            ++curr_;
            return *this;
        }

        iterator_type operator++(int) noexcept 
        {
            iterator_type tmp = *this;
            ++curr_;
            return tmp;
        }

        iterator_type& operator--() noexcept 
        {
            --curr_;
            return *this;
        }

        iterator_type operator--(int) noexcept 
        {
            iterator_type tmp = *this;
            --curr_;
            return tmp;
        }

        // 算术运算
        iterator_type& operator+=(difference_type n) noexcept 
        {
            curr_ += n;
            return *this;
        }

        iterator_type operator+(difference_type n) const noexcept 
        {
            return iterator_type(curr_ + n);
        }

        template <class U>
        iterator_type operator+(const vector_iterator<U>& other) const noexcept 
        {
            return iterator_type(curr_ + other.base());
        }

        iterator_type& operator-=(difference_type n) noexcept 
        {
            curr_ -= n;
            return *this;
        }

        iterator_type operator-(difference_type n) const noexcept 
        {
            return iterator_type(curr_ - n);
        }

        difference_type operator-(const iterator_type& other) const noexcept 
        {
            return curr_ - other.curr_;
        }

        template <class U>
        difference_type operator-(const vector_iterator<U>& other) const noexcept 
        {
            return curr_ - other.base();
        }

        // 比较运算
        bool operator==(const iterator_type& other) const noexcept 
        {
            return curr_ == other.curr_;
        }

        bool operator!=(const iterator_type& other) const noexcept 
        {
            return !(*this == other);
        }

        bool operator<(const iterator_type& other) const noexcept 
        {
            return curr_ < other.curr_;
        }

        bool operator>(const iterator_type& other) const noexcept 
        {
            return other < *this;
        }

        bool operator<=(const iterator_type& other) const noexcept 
        {
            return !(other < *this);
        }

        bool operator>=(const iterator_type& other) const noexcept 
        {
            return !(*this < other);
        }

        // 比较运算 - 支持不同类型迭代器之间的比较（比如 iterator 和 const_iterator）
        template <class U>
        bool operator==(const vector_iterator<U>& other) const noexcept 
        {
            return curr_ == other.base();
        }

        template <class U>
        bool operator!=(const vector_iterator<U>& other) const noexcept 
        {
            return !(*this == other);
        }

        template <class U>
        bool operator<(const vector_iterator<U>& other) const noexcept 
        {
            return curr_ < other.base();
        }

        template <class U>
        bool operator>(const vector_iterator<U>& other) const noexcept 
        {
            return other < *this;
        }

        template <class U>
        bool operator<=(const vector_iterator<U>& other) const noexcept 
        {
            return !(other < *this);
        }

        template <class U>
        bool operator>=(const vector_iterator<U>& other) const noexcept 
        {
            return !(*this < other);
        }

        // 类型转换
        operator vector_iterator<const T>() const noexcept 
        { 
            return vector_iterator<const T>(curr_); 
        }

        //友元函数
        friend iterator_type operator+(difference_type n, const iterator_type& it) noexcept 
        {
            return it + n;
        }

        friend iterator_type operator-(difference_type n, const iterator_type& it) noexcept 
        {
            return it - n;
        }
    };




    /*****************************************************************************************/
    // vector 的实现
    /*****************************************************************************************/
    template <class T, class Allocator = mystl::allocator<T>>
    class vector 
    {
    public:
        //------------------------------------------------------------------------------
        // 类型定义
        //------------------------------------------------------------------------------
        using value_type = T;
        using allocator_type = Allocator;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = typename allocator_type::pointer;
        using const_pointer = typename allocator_type::const_pointer;
        using iterator = vector_iterator<T>;
        using const_iterator = vector_iterator<const T>;
        using reverse_iterator = mystl::reverse_iterator<iterator>;
        using const_reverse_iterator = mystl::reverse_iterator<const_iterator>;

    private:
        //------------------------------------------------------------------------------
        // 成员变量
        //------------------------------------------------------------------------------
        pointer data_;          // 指向实际数据存储区的指针
        size_type size_;       // 当前存储的元素个数
        size_type capacity_;   // 当前分配的存储空间大小
        allocator_type alloc_; // 内存分配器实例

    public:
        //------------------------------------------------------------------------------
        // 构造/析构函数
        //------------------------------------------------------------------------------
        
        // 默认构造函数
        vector() noexcept : data_(nullptr), size_(0), capacity_(0) {}
        
        // 创建包含count个默认值的vector
        explicit vector(size_type count)
            : size_(count), capacity_(count)
        {
            data_ = alloc_.allocate(count);
            for (size_type i = 0; i < count; ++i)
            {
                alloc_.construct(data_ + i);
            }
        }

        // 创建包含count个值为value的vector
        vector(size_type count, const T& value)
            : size_(count), capacity_(count)
        {
            data_ = alloc_.allocate(count);
            for (size_type i = 0; i < count; ++i)
            {
                alloc_.construct(data_ + i, value);
            }
        }

        // 迭代器范围构造
        template<class InputIt, typename = typename enable_if<!is_integral<InputIt>::value>::type>
        vector(InputIt first, InputIt last) 
        {
            const auto n = mystl::distance(first, last);
            data_ = alloc_.allocate(n);
            size_ = n;
            capacity_ = n;
            mystl::uninitialized_copy(first, last, data_);
        }

        // 使用初始化列表创建vector
        vector(std::initializer_list<T> init)
            : size_(init.size()), capacity_(init.size())
        {
            data_ = alloc_.allocate(capacity_);
            size_type i = 0;
            for (const auto& item : init)
            {
                alloc_.construct(data_ + i++, item);
            }
        }

        // 创建vector的深拷贝
        vector(const vector& other) 
            : size_(other.size_), capacity_(other.size_)
        {
            data_ = alloc_.allocate(capacity_);
            for (size_type i = 0; i < size_; ++i)
            {
                alloc_.construct(data_ + i, other.data_[i]);
            }
        }

        // 移动构造一个vector
        vector(vector&& other) noexcept
            : data_(other.data_), size_(other.size_), capacity_(other.capacity_)
        {
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }

        // 析构函数
        ~vector() 
        {
            clear();
            if (data_) 
            {
                alloc_.deallocate(data_, capacity_);
            }
        }



        //------------------------------------------------------------------------------
        // 迭代器
        //------------------------------------------------------------------------------

        // 返回指向首元素的迭代器
        iterator begin() noexcept { return iterator(data_); }
        const_iterator begin() const noexcept { return const_iterator(data_); }
        const_iterator cbegin() const noexcept { return const_iterator(data_); }

        // 返回指向末元素后一位置的迭代器
        iterator end() noexcept { return iterator(data_ + size_); }
        const_iterator end() const noexcept { return const_iterator(data_ + size_); }
        const_iterator cend() const noexcept { return const_iterator(data_ + size_); }

        // 添加反向迭代器相关函数
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



        //------------------------------------------------------------------------------
        // 元素访问
        //------------------------------------------------------------------------------
        
        // 访问指定位置元素（无边界检查）
        reference operator[](size_type pos) { return data_[pos]; }
        const_reference operator[](size_type pos) const { return data_[pos]; }

        // 带边界检查的访问
        reference at(size_type pos)
        {
            if (pos >= size_)
            {
                throw out_of_range("vector::at");
            }
            return data_[pos];
        }

        const_reference at(size_type pos) const
        {
            if (pos >= size_)
            {
                throw out_of_range("vector::at");
            }
            return data_[pos];
        }

        // 返回第一个元素的引用
        reference front() { return data_[0]; }
        const_reference front() const { return data_[0]; }

        // 返回最后一个元素的引用
        reference back() { return data_[size_ - 1]; }
        const_reference back() const { return data_[size_ - 1]; }

        // 返回指向内存的指针
        pointer data() noexcept { return data_; }
        const_pointer data() const noexcept { return data_; }



        //------------------------------------------------------------------------------
        // 容量操作
        //------------------------------------------------------------------------------
        
        // 检查容器是否为空
        bool empty() const noexcept { return size_ == 0; }
        // 返回当前元素个数
        size_type size() const noexcept { return size_; }
        // 返回最大可容纳的元素个数
        size_type max_size() const noexcept 
        { 
            // 考虑两个限制因素：
            // 1. size_type 能表示的最大值
            // 2. 最大可分配的内存 / 每个元素的大小
            return mystl::min(
                size_type(-1) / sizeof(T),  // 防止内存大小溢出
                alloc_.max_size()           // 分配器的限制
            );
        }
        // 返回当前容量
        size_type capacity() const noexcept { return capacity_; }

        // 预留存储空间
        void reserve(size_type new_cap)
        {
            if (new_cap <= capacity_) return;

            pointer new_data = nullptr;
            size_type i = 0;
            try 
            {
                new_data = alloc_.allocate(new_cap);
                // 先复制，如果复制成功再移动
                for (; i < size_; ++i)
                {
                    alloc_.construct(new_data + i, data_[i]);  // 使用复制而不是移动，保证原数据不变
                }
                
                // 复制成功后，清理原内存
                for (size_type j = 0; j < size_; ++j)
                {
                    alloc_.destroy(data_ + j);
                }
                if (data_)
                {
                    alloc_.deallocate(data_, capacity_);
                }
                
                data_ = new_data;
                capacity_ = new_cap;
            }
            catch (...) 
            {
                // 析构对象
                for (size_type j = 0; j < i; ++j)
                {
                    alloc_.destroy(new_data + j);
                }
                // 释放内存
                if (new_data)   
                {
                    alloc_.deallocate(new_data, new_cap);
                }
                throw;  // 重新抛出异常
            }
        }

        // 减少容器容量以适应其大小
        void shrink_to_fit()
        {
            if (size_ < capacity_)
            {
                vector temp(*this);
                swap(temp);
            }
        }

        // 调整容器大小（用默认值填充）
        void resize(size_type count)
        {
            if (count > size_)
            {
                if (count > capacity_)
                {
                    reserve(count);  // reserve 已经有异常安全保证
                }
                
                size_type i = size_;
                try 
                {
                    // 构造新元素
                    for (; i < count; ++i)
                    {
                        alloc_.construct(data_ + i);
                    }
                    size_ = count;
                }
                catch (...) 
                {
                    // 如果构造失败，清理已构造的新元素
                    for (size_type j = size_; j < i; ++j)
                    {
                        alloc_.destroy(data_ + j);
                    }
                    throw;  // 重新抛出异常
                }
            }
            else
            {
                // 销毁多余元素
                for (size_type i = count; i < size_; ++i)
                {
                    alloc_.destroy(data_ + i);
                }
                size_ = count;
            }
        }

        // 调整容器大小（用指定值填充）
        void resize(size_type count, const value_type& value)
        {
            if (count > size_)
            {
                if (count > capacity_)
                {
                    reserve(count);  // reserve 已经有异常安全保证
                }
                
                size_type i = size_;
                try 
                {
                    // 构造新元素
                    for (; i < count; ++i)
                    {
                        alloc_.construct(data_ + i, value);
                    }
                    size_ = count;
                }
                catch (...) 
                {
                    // 如果构造失败，清理已构造的新元素
                    for (size_type j = size_; j < i; ++j)
                    {
                        alloc_.destroy(data_ + j);
                    }
                    throw;  // 重新抛出异常
                }
            }
            else
            {
                // 销毁多余元素
                for (size_type i = count; i < size_; ++i)
                {
                    alloc_.destroy(data_ + i);
                }
                size_ = count;
            }
        }



        //------------------------------------------------------------------------------
        // 赋值操作
        //------------------------------------------------------------------------------
        
        // 拷贝赋值运算符
        vector& operator=(const vector& other)
        {
            if (this != &other)
            {
                vector temp(other);
                swap(temp);
            }
            return *this;
        }

        // 移动赋值操作符
        vector& operator=(vector&& other) noexcept
        {
            if (this != &other)
            {
                clear();
                if (data_)
                {
                    alloc_.deallocate(data_, capacity_);
                }
                data_ = other.data_;
                size_ = other.size_;
                capacity_ = other.capacity_;
                other.data_ = nullptr;
                other.size_ = 0;
                other.capacity_ = 0;
            }
            return *this;
        }

        // assign 操作：用新内容替换原内容
        template<class InputIt, typename = typename enable_if<!is_integral<InputIt>::value>::type>
        void assign(InputIt first, InputIt last) 
        {
            size_type new_size = mystl::distance(first, last);
            if (new_size > capacity_)
            {
                pointer new_data = nullptr;
                try 
                {
                    new_data = alloc_.allocate(new_size);
                    mystl::uninitialized_copy(first, last, new_data);
                    
                    clear();  // 先清理旧元素
                    if (data_)
                    {
                        alloc_.deallocate(data_, capacity_);
                    }

                    data_ = new_data;
                    size_ = new_size;
                    capacity_ = new_size;
                }
                catch (...) 
                {
                    // 如果发生异常，清理新分配的内存
                    if (new_data)
                    {
                        for (size_type i = 0; i < new_size; ++i)
                        {
                            alloc_.destroy(new_data + i);
                        }
                        alloc_.deallocate(new_data, new_size);
                    }
                    throw;  // 重新抛出异常
                }
            }
            else 
            {
                clear();  // 先清理旧元素
                mystl::uninitialized_copy(first, last, data_);  // 在原空间构造新元素
                size_ = new_size;
            }
        }

        // assign 操作：用 n 个值为 value 的元素替换原内容
        void assign(size_type n, const value_type& value) 
        {
            if (n > capacity_)
            {
                pointer new_data = nullptr;
                try 
                {
                    new_data = alloc_.allocate(n);
                    mystl::uninitialized_fill_n(new_data, n, value);
                    
                    clear();  // 先清理旧元素
                    if (data_)
                    {
                        alloc_.deallocate(data_, capacity_);
                    }
                    
                    data_ = new_data;
                    size_ = n;
                    capacity_ = n;
                }
                catch (...) 
                {
                    // 如果发生异常，清理新分配的内存
                    if (new_data)
                    {
                        for (size_type i = 0; i < n; ++i)
                        {
                            alloc_.destroy(new_data + i);
                        }
                        alloc_.deallocate(new_data, n);
                    }
                    throw;  // 重新抛出异常
                }
            }
            else 
            {
                clear();  // 先清理旧元素
                mystl::uninitialized_fill_n(data_, n, value);  // 在原空间构造新元素
                size_ = n;
            }
        }

        // 使用初始化列表赋值
        void assign(std::initializer_list<T> ilist) 
        {
            assign(ilist.begin(), ilist.end());
        }



        //------------------------------------------------------------------------------
        // 修改器
        //------------------------------------------------------------------------------

        // 清空所有元素
        void clear() noexcept
        {
            for (size_type i = 0; i < size_; ++i)
            {
                alloc_.destroy(data_ + i);
            }
            size_ = 0;
        }

        // 在指定位置插入元素
        iterator insert(const_iterator pos, const value_type& value) 
        {
            size_type offset = pos - cbegin();
            if (size_ == capacity_)
            {
                size_type new_capacity = (capacity_ == 0) ? 1 : 2 * capacity_;
                pointer new_data = nullptr;
                try 
                {
                    new_data = alloc_.allocate(new_capacity);
                    mystl::uninitialized_copy(begin(), begin() + offset, new_data);
                    alloc_.construct(new_data + offset, value);
                    mystl::uninitialized_copy(begin() + offset, end(), new_data + offset + 1);
                }
                catch (...) 
                {
                    if (new_data)
                    {
                        for (size_type i = 0; i < offset; ++i)
                        {
                            alloc_.destroy(new_data + i);
                        }
                        alloc_.deallocate(new_data, new_capacity);
                    }
                    throw;
                }
                
                for (size_type i = 0; i < size_; ++i)
                {
                    alloc_.destroy(data_ + i);
                }
                if (data_)
                {
                    alloc_.deallocate(data_, capacity_);
                }
                
                data_ = new_data;
                capacity_ = new_capacity;
            }
            else 
            {
                try 
                {
                    // 先构造末尾元素
                    alloc_.construct(data_ + size_, mystl::move(data_[size_ - 1]));
                    
                    // 移动中间元素
                    size_type i;
                    try 
                    {
                        for (i = size_ - 1; i > offset; --i)
                        {
                            data_[i] = mystl::move(data_[i - 1]);
                        }
                    }
                    catch (...) 
                    {
                        // 如果移动失败，恢复已移动的元素
                        for (size_type k = size_ - 1; k > i; --k)
                        {
                            data_[k - 1] = mystl::move(data_[k]);
                        }
                        alloc_.destroy(data_ + size_);
                        throw;
                    }
                    
                    // 插入新元素
                    try 
                    {
                        data_[offset] = value;
                    }
                    catch (...) 
                    {
                        // 如果插入失败，恢复已移动的元素
                        for (size_type k = size_ - 1; k > offset; --k)
                        {
                            data_[k - 1] = mystl::move(data_[k]);
                        }
                        alloc_.destroy(data_ + size_);
                        throw;
                    }
                }
                catch (...) 
                {
                    // 如果在构造末尾元素时发生异常，直接销毁
                    alloc_.destroy(data_ + size_);
                    throw;
                }
            }
            ++size_;
            return iterator(data_ + offset);
        }

        // 在指定位置插入元素
        iterator insert(const_iterator pos, T&& value) 
        {
            size_type offset = pos - cbegin();
            if (size_ == capacity_)
            {
                size_type new_capacity = (capacity_ == 0) ? 1 : 2 * capacity_;
                pointer new_data = nullptr;
                try 
                {
                    new_data = alloc_.allocate(new_capacity);
                    mystl::uninitialized_copy(begin(), begin() + offset, new_data);
                    alloc_.construct(new_data + offset, mystl::move(value));
                    mystl::uninitialized_copy(begin() + offset, end(), new_data + offset + 1);
                }
                catch (...) 
                {
                    if (new_data)
                    {
                        for (size_type i = 0; i < offset; ++i)
                        {
                            alloc_.destroy(new_data + i);
                        }
                        alloc_.deallocate(new_data, new_capacity);
                    }
                    throw;
                }
                
                for (size_type i = 0; i < size_; ++i)
                {
                    alloc_.destroy(data_ + i);
                }
                if (data_)
                {
                    alloc_.deallocate(data_, capacity_);
                }
                
                data_ = new_data;
                capacity_ = new_capacity;
            }
            else 
            {
                // 先构造新元素的副本，如果构造失败，原数据不变
                value_type tmp(mystl::move(value));
                
                try 
                {
                    // 先构造末尾元素
                    alloc_.construct(data_ + size_, mystl::move(data_[size_ - 1]));
                    
                    // 移动中间元素
                    size_type i;
                    try 
                    {
                        for (i = size_ - 1; i > offset; --i)
                        {
                            data_[i] = mystl::move(data_[i - 1]);
                        }
                    }
                    catch (...) 
                    {
                        // 如果移动失败，恢复已移动的元素
                        for (size_type k = size_ - 1; k > i; --k)
                        {
                            data_[k - 1] = mystl::move(data_[k]);
                        }
                        alloc_.destroy(data_ + size_);
                        throw;
                    }
                    
                    // 插入新元素
                    data_[offset] = mystl::move(tmp);
                }
                catch (...) 
                {
                    // 如果在构造末尾元素时发生异常，直接销毁
                    alloc_.destroy(data_ + size_);
                    throw;
                }
            }
            ++size_;
            return iterator(data_ + offset);
        }

        // 在指定位置插入多个相同的元素
        iterator insert(const_iterator pos, size_type count, const value_type& value) 
        {
            size_type offset = pos - cbegin();
            if (size_ + count > capacity_)
            {
                size_type new_capacity = mystl::max(2 * capacity_, size_ + count);
                pointer new_data = nullptr;
                try 
                {
                    new_data = alloc_.allocate(new_capacity);
                    mystl::uninitialized_copy(begin(), begin() + offset, new_data);
                    mystl::uninitialized_fill_n(new_data + offset, count, value);
                    mystl::uninitialized_copy(begin() + offset, end(), new_data + offset + count);
                }
                catch (...) 
                {
                    if (new_data)
                    {
                        for (size_type i = 0; i < offset; ++i)
                        {
                            alloc_.destroy(new_data + i);
                        }
                        alloc_.deallocate(new_data, new_capacity);
                    }
                    throw;
                }
                
                for (size_type i = 0; i < size_; ++i)
                {
                    alloc_.destroy(data_ + i);
                }
                if (data_)
                {
                    alloc_.deallocate(data_, capacity_);
                }
                
                data_ = new_data;
                capacity_ = new_capacity;
            }
            else 
            {
                size_type constructed = 0;  // 记录已构造的元素数量
                try 
                {
                    // 先构造末尾的元素
                    for (; constructed < count; ++constructed)
                    {
                        alloc_.construct(data_ + size_ + constructed, mystl::move(data_[size_ - count + constructed]));
                    }
                    
                    // 移动中间元素
                    size_type move_idx;
                    try 
                    {
                        for (move_idx = size_ - count - 1; move_idx >= offset && move_idx < size_; --move_idx)
                        {
                            data_[move_idx + count] = mystl::move(data_[move_idx]);
                        }
                    }
                    catch (...) 
                    {
                        // 如果移动失败，恢复已移动的元素
                        for (size_type k = size_ - count - 1; k > move_idx; --k)
                        {
                            data_[k] = mystl::move(data_[k + count]);
                        }
                        // 销毁末尾临时空间
                        for (size_type k = 0; k < count; ++k)
                        {
                            alloc_.destroy(data_ + size_ + k);
                        }
                        throw;
                    }
                    
                    // 填充新值
                    size_type filled = 0;  // 记录已填充的元素数量
                    try 
                    {
                        for (; filled < count; ++filled)
                        {
                            data_[offset + filled] = value;
                        }
                    }
                    catch (...) 
                    {
                        // 如果填充新值时发生异常，恢复已移动的元素
                        for (size_type k = filled; k > 0; --k)
                        {
                            data_[offset + k - 1] = mystl::move(data_[offset + k - 1 + count]);
                        }
                        // 恢复中间移动的元素
                        for (size_type k = offset; k < size_ - count; ++k)
                        {
                            data_[k] = mystl::move(data_[k + count]);
                        }
                        // 销毁末尾临时空间
                        for (size_type k = 0; k < count; ++k)
                        {
                            alloc_.destroy(data_ + size_ + k);
                        }
                        throw;
                    }
                }
                catch (...) 
                {
                    // 如果在构造末尾元素时发生异常，销毁已构造的元素
                    for (size_type k = 0; k < constructed; ++k)
                    {
                        alloc_.destroy(data_ + size_ + k);
                    }
                    throw;
                }
            }
            size_ += count;
            return iterator(data_ + offset);
        }

        // 在指定位置插入范围内的元素
        template<class InputIt, typename = typename enable_if<!is_integral<InputIt>::value>::type>
        iterator insert(const_iterator pos, InputIt first, InputIt last) 
        {
            size_type offset = pos - cbegin();
            size_type count = mystl::distance(first, last);
            
            // 检查是否是自引用
            bool is_self = false;
            const auto* ptr = data();
            if constexpr (is_same_v<typename mystl::iterator_traits<InputIt>::iterator_category, random_access_iterator_tag>)
            {
                const auto* first_ptr = &(*first);
                const auto* last_ptr = &(*(last - 1));
                is_self = (first_ptr >= ptr && first_ptr < ptr + size_) || (last_ptr >= ptr && last_ptr < ptr + size_);
            }
            
            if (is_self)
            {
                // 如果是自引用，先复制要插入的数据
                vector tmp(first, last);
                return insert(pos, tmp.begin(), tmp.end());
            }
            
            if (size_ + count > capacity_)
            {
                size_type new_capacity = mystl::max(2 * capacity_, size_ + count);
                pointer new_data = nullptr;
                try 
                {
                    new_data = alloc_.allocate(new_capacity);
                    mystl::uninitialized_copy(begin(), begin() + offset, new_data);
                    mystl::uninitialized_copy(first, last, new_data + offset);
                    mystl::uninitialized_copy(begin() + offset, end(), new_data + offset + count);
                }
                catch (...) 
                {
                    if (new_data)
                    {
                        for (size_type i = 0; i < offset; ++i)
                        {
                            alloc_.destroy(new_data + i);
                        }
                        alloc_.deallocate(new_data, new_capacity);
                    }
                    throw;
                }
                
                for (size_type i = 0; i < size_; ++i)
                {
                    alloc_.destroy(data_ + i);
                }
                if (data_)
                {
                    alloc_.deallocate(data_, capacity_);
                }
                
                data_ = new_data;
                capacity_ = new_capacity;
            }
            else 
            {
                size_type constructed = 0;  // 记录已构造的元素数量
                try 
                {
                    // 先构造末尾的元素
                    for (; constructed < count; ++constructed)
                    {
                        alloc_.construct(data_ + size_ + constructed, mystl::move(data_[size_ - count + constructed]));
                    }
                    
                    // 移动中间元素
                    size_type move_idx;
                    try 
                    {
                        for (move_idx = size_ - count - 1; move_idx >= offset && move_idx < size_; --move_idx)
                        {
                            data_[move_idx + count] = mystl::move(data_[move_idx]);
                        }
                    }
                    catch (...) 
                    {
                        // 如果移动失败，恢复已移动的元素
                        for (size_type k = size_ - count - 1; k > move_idx; --k)
                        {
                            data_[k] = mystl::move(data_[k + count]);
                        }
                        // 销毁末尾临时空间
                        for (size_type k = 0; k < count; ++k)
                        {
                            alloc_.destroy(data_ + size_ + k);
                        }
                        throw;
                    }
                    
                    // 插入新元素
                    auto curr = first;
                    size_type inserted = 0;
                    try 
                    {
                        for (; inserted < count; ++inserted, ++curr)
                        {
                            data_[offset + inserted] = *curr;
                        }
                    }
                    catch (...) 
                    {
                        // 如果插入失败，恢复已移动的元素
                        for (size_type k = inserted; k > 0; --k)
                        {
                            data_[offset + k - 1] = mystl::move(data_[offset + k - 1 + count]);
                        }
                        // 恢复中间移动的元素
                        for (size_type k = offset; k < size_ - count; ++k)
                        {
                            data_[k] = mystl::move(data_[k + count]);
                        }
                        // 销毁末尾临时空间
                        for (size_type k = 0; k < count; ++k)
                        {
                            alloc_.destroy(data_ + size_ + k);
                        }
                        throw;
                    }
                }
                catch (...) 
                {
                    // 如果在构造末尾元素时发生异常，销毁已构造的元素
                    for (size_type k = 0; k < constructed; ++k)
                    {
                        alloc_.destroy(data_ + size_ + k);
                    }
                    throw;
                }
            }
            size_ += count;
            return iterator(data_ + offset);
        }

        // 在指定位置插入初始化列表
        iterator insert(const_iterator pos, std::initializer_list<T> ilist) 
        {
            return insert(pos, ilist.begin(), ilist.end());
        }

        template<class... Args>
        iterator emplace(const_iterator pos, Args&&... args) 
        {
            size_type offset = pos - cbegin();
            if (size_ == capacity_)
            {
                size_type new_capacity = (capacity_ == 0) ? 1 : 2 * capacity_;
                pointer new_data = nullptr;
                try 
                {
                    // 1. 分配新内存
                    new_data = alloc_.allocate(new_capacity);
                    
                    // 2. 复制前半部分
                    mystl::uninitialized_copy(begin(), begin() + offset, new_data);
                    
                    // 3. 构造新元素
                    alloc_.construct(new_data + offset, mystl::forward<Args>(args)...);
                    
                    // 4. 复制后半部分
                    mystl::uninitialized_copy(begin() + offset, end(), new_data + offset + 1);
                }
                catch (...) 
                {
                    // 清理已构造的元素
                    if (new_data)
                    {
                        for (size_type i = 0; i < offset; ++i)
                        {
                            alloc_.destroy(new_data + i);
                        }
                        alloc_.deallocate(new_data, new_capacity);
                    }
                    throw;  // 重新抛出异常
                }
                
                // 清理旧内存
                for (size_type i = 0; i < size_; ++i)
                {
                    alloc_.destroy(data_ + i);
                }
                if (data_)
                {
                    alloc_.deallocate(data_, capacity_);
                }
                
                data_ = new_data;
                capacity_ = new_capacity;
            }
            else 
            {
                // 先尝试构造新元素，如果失败，原数据不变
                value_type tmp(mystl::forward<Args>(args)...);
                
                try 
                {
                    alloc_.construct(data_ + size_, mystl::move(data_[size_ - 1]));
                    
                    size_type move_idx;
                    try 
                    {
                        for (move_idx = size_ - 1; move_idx > offset; --move_idx)
                        {
                            data_[move_idx] = mystl::move(data_[move_idx - 1]);
                        }
                    }
                    catch (...) 
                    {
                        // 如果移动失败，恢复已移动的元素
                        for (size_type k = size_ - 1; k > move_idx; --k)
                        {
                            data_[k - 1] = mystl::move(data_[k]);
                        }
                        alloc_.destroy(data_ + size_);
                        throw;
                    }
                    
                    data_[offset] = mystl::move(tmp);
                }
                catch (...) 
                {
                    // 如果在构造末尾元素时发生异常，直接销毁
                    alloc_.destroy(data_ + size_);
                    throw;
                }
            }
            ++size_;
            return iterator(data_ + offset);
        }

        template<class... Args>
        void emplace_back(Args&&... args)
        {
            if (size_ == capacity_)
            {
                reserve(capacity_ == 0 ? 1 : 2 * capacity_);
            }
            alloc_.construct(data_ + size_, mystl::forward<Args>(args)...);
            ++size_;
        }

        // 在末尾添加元素（拷贝版本）
        void push_back(const T& value)
        {
            if (size_ == capacity_)
            {
                size_type new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
                pointer new_data = nullptr;
                try 
                {
                    new_data = alloc_.allocate(new_capacity);
                    mystl::uninitialized_copy(begin(), end(), new_data);
                    alloc_.construct(new_data + size_, value);
                }
                catch (...) 
                {
                    // 清理已构造的元素
                    if (new_data)
                    {
                        for (size_type i = 0; i < size_; ++i)
                        {
                            alloc_.destroy(new_data + i);
                        }
                        alloc_.deallocate(new_data, new_capacity);
                    }
                    throw;  // 重新抛出异常
                }
                
                // 清理旧内存
                for (size_type i = 0; i < size_; ++i)
                {
                    alloc_.destroy(data_ + i);
                }
                if (data_)
                {
                    alloc_.deallocate(data_, capacity_);
                }
                
                data_ = new_data;
                capacity_ = new_capacity;
            }
            else 
            {
                alloc_.construct(data_ + size_, mystl::move(value));
            }
            ++size_;
        }

        // 在末尾添加元素（移动版本）
        void push_back(T&& value)
        {
            if (size_ == capacity_)
            {
                size_type new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
                pointer new_data = nullptr;
                try 
                {
                    new_data = alloc_.allocate(new_capacity);
                    mystl::uninitialized_copy(begin(), end(), new_data);
                    alloc_.construct(new_data + size_, mystl::move(value));
                }
                catch (...) 
                {
                    if (new_data)
                    {
                        for (size_type i = 0; i < size_; ++i)
                        {
                            alloc_.destroy(new_data + i);
                        }
                        alloc_.deallocate(new_data, new_capacity);
                    }
                    throw;
                }
                
                for (size_type i = 0; i < size_; ++i)
                {
                    alloc_.destroy(data_ + i);
                }
                if (data_)
                {
                    alloc_.deallocate(data_, capacity_);
                }
                
                data_ = new_data;
                capacity_ = new_capacity;
            }
            else 
            {
                alloc_.construct(data_ + size_, mystl::move(value));
            }
            ++size_;
        }

        // 删除最后一个元素
        void pop_back()
        {
            if (!empty())
            {
                alloc_.destroy(data_ + --size_);
            }
        }

        // 删除指定位置的元素
        iterator erase(const_iterator pos) 
        {
            if (pos == end()) return iterator(data_ + size_);
            
            size_type offset = pos - cbegin();
            // 移动后续元素
            for (size_type i = offset; i < size_ - 1; ++i)
            {
                alloc_.destroy(data_ + i);
                alloc_.construct(data_ + i, mystl::move(data_[i + 1]));
            }
            alloc_.destroy(data_ + size_ - 1);
            --size_;
            return iterator(data_ + offset);
        }

        // 删除一段区间的元素
        iterator erase(const_iterator first, const_iterator last) 
        {
            if (first == last) return iterator(data_ + (first - cbegin()));
            
            size_type offset = first - cbegin();
            size_type count = last - first;
            
            // 移动后续元素
            for (size_type i = offset; i < size_ - count; ++i)
            {
                alloc_.destroy(data_ + i);
                alloc_.construct(data_ + i, mystl::move(data_[i + count]));
            }
            
            // 销毁多余元素
            for (size_type i = size_ - count; i < size_; ++i)
            {
                alloc_.destroy(data_ + i);
            }
            
            size_ -= count;
            return iterator(data_ + offset);
        }

        // 与另一个vector交换内容
        void swap(vector& other) noexcept
        {
            mystl::swap(data_, other.data_);
            mystl::swap(size_, other.size_);
            mystl::swap(capacity_, other.capacity_);
        }        
    };



    //------------------------------------------------------------------------------
    // 非成员函数
    //------------------------------------------------------------------------------
    
    // 判断两个vector是否相等
    template<class T, class Alloc>
    bool operator==(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs)
    {
        return lhs.size() == rhs.size() && mystl::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    // 判断两个vector是否不相等
    template<class T, class Alloc>
    bool operator!=(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs)
    {
        return !(lhs == rhs);
    }

    template<class T, class Alloc>
    bool operator<(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs)
    {
        return mystl::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template<class T, class Alloc>
    bool operator>(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs)
    {
        return rhs < lhs;
    }

    template<class T, class Alloc>
    bool operator<=(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs)
    {
        return !(rhs < lhs);
    }

    template<class T, class Alloc>
    bool operator>=(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs)
    {
        return !(lhs < rhs);
    }

    // 交换两个vector的内容
    template<class T, class Alloc>
    void swap(vector<T, Alloc>& lhs, vector<T, Alloc>& rhs) noexcept
    {
        lhs.swap(rhs);
    }
} // namespace mystl
