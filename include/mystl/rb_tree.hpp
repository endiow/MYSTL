#pragma once
#include "functional.hpp"
#include "allocator.hpp"
#include "iterator.hpp"

namespace mystl 
{

    // 红黑树节点颜色
    using rb_tree_color_type = bool;
    static constexpr rb_tree_color_type rb_tree_red = false;    // 红色为0
    static constexpr rb_tree_color_type rb_tree_black = true;   // 黑色为1

    // 红黑树节点基类
    struct rb_tree_node_base 
    {
        using color_type = rb_tree_color_type;
        using base_ptr = rb_tree_node_base*;

        color_type color;   // 节点颜色
        base_ptr parent;    // 父节点
        base_ptr left;      // 左子节点
        base_ptr right;     // 右子节点

        static base_ptr minimum(base_ptr x) noexcept
        {
            while (x->left != nullptr) 
                x = x->left;
            return x;
        }

        static base_ptr maximum(base_ptr x) noexcept
        {
            while (x->right != nullptr) 
                x = x->right;
            return x;
        }
    };

    // 红黑树节点
    template <class T>
    struct rb_tree_node : public rb_tree_node_base 
    {
        using link_type = rb_tree_node<T>*;
        T value;  // 节点值
    };

    // 红黑树迭代器基类
    class rb_tree_iterator_base 
    {
    public:
        using base_ptr = rb_tree_node_base*;
        base_ptr node;  // 改为 public

        rb_tree_iterator_base() noexcept : node(nullptr) {}
        explicit rb_tree_iterator_base(base_ptr x) noexcept : node(x) {}

        void increment() noexcept 
        {
            if (node->right != nullptr) 
            {
                // 有右子树，则找右子树的最左节点
                node = node->right;
                while (node->left != nullptr)
                    node = node->left;
            }
            else 
            {
                // 没有右子树，则找第一个"该节点位于其左子树"的祖先节点
                base_ptr y = node->parent;
                while (node == y->right) 
                {
                    node = y;
                    y = y->parent;
                }
                if (node->right != y)  // 特殊情况：当前节点为根节点
                    node = y;
            }
        }

        void decrement() noexcept 
        {
            if (node->color == rb_tree_red && node->parent->parent == node)
            {
                // 特殊情况：node为header时
                node = node->right;
            }
            else if (node->left != nullptr) 
            {
                // 有左子树，则找左子树的最右节点
                base_ptr y = node->left;
                while (y->right != nullptr)
                    y = y->right;
                node = y;
            }
            else 
            {
                // 没有左子树，则找第一个"该节点位于其右子树"的祖先节点
                base_ptr y = node->parent;
                while (node == y->left) 
                {
                    node = y;
                    y = y->parent;
                }
                node = y;
            }
        }

        bool operator==(const rb_tree_iterator_base& rhs) const noexcept
        {
            return node == rhs.node;
        }

        bool operator!=(const rb_tree_iterator_base& rhs) const noexcept
        {
            return node != rhs.node;
        }
    };

    // 红黑树迭代器
    template <class T, class Ref, class Ptr>
    class rb_tree_iterator : public rb_tree_iterator_base 
    {
    public:
        using iterator_category = mystl::bidirectional_iterator_tag;
        using value_type = T;
        using reference = Ref;
        using pointer = Ptr;
        using difference_type = ptrdiff_t;
        using base = rb_tree_iterator_base;
        using self = rb_tree_iterator<T, Ref, Ptr>;
        using link_type = rb_tree_node<T>*;

        rb_tree_iterator() noexcept {}
        explicit rb_tree_iterator(base_ptr x) noexcept : base(x) {}

        reference operator*() const noexcept 
        { 
            return static_cast<link_type>(node)->value; 
        }

        pointer operator->() const noexcept 
        { 
            return &(operator*()); 
        }

        self& operator++() noexcept
        {
            increment();
            return *this;
        }

