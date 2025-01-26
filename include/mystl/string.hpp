#pragma once
#include "allocator.hpp"
#include "iterator.hpp"
#include <cstring>
#include <stdexcept>

namespace mystl 
{
    class string 
    {
    public:
        //------------------------------------------------------------------------------
        // 类型定义
        //------------------------------------------------------------------------------
        using value_type = char;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using reference = char&;
        using const_reference = const char&;
        using pointer = char*;
        using const_pointer = const char*;
        using iterator = char*;
        using const_iterator = const char*;
        using reverse_iterator = mystl::reverse_iterator<iterator>;
        using const_reverse_iterator = mystl::reverse_iterator<const_iterator>;
        using allocator_type = mystl::allocator<char>;

        static constexpr size_type npos = static_cast<size_type>(-1);

    private:
        pointer data_;        // 字符串数据
        size_type size_;      // 字符串长度
        size_type capacity_;  // 容量
        allocator_type alloc_;

    public:
        //------------------------------------------------------------------------------
        // 构造/析构函数
        //------------------------------------------------------------------------------
        
        // 默认构造函数
        string() noexcept
            : data_(nullptr), size_(0), capacity_(0) 
        {
            data_ = alloc_.allocate(1);
            data_[0] = '\0';
            capacity_ = 1;
        }

        // 使用C风格字符串构造
        string(const char* str)
            : data_(nullptr), size_(0), capacity_(0)
        {
            size_ = strlen(str);
            capacity_ = size_ + 1;
            data_ = alloc_.allocate(capacity_);
            memcpy(data_, str, size_);
            data_[size_] = '\0';
        }

        // 使用C风格字符串的前count个字符构造
        string(const char* str, size_type count)
            : data_(nullptr), size_(0), capacity_(0)
        {
            size_ = count;
            capacity_ = size_ + 1;
            data_ = alloc_.allocate(capacity_);
            memcpy(data_, str, size_);
            data_[size_] = '\0';
        }

        // 构造包含count个字符ch的字符串
        string(size_type count, char ch)
            : data_(nullptr), size_(0), capacity_(0)
        {
            size_ = count;
            capacity_ = size_ + 1;
            data_ = alloc_.allocate(capacity_);
            memset(data_, ch, size_);
            data_[size_] = '\0';
        }

        // 使用迭代器范围构造
        template <class InputIt>
        string(InputIt first, InputIt last)
            : data_(nullptr), size_(0), capacity_(0)
        {
            size_ = mystl::distance(first, last);
            capacity_ = size_ + 1;
            data_ = alloc_.allocate(capacity_);
            for (size_type i = 0; first != last; ++first, ++i)
                data_[i] = *first;
            data_[size_] = '\0';
        }

        // 拷贝构造函数
        string(const string& other)
            : data_(nullptr), size_(0), capacity_(0)
        {
            size_ = other.size_;
            capacity_ = size_ + 1;
            data_ = alloc_.allocate(capacity_);
            memcpy(data_, other.data_, size_);
            data_[size_] = '\0';
        }

        // 移动构造函数
        string(string&& other) noexcept
            : data_(other.data_), size_(other.size_), capacity_(other.capacity_)
        {
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }

        // 析构函数
        ~string() 
        {
            if (data_)
                alloc_.deallocate(data_, capacity_);
        }

        //------------------------------------------------------------------------------
        // 赋值操作
        //------------------------------------------------------------------------------
        
        string& operator=(const string& other) 
        {
            if (this != &other) 
            {
                if (capacity_ < other.size_ + 1) 
                {
                    alloc_.deallocate(data_, capacity_);
                    capacity_ = other.size_ + 1;
                    data_ = alloc_.allocate(capacity_);
                }
                size_ = other.size_;
                memcpy(data_, other.data_, size_);
                data_[size_] = '\0';
            }
            return *this;
        }

        string& operator=(string&& other) noexcept 
        {
            if (this != &other) 
            {
                alloc_.deallocate(data_, capacity_);
                data_ = other.data_;
                size_ = other.size_;
                capacity_ = other.capacity_;
                other.data_ = nullptr;
                other.size_ = 0;
                other.capacity_ = 0;
            }
            return *this;
        }

        string& operator=(const char* str)
        {
            size_type len = strlen(str);
            if (capacity_ < len + 1) 
            {
                alloc_.deallocate(data_, capacity_);
                capacity_ = len + 1;
                data_ = alloc_.allocate(capacity_);
            }
            size_ = len;
            memcpy(data_, str, size_);
            data_[size_] = '\0';
            return *this;
        }

