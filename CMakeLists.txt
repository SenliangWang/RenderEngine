cmake_minimum_required(VERSION 3.10)

# 设置项目名称
project(Hazel)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows平台特定设置
if(WIN32)
    add_definitions(-DHZ_PLATFORM_WINDOWS)
endif()

# 编译选项
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# 添加子目录
add_subdirectory(Hazel)
add_subdirectory(Sandbox)
