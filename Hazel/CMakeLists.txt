# 源文件
set(HAZEL_SOURCES 
    src/Hazel/Core/Application.cpp
    hzpch.cpp
)

# 头文件
set(HAZEL_HEADERS
    src/Hazel/Core/Application.h
    src/Hazel/EntryPoint.h
    hzpch.h
    Hazel.h
)

# 添加静态库
add_library(Hazel STATIC 
    ${HAZEL_SOURCES}
    ${HAZEL_HEADERS}
)

# 包含目录
target_include_directories(Hazel PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 链接库
target_link_libraries(Hazel
    # 添加外部依赖库
)
