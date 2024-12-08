# 源文件
set(SANDBOX_SOURCES
    SandBox.cpp
)

# 添加可执行文件
add_executable(Sandbox ${SANDBOX_SOURCES})

# 链接Hazel库
target_link_libraries(Sandbox PRIVATE Hazel)

# 为 Sandbox 目标添加编译选项
target_compile_options(Sandbox PRIVATE
    /utf-8  # 启用 UTF-8 编码支持
)

# 包含目录
target_include_directories(Sandbox PRIVATE
    ${CMAKE_SOURCE_DIR}/../Hazel/src
    ${CMAKE_SOURCE_DIR}/../Hazel
)

# 设置输出目录
set_target_properties(Sandbox PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
