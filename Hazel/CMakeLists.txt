# 头文件
set(HAZEL_HEADERS
    src/Hazel/Core/Application.h
    src/Hazel/Core/Log.h
    src/Hazel/EntryPoint.h
    hzpch.h
    Hazel.h
)

# 源文件
set(HAZEL_SOURCES 
    src/Hazel/Core/Application.cpp
    src/Hazel/Core/Log.cpp
    hzpch.cpp
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

    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog/include

)

# 链接库
target_link_libraries(Hazel
    # 添加外部依赖库
)

# 添加spdlog库
add_subdirectory(vendor/spdlog)

# 在目标中链接spdlog
target_link_libraries(Hazel PUBLIC spdlog::spdlog)