        self operator++(int) noexcept
        {
            self tmp = *this;
            increment();
            return tmp;
        }

        self& operator--() noexcept
        {
            decrement();
            return *this;
        }

        self operator--(int) noexcept
        {
            self tmp = *this;
            decrement();
            return tmp;
        }
    };

    // 红黑树
    template <class T, class Compare = mystl::less<T>, class Alloc = mystl::allocator<T>>
    class rb_tree 
    {
    public:
        // 类型定义
        using value_type = typename Alloc::value_type;
        using allocator_type = Alloc;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using pointer = typename allocator_type::pointer;
        using const_pointer = typename allocator_type::const_pointer;
        using reference = value_type&;
        using const_reference = const value_type&;

        using iterator = rb_tree_iterator<value_type, value_type&, value_type*>;
        using const_iterator = rb_tree_iterator<value_type, const value_type&, const value_type*>;
        using reverse_iterator = mystl::reverse_iterator<iterator>;
        using const_reverse_iterator = mystl::reverse_iterator<const_iterator>;

    protected:
        // 节点相关
        using rb_tree_node = rb_tree_node<value_type>;
        using node_allocator = typename Alloc::template rebind<rb_tree_node>::other;

        node_allocator node_alloc_;  // 节点分配器
        size_type node_count_;       // 节点数
        rb_tree_node* header_;       // 特殊节点，与根节点互为父节点
        Compare key_compare;         // 节点键值比较函数

        // 获取节点颜色
        static rb_tree_color_type& color(rb_tree_node* x) noexcept
        { 
            return x->color; 
        }

        // 获取父节点
        static rb_tree_node* parent(rb_tree_node* x) noexcept
        { 
            return reinterpret_cast<rb_tree_node*>(x->parent); 
        }

        // 获取左子节点
        static rb_tree_node* left(rb_tree_node* x) noexcept
        { 
            return reinterpret_cast<rb_tree_node*>(x->left); 
        }

        // 获取右子节点
        static rb_tree_node* right(rb_tree_node* x) noexcept
        { 
            return reinterpret_cast<rb_tree_node*>(x->right); 
        }

        // 设置父节点
        static void set_parent(rb_tree_node* node, rb_tree_node* parent) noexcept
        {
            node->parent = parent;
        }

        // 设置左子节点
        static void set_left(rb_tree_node* node, rb_tree_node* left) noexcept
        {
            node->left = left;
        }

        // 设置右子节点
        static void set_right(rb_tree_node* node, rb_tree_node* right) noexcept
        {
            node->right = right;
        }

        // 获取最小值节点
        static rb_tree_node* minimum(rb_tree_node* x) noexcept
        { 
            return static_cast<rb_tree_node*>(rb_tree_node_base::minimum(x)); 
        }

        // 获取最大值节点
        static rb_tree_node* maximum(rb_tree_node* x) noexcept
        { 
            return static_cast<rb_tree_node*>(rb_tree_node_base::maximum(x)); 
        }

    public:
        // 构造函数
        rb_tree() 
        { 
            init(); 
        }

        rb_tree(const Compare& comp) 
            : key_compare(comp)
        { 
            init(); 
        }

        // 析构函数
        ~rb_tree() 
        { 
            clear(); 
            deallocate_node(header_); 
        }

        // 迭代器相关
        iterator begin() noexcept 
        { 
            return iterator(left(header_)); 
        }
        const_iterator begin() const noexcept 
        { 
            return const_iterator(left(header_)); 
        }
        iterator end() noexcept 
        { 
            return iterator(header_); 
        }
        const_iterator end() const noexcept 
        { 
            return const_iterator(header_); 
        }
        const_iterator cbegin() const noexcept 
        { 
            return begin(); 
        }
        const_iterator cend() const noexcept 
        { 
            return end(); 
        }
        reverse_iterator rbegin() noexcept 
        { 
            return reverse_iterator(end()); 
        }
        const_reverse_iterator rbegin() const noexcept 
        { 
            return const_reverse_iterator(end()); 
        }
        reverse_iterator rend() noexcept 
        { 
            return reverse_iterator(begin()); 
        }
        const_reverse_iterator rend() const noexcept 
        { 
            return const_reverse_iterator(begin()); 
        }

