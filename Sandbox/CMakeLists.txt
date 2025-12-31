# 源文件
set(SANDBOX_SOURCES
    SandBox.cpp
)

# 添加可执行文件
add_executable(Sandbox ${SANDBOX_SOURCES})

# 链接Hazel库
target_link_libraries(Sandbox PRIVATE Hazel)

# 为 Sandbox 目标添加编译选项
if(MSVC)
    target_compile_options(Sandbox PRIVATE /utf-8)  # 启用 UTF-8 编码支持
else()
    target_compile_options(Sandbox PRIVATE -finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

# 包含目录
target_include_directories(Sandbox PRIVATE
    ${CMAKE_SOURCE_DIR}/../Hazel/src
    ${CMAKE_SOURCE_DIR}/../Hazel
)

# 设置输出目录
set_target_properties(Sandbox PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# 设定 Visual Studio 调试时的工作目录为源码中的 Sandbox 目录，
# 以便运行时可使用相对路径访问 `assets` 与贴图文件。
set_target_properties(Sandbox PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/Sandbox"
)