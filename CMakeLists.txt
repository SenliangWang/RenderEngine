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

# 启用断言
option(HZ_ENABLE_ASSERTS "Enable assertions" ON)

if(HZ_ENABLE_ASSERTS)
    add_definitions(-DHZ_ENABLE_ASSERTS)
endif()

# 添加子目录
add_subdirectory(Hazel)
add_subdirectory(Sandbox)
