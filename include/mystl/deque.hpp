#pragma once

#include "allocator.hpp"
#include "iterator.hpp"

namespace mystl 
{

    // 设置 buffer 大小
    inline size_t deque_buf_size(size_t size) 
    {
        return size < 512 ? size_t(512 / size) : size_t(1);
    }

    // deque 的迭代器设计
    template <class T>
    struct deque_iterator 
    {
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using difference_type = ptrdiff_t;
        using iterator_category = mystl::random_access_iterator_tag;
        
        // 迭代器需要维护当前元素的位置和所在缓冲区的信息
        T* cur;           // 指向当前元素
        T* first;         // 指向当前缓冲区的头
        T* last;          // 指向当前缓冲区的尾
        T** node;         // 指向控制中心
        
        // 重载运算符
        reference operator*() const 
        {
            return *cur;
        }
        
        pointer operator->() const 
        {
            return cur;
        }
        
        difference_type operator-(const deque_iterator& x) const 
        {
            return difference_type(deque_buf_size(sizeof(T))) * (node - x.node - 1) + (cur - first) + (x.last - x.cur);
        }
        
        deque_iterator& operator++() 
        {
            ++cur;
            if (cur == last) 
            {
                set_node(node + 1);
                cur = first;
            }
            return *this;
        }

        void set_node(T** new_node) 
        {
            node = new_node;
            first = *new_node;
            last = first + difference_type(deque_buf_size(sizeof(T)));
        }

        deque_iterator operator++(int) 
        {
            deque_iterator tmp = *this;
            ++*this;
            return tmp;
        }

        deque_iterator& operator--() 
        {
            if (cur == first) 
            {
                set_node(node - 1);
                cur = last - 1;
            }
            else 
            {
                --cur;
            }
            return *this;
        }

        deque_iterator operator--(int) 
        {
            deque_iterator tmp = *this;
            --*this;
            return tmp;
        }

        deque_iterator& operator+=(difference_type n) 
        {
            difference_type offset = n + (cur - first);
            if (offset >= 0 && offset < difference_type(deque_buf_size(sizeof(T)))) 
            {
                // 目标位置在同一缓冲区内
                cur += n;
            }
            else 
            {
                // 目标位置在其他缓冲区
                difference_type node_offset =
                    offset > 0 ? offset / difference_type(deque_buf_size(sizeof(T)))
                              : -difference_type((-offset - 1) / deque_buf_size(sizeof(T))) - 1;
                set_node(node + node_offset);
                cur = first + (offset - node_offset * difference_type(deque_buf_size(sizeof(T))));
            }
            return *this;
        }

        deque_iterator operator+(difference_type n) const 
        {
            deque_iterator tmp = *this;
            return tmp += n;
        }

        deque_iterator& operator-=(difference_type n) 
        {
            return *this += -n;
        }

        deque_iterator operator-(difference_type n) const 
        {
            deque_iterator tmp = *this;
            return tmp -= n;
        }

        reference operator[](difference_type n) const 
        {
            return *(*this + n);
        }

        bool operator==(const deque_iterator& x) const 
        {
            return cur == x.cur;
        }

        bool operator!=(const deque_iterator& x) const 
        {
            return !(*this == x);
        }

        bool operator<(const deque_iterator& x) const 
        {
            return (node == x.node) ? (cur < x.cur) : (node < x.node);
        }
    };

    template <class T, class Allocator = mystl::allocator<T>>
    class deque 
    {
    public:
        // 类型定义
        using value_type = T;
        using allocator_type = Allocator;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = typename allocator_type::pointer;
        using const_pointer = typename allocator_type::const_pointer;
        using iterator = deque_iterator<T>;
        using const_iterator = deque_iterator<const T>;
        
    private:
        using map_allocator = typename Allocator::template rebind<T*>::other;
        
        iterator start_;           // 指向第一个元素
        iterator finish_;          // 指向最后一个元素的下一个位置
        T** map_;                  // 控制中心，存储缓冲区的地址
        size_type map_size_;       // 控制中心大小
        
