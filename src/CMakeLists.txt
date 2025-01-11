# 创建头文件库
add_library(mystl INTERFACE)
target_include_directories(mystl INTERFACE ${PROJECT_SOURCE_DIR}/include) 