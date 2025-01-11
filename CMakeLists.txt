cmake_minimum_required(VERSION 3.14)
project(mystl)

# 启用测试
enable_testing()

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加 Google Test
add_subdirectory("C:/Users/IMikoto/Documents/vscode_files/c++/googletest" googletest)

# 添加头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)

# 设置库的输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 添加子目录
add_subdirectory(src)
add_subdirectory(test) 