        // 容量相关
        bool empty() const noexcept 
        { 
            return node_count_ == 0; 
        }
        size_type size() const noexcept 
        { 
            return node_count_; 
        }
        size_type max_size() const noexcept 
        { 
            return static_cast<size_type>(-1); 
        }

        // 插入操作
        mystl::pair<iterator, bool> insert_unique(const value_type& value)
        {
            rb_tree_node* y = get_insert_pos(value);
            if (y == header_ || key_compare(value, y->value))  // 插入在左边
            {
                return mystl::pair<iterator, bool>(
                    insert_aux(y, value, true), true);
            }
            else if (key_compare(y->value, value))  // 插入在右边
            {
                return mystl::pair<iterator, bool>(
                    insert_aux(y, value, false), true);
            }
            // 值相等，不插入
            return mystl::pair<iterator, bool>(iterator(y), false);
        }

        iterator insert_equal(const value_type& value)
        {
            rb_tree_node* y = get_insert_pos(value);
            return insert_aux(y, value, false);
        }

        // 删除操作
        void erase(iterator position)
        {
            rb_tree_node* y = static_cast<rb_tree_node*>(rb_tree_erase(position.node));
            destroy_node(y);
            --node_count_;
        }

        size_type erase(const value_type& value)
        {
            auto p = equal_range(value);
            size_type n = mystl::distance(p.first, p.second);
            erase(p.first, p.second);
            return n;
        }

        void erase(iterator first, iterator last)
        {
            if (first == begin() && last == end())
                clear();
            else
                while (first != last) 
                    erase(first++);
        }

        // 查找操作
        iterator find(const value_type& value)
        {
            rb_tree_node* y = header_;
            rb_tree_node* x = parent(header_);

            while (x != nullptr)
            {
                if (!key_compare(x->value, value))
                {
                    y = x;
                    x = left(x);
                }
                else
                    x = right(x);
            }

            iterator j = iterator(y);
            return (j == end() || key_compare(value, *j)) ? end() : j;
        }

        const_iterator find(const value_type& value) const
        {
            rb_tree_node* y = header_;
            rb_tree_node* x = parent(header_);

            while (x != nullptr)
            {
                if (!key_compare(x->value, value))
                {
                    y = x;
                    x = left(x);
                }
                else
                    x = right(x);
            }

            const_iterator j = const_iterator(y);
            return (j == end() || key_compare(value, *j)) ? end() : j;
        }

        // 范围查找
        mystl::pair<iterator, iterator> equal_range(const value_type& value)
        {
            return mystl::pair<iterator, iterator>(
                lower_bound(value), upper_bound(value));
        }

        mystl::pair<const_iterator, const_iterator> 
        equal_range(const value_type& value) const
        {
            return mystl::pair<const_iterator, const_iterator>(
                lower_bound(value), upper_bound(value));
        }

        iterator lower_bound(const value_type& value)
        {
            rb_tree_node* y = header_;
            rb_tree_node* x = parent(header_);

            while (x != nullptr)
            {
                if (!key_compare(x->value, value))
                {
                    y = x;
                    x = left(x);
                }
                else
                    x = right(x);
            }

            return iterator(y);
        }

        const_iterator lower_bound(const value_type& value) const
        {
            rb_tree_node* y = header_;
            rb_tree_node* x = parent(header_);

            while (x != nullptr)
            {
                if (!key_compare(x->value, value))
                {
                    y = x;
                    x = left(x);
                }
                else
                    x = right(x);
            }

            return const_iterator(y);
        }

