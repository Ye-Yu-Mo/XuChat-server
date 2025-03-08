# 声明所需的cmake版本
cmake_minimum_required(VERSION 3.0.0)
# 定义项目工程名称
project(test)
# 设置生成目标
add_executable(main main.cc)

set(target "main")
set(src_files "")
list(APPEND src_files main.cc child.cc)
CMAKE_CURRENT_BINARY_DIR
CMAKE_CURRENT_SOURCE_DIR