        string& operator=(char ch)
        {
            if (capacity_ < 2) 
            {
                alloc_.deallocate(data_, capacity_);
                capacity_ = 2;
                data_ = alloc_.allocate(capacity_);
            }
            size_ = 1;
            data_[0] = ch;
            data_[1] = '\0';
            return *this;
        }

        string& assign(const string& str)
        {
            return *this = str;
        }

        string& assign(const char* str)
        {
            return *this = str;
        }

        string& assign(const char* str, size_type count)
        {
            if (capacity_ <= count)
            {
                alloc_.deallocate(data_, capacity_);
                capacity_ = count + 1;
                data_ = alloc_.allocate(capacity_);
            }
            size_ = count;
            memcpy(data_, str, count);
            data_[count] = '\0';
            return *this;
        }

        string& assign(size_type count, char ch)
        {
            if (capacity_ <= count)
            {
                alloc_.deallocate(data_, capacity_);
                capacity_ = count + 1;
                data_ = alloc_.allocate(capacity_);
            }
            size_ = count;
            memset(data_, ch, count);
            data_[count] = '\0';
            return *this;
        }

        //------------------------------------------------------------------------------
        // 迭代器
        //------------------------------------------------------------------------------
        
        iterator begin() noexcept { return data_; }
        const_iterator begin() const noexcept { return data_; }
        iterator end() noexcept { return data_ + size_; }
        const_iterator end() const noexcept { return data_ + size_; }
        reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
        const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
        reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
        const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
        const_iterator cbegin() const noexcept { return begin(); }
        const_iterator cend() const noexcept { return end(); }
        const_reverse_iterator crbegin() const noexcept { return rbegin(); }
        const_reverse_iterator crend() const noexcept { return rend(); }

        //------------------------------------------------------------------------------
        // 容量操作
        //------------------------------------------------------------------------------
        
        size_type size() const noexcept { return size_; }
        size_type length() const noexcept { return size_; }
        bool empty() const noexcept { return size_ == 0; }
        size_type capacity() const noexcept { return capacity_ - 1; }
        
        void shrink_to_fit()
        {
            if (capacity_ > size_ + 1)
            {
                pointer new_data = alloc_.allocate(size_ + 1);
                memcpy(new_data, data_, size_);
                new_data[size_] = '\0';
                alloc_.deallocate(data_, capacity_);
                data_ = new_data;
                capacity_ = size_ + 1;
            }
        }

        void reserve(size_type new_cap) 
        {
            if (new_cap <= capacity_ - 1) return;
            pointer new_data = alloc_.allocate(new_cap + 1);
            memcpy(new_data, data_, size_);
            new_data[size_] = '\0';
            alloc_.deallocate(data_, capacity_);
            data_ = new_data;
            capacity_ = new_cap + 1;
        }

        //------------------------------------------------------------------------------
        // 元素访问
        //------------------------------------------------------------------------------
        
        reference operator[](size_type pos) { return data_[pos]; }
        const_reference operator[](size_type pos) const { return data_[pos]; }
        
        reference at(size_type pos) 
        {
            if (pos >= size_)
                throw std::out_of_range("string::at");
            return data_[pos];
        }
        
        const_reference at(size_type pos) const 
        {
            if (pos >= size_)
                throw std::out_of_range("string::at");
            return data_[pos];
        }
        
        reference front() { return data_[0]; }
        const_reference front() const { return data_[0]; }
        reference back() { return data_[size_ - 1]; }
        const_reference back() const { return data_[size_ - 1]; }
        const char* c_str() const noexcept { return data_; }
        const char* data() const noexcept { return data_; }

        //------------------------------------------------------------------------------
        // 修改器
        //------------------------------------------------------------------------------
        
        void clear() noexcept 
        {
            size_ = 0;
            data_[0] = '\0';
        }

        string& insert(size_type pos, const string& str)
        {
            return insert(pos, str.data_, str.size_);
        }

        string& insert(size_type pos, const char* str)
        {
            return insert(pos, str, strlen(str));
        }