        iterator upper_bound(const value_type& value)
        {
            rb_tree_node* y = header_;
            rb_tree_node* x = parent(header_);

            while (x != nullptr)
            {
                if (key_compare(value, x->value))
                {
                    y = x;
                    x = left(x);
                }
                else
                    x = right(x);
            }

            return iterator(y);
        }

        const_iterator upper_bound(const value_type& value) const
        {
            rb_tree_node* y = header_;
            rb_tree_node* x = parent(header_);

            while (x != nullptr)
            {
                if (key_compare(value, x->value))
                {
                    y = x;
                    x = left(x);
                }
                else
                    x = right(x);
            }

            return const_iterator(y);
        }

        // 赋值操作
        rb_tree& operator=(const rb_tree& rhs)
        {
            if (this != &rhs)
            {
                clear();
                key_compare = rhs.key_compare;
                if (!rhs.empty())
                {
                    try 
                    {
                        parent(header_) = copy_tree(parent(rhs.header_), header_);
                        left(header_) = minimum(parent(header_));
                        right(header_) = maximum(parent(header_));
                        node_count_ = rhs.node_count_;
                    }
                    catch (...) 
                    {
                        clear();
                        throw;
                    }
                }
            }
            return *this;
        }

        rb_tree& operator=(rb_tree&& rhs) noexcept
        {
            if (this != &rhs)
            {
                clear();
                header_ = rhs.header_;
                node_count_ = rhs.node_count_;
                key_compare = rhs.key_compare;
                
                rhs.header_ = nullptr;
                rhs.node_count_ = 0;
            }
            return *this;
        }

        // 比较操作
        bool operator==(const rb_tree& rhs) const
        {
            return size() == rhs.size() && 
                   mystl::equal(begin(), end(), rhs.begin());
        }

        bool operator!=(const rb_tree& rhs) const
        {
            return !(*this == rhs);
        }

        bool operator<(const rb_tree& rhs) const
        {
            return mystl::lexicographical_compare(begin(), end(), 
                                                rhs.begin(), rhs.end());
        }

        bool operator>(const rb_tree& rhs) const
        {
            return rhs < *this;
        }

        bool operator<=(const rb_tree& rhs) const
        {
            return !(rhs < *this);
        }

        bool operator>=(const rb_tree& rhs) const
        {
            return !(*this < rhs);
        }

        // 清空操作
        void clear()
        {
            if (node_count_ != 0)
            {
                clear_tree(parent(header_));
                set_parent(header_, nullptr);
                set_left(header_, header_);
                set_right(header_, header_);
                node_count_ = 0;
            }
        }

        // 获取比较函数
        Compare key_comp() const { return key_compare; }

        // 交换操作
        void swap(rb_tree& rhs) noexcept
        {
            if (this != &rhs)
            {
                mystl::swap(header_, rhs.header_);
                mystl::swap(node_count_, rhs.node_count_);
                mystl::swap(key_compare, rhs.key_compare);
            }
        }

        // 计数操作
        size_type count(const value_type& value) const
        {
            auto p = equal_range(value);
            return mystl::distance(p.first, p.second);
        }

        // 获取根节点
        rb_tree_node* get_root() const noexcept 
        { 
            return parent(header_); 
        }
        
        // 获取最小节点
        rb_tree_node* get_leftmost() const noexcept 
        { 
            return left(header_); 
        }
        
        // 获取最大节点
        rb_tree_node* get_rightmost() const noexcept 
        { 
            return right(header_); 
        }

    protected:
        // 初始化红黑树
        void init() noexcept
        {
            // 分配并初始化 header 节点
            header_ = allocate_node();
            
            // 将 header 节点设为黑色
            color(header_) = rb_tree_red;  // header 为红色,与根节点区分
            
            // 初始化指针
            set_parent(header_, nullptr);
            set_left(header_, header_);    // 初始时最小节点为自己
            set_right(header_, header_);   // 初始时最大节点为自己
            
            // 初始化节点计数
            node_count_ = 0;
        }

