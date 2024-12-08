# 源文件
set(SANDBOX_SOURCES
    SandBox.cpp
)

# 添加可执行文件
add_executable(Sandbox ${SANDBOX_SOURCES})

# 链接Hazel库
target_link_libraries(Sandbox PRIVATE Hazel)

# 包含目录
target_include_directories(Sandbox PRIVATE
    ${CMAKE_SOURCE_DIR}/../Hazel/src
    ${CMAKE_SOURCE_DIR}/../Hazel
)

# 设置输出目录
set_target_properties(Sandbox PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