        string& insert(size_type pos, const char* str, size_type count)
        {
            if (pos > size_)
                throw std::out_of_range("string::insert");

            if (size_ + count >= capacity_)
                reserve((size_ + count) * 2);

            memmove(data_ + pos + count, data_ + pos, size_ - pos);
            memcpy(data_ + pos, str, count);
            size_ += count;
            data_[size_] = '\0';
            return *this;
        }

        string& erase(size_type pos = 0, size_type count = npos)
        {
            if (pos > size_)
                throw std::out_of_range("string::erase");

            if (count == npos || pos + count > size_)
                count = size_ - pos;

            memmove(data_ + pos, data_ + pos + count, size_ - pos - count);
            size_ -= count;
            data_[size_] = '\0';
            return *this;
        }

        void push_back(char ch) 
        {
            if (size_ + 1 >= capacity_)
                reserve(capacity_ * 2);
            data_[size_++] = ch;
            data_[size_] = '\0';
        }

        void pop_back() 
        {
            if (!empty())
            {
                --size_;
                data_[size_] = '\0';
            }
        }

        string& append(const string& str)
        {
            return append(str.data_, str.size_);
        }

        string& append(const char* str)
        {
            return append(str, strlen(str));
        }

        string& append(const char* str, size_type count)
        {
            if (size_ + count >= capacity_)
                reserve((size_ + count) * 2);
            memcpy(data_ + size_, str, count);
            size_ += count;
            data_[size_] = '\0';
            return *this;
        }

        string& operator+=(const string& str) { return append(str); }
        string& operator+=(const char* str) { return append(str); }
        string& operator+=(char ch) { push_back(ch); return *this; }

        string& replace(size_type pos, size_type count, const string& str)
        {
            return replace(pos, count, str.data_, str.size_);
        }

        string& replace(size_type pos, size_type count, const char* str)
        {
            return replace(pos, count, str, strlen(str));
        }

        string& replace(size_type pos, size_type count, const char* str, size_type count2)
        {
            if (pos > size_)
                throw std::out_of_range("string::replace");

            count = std::min(count, size_ - pos);
            size_type new_size = size_ - count + count2;

            if (new_size > capacity_ - 1)
                reserve(new_size * 2);

            if (count != count2)
                memmove(data_ + pos + count2, data_ + pos + count, size_ - pos - count);

            memcpy(data_ + pos, str, count2);
            size_ = new_size;
            data_[size_] = '\0';
            return *this;
        }

        void swap(string& other) noexcept
        {
            mystl::swap(data_, other.data_);
            mystl::swap(size_, other.size_);
            mystl::swap(capacity_, other.capacity_);
        }

        //------------------------------------------------------------------------------
        // 字符串操作
        //------------------------------------------------------------------------------
        
        size_type find(const string& str, size_type pos = 0) const noexcept
        {
            return find(str.data_, pos, str.size_);
        }

        size_type find(const char* str, size_type pos = 0) const noexcept
        {
            return find(str, pos, strlen(str));
        }

        size_type find(const char* str, size_type pos, size_type count) const noexcept
        {
            if (pos > size_ || count > size_ - pos)
                return npos;

            if (count == 0)
                return pos;

            for (size_type i = pos; i <= size_ - count; ++i)
            {
                if (memcmp(data_ + i, str, count) == 0)
                    return i;
            }
            return npos;
        }

        size_type find(char ch, size_type pos = 0) const noexcept
        {
            for (size_type i = pos; i < size_; ++i)
            {
                if (data_[i] == ch)
                    return i;
            }
            return npos;
        }

        size_type rfind(const string& str, size_type pos = npos) const noexcept
        {
            return rfind(str.data_, pos, str.size_);
        }

        size_type rfind(const char* str, size_type pos = npos) const noexcept
        {
            return rfind(str, pos, strlen(str));
        }

        size_type rfind(const char* str, size_type pos, size_type count) const noexcept
        {
            if (size_ < count)
                return npos;

            if (count == 0)
                return std::min(pos, size_);

            pos = std::min(pos, size_ - count);

            for (auto i = pos + 1; i-- > 0;)
            {
                if (memcmp(data_ + i, str, count) == 0)
                    return i;
            }
            return npos;
        }

        size_type rfind(char ch, size_type pos = npos) const noexcept
        {
            if (size_ == 0)
                return npos;

            pos = std::min(pos, size_ - 1);
            for (auto i = pos + 1; i-- > 0;)
            {
                if (data_[i] == ch)
                    return i;
            }
            return npos;
        }