        // 分配一个节点
        rb_tree_node* allocate_node()
        {
            return node_alloc_.allocate(1);
        }

        // 释放一个节点
        void deallocate_node(rb_tree_node* p)
        {
            node_alloc_.deallocate(p, 1);
        }

        // 创建一个节点
        rb_tree_node* create_node(const value_type& value)
        {
            rb_tree_node* tmp = allocate_node();
            try 
            {
                mystl::construct(&tmp->value, value);
                tmp->left = nullptr;
                tmp->right = nullptr;
                tmp->parent = nullptr;
                tmp->color = rb_tree_red;  // 新节点默认为红色
            }
            catch (...) 
            {
                deallocate_node(tmp);
                throw;
            }
            return tmp;
        }

        // 复制一个节点
        rb_tree_node* clone_node(rb_tree_node* x)
        {
            rb_tree_node* tmp = create_node(x->value);
            tmp->color = x->color;
            return tmp;
        }

        // 销毁一个节点
        void destroy_node(rb_tree_node* p)
        {
            mystl::destroy_at(&p->value);
            deallocate_node(p);
        }

        // 左旋操作
        void rb_tree_rotate_left(rb_tree_node* x) noexcept
        {
            rb_tree_node* y = right(x);
            set_right(x, left(y));
            if (left(y) != nullptr)
                set_parent(left(y), x);
            set_parent(y, parent(x));
            if (x == parent(header_))  // x是根节点
                set_parent(header_, y);
            else if (x == left(parent(x)))  // x是左子节点
                set_left(parent(x), y);
            else  // x是右子节点
                set_right(parent(x), y);
            set_left(y, x);
            set_parent(x, y);
        }

        // 右旋操作
        void rb_tree_rotate_right(rb_tree_node* x) noexcept
        {
            rb_tree_node* y = left(x);
            set_left(x, right(y));
            if (right(y) != nullptr)
                set_parent(right(y), x);
            set_parent(y, parent(x));
            if (x == parent(header_))  // x是根节点
                set_parent(header_, y);
            else if (x == right(parent(x)))  // x是右子节点
                set_right(parent(x), y);
            else  // x是左子节点
                set_left(parent(x), y);
            set_right(y, x);
            set_parent(x, y);
        }

        // 插入节点后的调整
        void rb_tree_insert_rebalance(rb_tree_node* x) noexcept
        {
            x->color = rb_tree_red;  // 新插入的节点为红色
            
            while (x != parent(header_) && parent(x)->color == rb_tree_red)
            {
                if (parent(x) == left(parent(parent(x))))  // 父节点是祖父节点的左子节点
                {
                    rb_tree_node* y = right(parent(parent(x)));  // 叔叔节点
                    if (y && y->color == rb_tree_red)  // case 1: 叔叔节点为红色
                    {
                        parent(x)->color = rb_tree_black;
                        y->color = rb_tree_black;
                        parent(parent(x))->color = rb_tree_red;
                        x = parent(parent(x));
                    }
                    else  // case 2/3: 叔叔节点为黑色
                    {
                        if (x == right(parent(x)))  // case 2: 当前节点是右子节点
                        {
                            x = parent(x);
                            rb_tree_rotate_left(x);
                        }
                        // case 3: 当前节点是左子节点
                        parent(x)->color = rb_tree_black;
                        parent(parent(x))->color = rb_tree_red;
                        rb_tree_rotate_right(parent(parent(x)));
                    }
                }
                else  // 父节点是祖父节点的右子节点，对称处理
                {
                    rb_tree_node* y = left(parent(parent(x)));  // 叔叔节点
                    if (y && y->color == rb_tree_red)  // case 1: 叔叔节点为红色
                    {
                        parent(x)->color = rb_tree_black;
                        y->color = rb_tree_black;
                        parent(parent(x))->color = rb_tree_red;
                        x = parent(parent(x));
                    }
                    else  // case 2/3: 叔叔节点为黑色
                    {
                        if (x == left(parent(x)))  // case 2: 当前节点是左子节点
                        {
                            x = parent(x);
                            rb_tree_rotate_right(x);
                        }
                        // case 3: 当前节点是右子节点
                        parent(x)->color = rb_tree_black;
                        parent(parent(x))->color = rb_tree_red;
                        rb_tree_rotate_left(parent(parent(x)));
                    }
                }
            }
            parent(header_)->color = rb_tree_black;  // 根节点始终为黑色
        }

