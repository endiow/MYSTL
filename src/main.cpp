#include <iostream>
#include "../include/mystl/vector.hpp"
#include "../include/mystl/list.hpp"
#include "../include/mystl/functional.hpp"
#include "../include/mystl/iterator.hpp"

void test_vector() 
{
    std::cout << "\n=== Testing vector ===\n";
    
    // 构造函数
    mystl::vector<int> vec;
    mystl::vector<int> vec2(5, 2);  // [2,2,2,2,2]
    
    // push_back
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    
    // 迭代器遍历
    std::cout << "vec: ";
    for (const auto& x : vec) 
    {
        std::cout << x << " ";
    }
    std::cout << "\n";
    
    // 访问元素
    std::cout << "front: " << vec.front() << ", back: " << vec.back() << "\n";
    std::cout << "vec[1]: " << vec[1] << "\n";
    
    // 容量操作
    std::cout << "size: " << vec.size() << ", capacity: " << vec.capacity() << "\n";
    
    // 修改元素
    vec[1] = 5;
    std::cout << "after modify, vec[1]: " << vec[1] << "\n";
}

void test_list() 
{
    std::cout << "\n=== Testing list ===\n";
    
    // 构造函数
    mystl::list<int> lst;
    mystl::list<int> lst2(3, 2);  // [2,2,2]
    
    // 添加元素
    lst.push_back(1);
    lst.push_back(2);
    lst.push_back(3);
    lst.push_front(0);
    
    // 遍历
    std::cout << "lst: ";
    for (const auto& x : lst) 
    {
        std::cout << x << " ";
    }
    std::cout << "\n";
    
    // 访问元素
    std::cout << "front: " << lst.front() << ", back: " << lst.back() << "\n";
    
    // 删除元素
    lst.pop_back();
    lst.pop_front();
    
    std::cout << "after pop, lst: ";
    for (const auto& x : lst) 
    {
        std::cout << x << " ";
    }
    std::cout << "\n";
    
    // 插入元素
    auto it = lst.begin();
    ++it;
    lst.insert(it, 5);
    
    std::cout << "after insert, lst: ";
    for (const auto& x : lst) 
    {
        std::cout << x << " ";
    }
    std::cout << "\n";
    
    // 链表算法
    lst.reverse();
    std::cout << "after reverse, lst: ";
    for (const auto& x : lst) 
    {
        std::cout << x << " ";
    }
    std::cout << "\n";
    
    // 合并有序链表
    mystl::list<int> lst3;
    lst3.push_back(4);
    lst3.push_back(6);
    lst3.push_back(8);
    
    lst.merge(lst3);
    std::cout << "after merge, lst: ";
    for (const auto& x : lst) 
    {
        std::cout << x << " ";
    }
    std::cout << "\n";
}

int main() 
{
    test_vector();
    test_list();
    return 0;
} 