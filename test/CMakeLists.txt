# 添加所有测试源文件
add_executable(mystl_test
    vector_test.cpp
    algorithm_test.cpp
    iterator_test.cpp
    allocator_test.cpp
    algorithm_base_test.cpp
    list_test.cpp
    deque_test.cpp
    queue_test.cpp
    rb_tree_test.cpp
    hashtable_test.cpp
    string_test.cpp
)

# 链接 Google Test 和我们的库
target_link_libraries(mystl_test
    PRIVATE
    mystl
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(mystl_test) 