        // 查找合适的插入位置
        rb_tree_node* get_insert_pos(const value_type& value)
        {
            rb_tree_node* y = header_;
            rb_tree_node* x = parent(header_);  // 从根节点开始
            
            while (x != nullptr)
            {
                y = x;
                x = key_compare(value, x->value) ? left(x) : right(x);
            }
            
            return y;
        }

        // 递归复制红黑树
        rb_tree_node* copy_tree(rb_tree_node* x, rb_tree_node* p)
        {
            rb_tree_node* top = clone_node(x);
            top->parent = p;

            try 
            {
                if (x->right)
                    top->right = copy_tree(right(x), top);
                if (x->left)
                    top->left = copy_tree(left(x), top);
            }
            catch (...) 
            {
                clear_tree(top);
                throw;
            }
            return top;
        }

        // 递归删除红黑树
        void clear_tree(rb_tree_node* x)
        {
            while (x != nullptr)
            {
                clear_tree(right(x));  // 递归清理右子树
                rb_tree_node* y = left(x);  // 保存左子树
                destroy_node(x);  // 销毁当前节点
                x = y;  // 继续处理左子树
            }
        }

        // 插入节点的辅助函数
        iterator insert_aux(rb_tree_node* y, const value_type& value, bool add_left)
        {
            rb_tree_node* z = create_node(value);
            
            if (y == header_ || add_left)  // 插入第一个节点或作为左子节点
            {
                set_left(y, z);
                if (y == header_)  // 第一个节点
                {
                    set_parent(header_, z);
                    set_right(header_, z);
                }
                else if (y == left(header_))  // 更新最小节点
                    set_left(header_, z);
            }
            else  // 作为右子节点
            {
                set_right(y, z);
                if (y == right(header_))  // 更新最大节点
                    set_right(header_, z);
            }
            
            set_parent(z, y);
            rb_tree_insert_rebalance(z);
            ++node_count_;
            return iterator(z);
        }

        // 删除节点的辅助函数
        rb_tree_node_base* rb_tree_erase(rb_tree_node_base* z)
        {
            rb_tree_node_base* y = z;  // 实际要删除的节点
            rb_tree_node_base* x = nullptr;  // 替代节点
            rb_tree_node_base* x_parent = nullptr;

            if (y->left == nullptr)     // z的左子树为空
                x = y->right;
            else if (y->right == nullptr)  // z的右子树为空
                x = y->left;
            else 
            {  // z的左右子树都不为空
                y = y->right;
                while (y->left != nullptr)
                    y = y->left;
                x = y->right;
            }

            if (y != z) 
            {  // y是z的后继节点
                z->left->parent = y;
                y->left = z->left;

                if (y != z->right) 
                {
                    x_parent = y->parent;
                    if (x) x->parent = y->parent;
                    y->parent->left = x;
                    y->right = z->right;
                    z->right->parent = y;
                }
                else
                    x_parent = y;

                if (parent(header_) == z)
                    set_parent(header_, reinterpret_cast<rb_tree_node*>(y));
                else if (z->parent->left == z)
                    z->parent->left = y;
                else
                    z->parent->right = y;

                y->parent = z->parent;
                mystl::swap(y->color, z->color);
                y = z;
            }
            else 
            {  // y == z
                x_parent = y->parent;
                if (x) 
                    x->parent = y->parent;

                if (parent(header_) == z)
                    set_parent(header_, reinterpret_cast<rb_tree_node*>(x));
                else if (z->parent->left == z)
                    z->parent->left = x;
                else
                    z->parent->right = x;

                if (left(header_) == z)
                {
                    if (z->right == nullptr)
                        set_left(header_, reinterpret_cast<rb_tree_node*>(z->parent));
                    else
                        set_left(header_, minimum(reinterpret_cast<rb_tree_node*>(x)));
                }
                if (right(header_) == z)
                {
                    if (z->left == nullptr)
                        set_right(header_, reinterpret_cast<rb_tree_node*>(z->parent));
                    else
                        set_right(header_, maximum(reinterpret_cast<rb_tree_node*>(x)));
                }
            }

            if (y->color == rb_tree_black)
                rb_tree_erase_rebalance(x, x_parent);

            return y;
        }