        // 分配器
        allocator_type alloc_;     // 元素分配器
        map_allocator map_alloc_;  // map分配器

    public:
        // 构造函数
        deque() : map_(nullptr), map_size_(0) 
        {
            create_map_and_nodes(0);
        }

        explicit deque(size_type count) 
        {
            create_map_and_nodes(count);
            mystl::uninitialized_fill(start_, finish_, value_type());
        }

        deque(size_type count, const T& value) 
        {
            create_map_and_nodes(count);
            mystl::uninitialized_fill(start_, finish_, value);
        }

        deque(const deque& other) 
        {
            create_map_and_nodes(other.size());
            mystl::uninitialized_copy(other.begin(), other.end(), start_);
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
        
        // 容量相关
        bool empty() const 
        {
            return start_ == finish_;
        }
        
        size_type size() const 
        {
            return finish_ - start_;
        }
        
        // 元素访问
        reference front() 
        {
            return *start_;
        }
        
        reference back() 
        {
            return *(finish_ - 1);
        }
        
        // 修改容器操作
        void push_back(const T& value) 
        {
            if (finish_.cur != finish_.last - 1) 
            {
                alloc_.construct(finish_.cur, value);
                ++finish_.cur;
            }
            else 
            {
                push_back_aux(value);
            }
        }
        
        void push_front(const T& value) 
        {
            if (start_.cur != start_.first) 
            {
                alloc_.construct(start_.cur - 1, value);
                --start_.cur;
            }
            else 
            {
                push_front_aux(value);
            }
        }
        
        void pop_back() 
        {
            if (finish_.cur != finish_.first) 
            {
                --finish_.cur;
                alloc_.destroy(finish_.cur);
            }
            else 
            {
                pop_back_aux();
            }
        }
        
        void pop_front() 
        {
            if (start_.cur != start_.last - 1) 
            {
                alloc_.destroy(start_.cur);
                ++start_.cur;
            }
            else 
            {
                pop_front_aux();
            }
        }
        
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
        void assign(InputIt first, InputIt last) 
        {
            clear();
            insert(begin(), first, last);
        }
        
        iterator insert(const_iterator pos, const T& value) 
        {
            if (pos.cur == start_.cur) 
            {
                push_front(value);
                return start_;
            }
            else if (pos.cur == finish_.cur) 
            {
                push_back(value);
                iterator tmp = finish_;
                --tmp;
                return tmp;
            }
            return insert_aux(pos, value);
        }

        iterator erase(const_iterator pos) 
        {
            iterator next = iterator(pos.cur);
            ++next;
            difference_type index = pos - begin();
            if (index < (size() >> 1)) 
            {
                mystl::copy_backward(begin(), iterator(pos), next);
                pop_front();
            }
            else 
            {
                mystl::copy(next, end(), iterator(pos));
                pop_back();
            }
            return begin() + index;
        }

        iterator begin() noexcept { return start_; }
        const_iterator begin() const noexcept { return start_; }
        iterator end() noexcept { return finish_; }
        const_iterator end() const noexcept { return finish_; }
        reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
        const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
        reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
        const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

        void resize(size_type count) 
        {
            resize(count, value_type());
        }

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

        void shrink_to_fit() 
        {
            // 实现收缩容量的操作
        }

        // 添加范围删除
        iterator erase(const_iterator first, const_iterator last) 
        {
            if (first == begin() && last == end()) 
            {
                clear();
                return end();
            }
            else 
            {
                difference_type n = last - first;
                difference_type elems_before = first - begin();
                if (elems_before < (size() - n) / 2) 
                {
                    mystl::copy_backward(begin(), iterator(first), iterator(last));
                    iterator new_start = begin() + n;
                    destroy_range(begin(), new_start);
                    for (map_pointer cur = start_.node; cur < new_start.node; ++cur)
                        data_allocator::deallocate(*cur, buffer_size());
                    start_ = new_start;
                }
                else 
                {
                    mystl::copy(iterator(last), end(), iterator(first));
                    iterator new_finish = end() - n;
                    destroy_range(new_finish, end());
                    for (map_pointer cur = new_finish.node + 1; cur <= finish_.node; ++cur)
                        data_allocator::deallocate(*cur, buffer_size());
                    finish_ = new_finish;
                }
                return begin() + elems_before;
            }
        }

        // 添加清空操作
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

        // 添加交换操作
        void swap(deque& other) noexcept 
        {
            mystl::swap(start_, other.start_);
            mystl::swap(finish_, other.finish_);
            mystl::swap(map_, other.map_);
            mystl::swap(map_size_, other.map_size_);
        }

    private:
        // 辅助函数
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

        void push_back_aux(const T& value) 
        {
            reserve_map_at_back();
            *(finish_.node + 1) = alloc_.allocate(deque_buf_size(sizeof(T)));
            try 
            {
                alloc_.construct(finish_.cur, value);
                finish_.set_node(finish_.node + 1);
                finish_.cur = finish_.first;
            }
            catch(...) 
            {
                alloc_.deallocate(*(finish_.node + 1), deque_buf_size(sizeof(T)));
                throw;
            }
        }

        void push_front_aux(const T& value) 
        {
            reserve_map_at_front();
            *(start_.node - 1) = alloc_.allocate(deque_buf_size(sizeof(T)));
            try 
            {
                start_.set_node(start_.node - 1);
                start_.cur = start_.last - 1;
                alloc_.construct(start_.cur, value);
            }
            catch(...) 
            {
                start_.set_node(start_.node + 1);
                start_.cur = start_.first;
                alloc_.deallocate(*(start_.node - 1), deque_buf_size(sizeof(T)));
                throw;
            }
        }

        void pop_back_aux() 
        {
            alloc_.deallocate(*finish_.node, deque_buf_size(sizeof(T)));
            finish_.set_node(finish_.node - 1);
            finish_.cur = finish_.last - 1;
            alloc_.destroy(finish_.cur);
        }

        void pop_front_aux() 
        {
            alloc_.destroy(start_.cur);
            alloc_.deallocate(*start_.node, deque_buf_size(sizeof(T)));
            start_.set_node(start_.node + 1);
            start_.cur = start_.first;
        }

        void reserve_map_at_back(size_type nodes_to_add = 1) 
        {
            if (nodes_to_add + 1 > map_size_ - (finish_.node - map_))
                reallocate_map(nodes_to_add, false);
        }

        void reserve_map_at_front(size_type nodes_to_add = 1) 
        {
            if (nodes_to_add > size_type(start_.node - map_))
                reallocate_map(nodes_to_add, true);
        }

        void reallocate_map(size_type nodes_to_add, bool add_at_front) 
        {
            size_type old_num_nodes = finish_.node - start_.node + 1;
            size_type new_num_nodes = old_num_nodes + nodes_to_add;

            map_pointer new_nstart;
            if (map_size_ > 2 * new_num_nodes) 
            {
                new_nstart = map_ + (map_size_ - new_num_nodes) / 2 
                            + (add_at_front ? nodes_to_add : 0);
                if (new_nstart < start_.node)
                    mystl::copy(start_.node, finish_.node + 1, new_nstart);
                else
                    mystl::copy_backward(start_.node, finish_.node + 1, 
                                       new_nstart + old_num_nodes);
            }
            else 
            {
                size_type new_map_size = map_size_ + mystl::max(map_size_, nodes_to_add) + 2;
                map_pointer new_map = map_allocator::allocate(new_map_size);
                new_nstart = new_map + (new_map_size - new_num_nodes) / 2
                            + (add_at_front ? nodes_to_add : 0);
                mystl::copy(start_.node, finish_.node + 1, new_nstart);
                map_allocator::deallocate(map_, map_size_);
                map_ = new_map;
                map_size_ = new_map_size;
            }

            start_.set_node(new_nstart);
            finish_.set_node(new_nstart + old_num_nodes - 1);
        }
    };

} // namespace mystl 