        size_type find_first_of(const string& str, size_type pos = 0) const noexcept
        {
            return find_first_of(str.data_, pos, str.size_);
        }

        size_type find_first_of(const char* str, size_type pos = 0) const noexcept
        {
            return find_first_of(str, pos, strlen(str));
        }

        size_type find_first_of(const char* str, size_type pos, size_type count) const noexcept
        {
            for (size_type i = pos; i < size_; ++i)
            {
                if (memchr(str, data_[i], count))
                    return i;
            }
            return npos;
        }

        size_type find_last_of(const string& str, size_type pos = npos) const noexcept
        {
            return find_last_of(str.data_, pos, str.size_);
        }

        size_type find_last_of(const char* str, size_type pos = npos) const noexcept
        {
            return find_last_of(str, pos, strlen(str));
        }

        size_type find_last_of(const char* str, size_type pos, size_type count) const noexcept
        {
            if (size_ == 0)
                return npos;

            pos = std::min(pos, size_ - 1);

            for (auto i = pos + 1; i-- > 0;)
            {
                if (memchr(str, data_[i], count))
                    return i;
            }
            return npos;
        }

        size_type find_first_not_of(const string& str, size_type pos = 0) const noexcept
        {
            return find_first_not_of(str.data_, pos, str.size_);
        }

        size_type find_first_not_of(const char* str, size_type pos = 0) const noexcept
        {
            return find_first_not_of(str, pos, strlen(str));
        }

        size_type find_first_not_of(const char* str, size_type pos, size_type count) const noexcept
        {
            for (size_type i = pos; i < size_; ++i)
            {
                if (!memchr(str, data_[i], count))
                    return i;
            }
            return npos;
        }

        size_type find_last_not_of(const string& str, size_type pos = npos) const noexcept
        {
            return find_last_not_of(str.data_, pos, str.size_);
        }

        size_type find_last_not_of(const char* str, size_type pos = npos) const noexcept
        {
            return find_last_not_of(str, pos, strlen(str));
        }

        size_type find_last_not_of(const char* str, size_type pos, size_type count) const noexcept
        {
            if (size_ == 0)
                return npos;

            pos = std::min(pos, size_ - 1);

            for (auto i = pos + 1; i-- > 0;)
            {
                if (!memchr(str, data_[i], count))
                    return i;
            }
            return npos;
        }

        string substr(size_type pos = 0, size_type count = npos) const
        {
            if (pos > size_)
                throw std::out_of_range("string::substr");

            if (count == npos || pos + count > size_)
                count = size_ - pos;

            return string(data_ + pos, count);
        }

        //------------------------------------------------------------------------------
        // 比较操作
        //------------------------------------------------------------------------------
        
        int compare(const string& str) const noexcept
        {
            return compare(str.data_);
        }

        int compare(const char* str) const noexcept
        {
            return strcmp(data_, str);
        }

        bool operator==(const string& rhs) const noexcept 
        {
            return size_ == rhs.size_ && memcmp(data_, rhs.data_, size_) == 0;
        }

        bool operator!=(const string& rhs) const noexcept
        {
            return !(*this == rhs);
        }

        bool operator<(const string& rhs) const noexcept 
        {
            return strcmp(data_, rhs.data_) < 0;
        }

        bool operator<=(const string& rhs) const noexcept
        {
            return !(rhs < *this);
        }

        bool operator>(const string& rhs) const noexcept
        {
            return rhs < *this;
        }

        bool operator>=(const string& rhs) const noexcept
        {
            return !(*this < rhs);
        }
    };

    //------------------------------------------------------------------------------
    // 非成员函数
    //------------------------------------------------------------------------------
    
    inline string operator+(const string& lhs, const string& rhs)
    {
        string result(lhs);
        result += rhs;
        return result;
    }

    inline string operator+(const string& lhs, const char* rhs)
    {
        string result(lhs);
        result += rhs;
        return result;
    }

    inline string operator+(const char* lhs, const string& rhs)
    {
        string result(lhs);
        result += rhs;
        return result;
    }

    template<>
    struct hash<string> 
    {
        size_t operator()(const string& str) const noexcept 
        {
            size_t hash = 5381;
            for (auto c : str)
                hash = ((hash << 5) + hash) + c;
            return hash;
        }
    };

    inline void swap(string& lhs, string& rhs) noexcept
    {
        lhs.swap(rhs);
    }

} // namespace mystl 