        // 删除节点后的平衡调整
        void rb_tree_erase_rebalance(rb_tree_node_base* x, rb_tree_node_base* x_parent) noexcept
        {
            while (x != parent(header_) && (x == nullptr || x->color == rb_tree_black))
            {
                if (x == x_parent->left) 
                {  // x是左子节点
                    rb_tree_node_base* w = x_parent->right;
                    if (w->color == rb_tree_red) 
                    {
                        w->color = rb_tree_black;
                        x_parent->color = rb_tree_red;
                        rb_tree_rotate_left(static_cast<rb_tree_node*>(x_parent));
                        w = x_parent->right;
                    }
                    if ((w->left == nullptr || w->left->color == rb_tree_black) &&
                        (w->right == nullptr || w->right->color == rb_tree_black)) 
                    {
                        w->color = rb_tree_red;
                        x = x_parent;
                        x_parent = x_parent->parent;
                    }
                    else 
                    {
                        if (w->right == nullptr || w->right->color == rb_tree_black) 
                        {
                            if (w->left) w->left->color = rb_tree_black;
                            w->color = rb_tree_red;
                            rb_tree_rotate_right(static_cast<rb_tree_node*>(w));
                            w = x_parent->right;
                        }
                        w->color = x_parent->color;
                        x_parent->color = rb_tree_black;
                        if (w->right) w->right->color = rb_tree_black;
                        rb_tree_rotate_left(static_cast<rb_tree_node*>(x_parent));
                        break;
                    }
                }
                else 
                {  // x是右子节点
                    rb_tree_node_base* w = x_parent->left;
                    if (w->color == rb_tree_red) 
                    {
                        w->color = rb_tree_black;
                        x_parent->color = rb_tree_red;
                        rb_tree_rotate_right(static_cast<rb_tree_node*>(x_parent));
                        w = x_parent->left;
                    }
                    if ((w->right == nullptr || w->right->color == rb_tree_black) &&
                        (w->left == nullptr || w->left->color == rb_tree_black)) 
                    {
                        w->color = rb_tree_red;
                        x = x_parent;
                        x_parent = x_parent->parent;
                    }
                    else 
                    {
                        if (w->left == nullptr || w->left->color == rb_tree_black) 
                        {
                            if (w->right) w->right->color = rb_tree_black;
                            w->color = rb_tree_red;
                            rb_tree_rotate_left(static_cast<rb_tree_node*>(w));
                            w = x_parent->left;
                        }
                        w->color = x_parent->color;
                        x_parent->color = rb_tree_black;
                        if (w->left) w->left->color = rb_tree_black;
                        rb_tree_rotate_right(static_cast<rb_tree_node*>(x_parent));
                        break;
                    }
                }
            }
            if (x) x->color = rb_tree_black;
        }

        // ... 其他辅助函数
    };

} // namespace mystl 