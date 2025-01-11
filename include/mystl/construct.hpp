#pragma once

#include "type_traits.hpp"
#include "util.hpp"

namespace mystl 
{
    /*****************************************************************************************/
    // construct
    // 在已分配的内存上构造对象
    /*****************************************************************************************/
    template<class T, class... Args>
    void construct(T* p, Args&&... args)
    {
        ::new (static_cast<void*>(p)) T(mystl::forward<Args>(args)...);
    }

    

    /*****************************************************************************************/
    // destroy
    // 销毁对象
    /*****************************************************************************************/
    template<class T>
    void destroy_at(T* p) 
    {
        p->~T();
    }

    template<class ForwardIt>
    void destroy(ForwardIt first, ForwardIt last) 
    {
        for (; first != last; ++first)
        {
            destroy_at(addressof(*first));
        }
    }

    template<class ForwardIt, class Size>
    ForwardIt destroy_n(ForwardIt first, Size n) 
    {
        for (; n > 0; ++first, --n)
        {
            destroy_at(addressof(*first));
        }
        return first;
    }
} // namespace mystl 