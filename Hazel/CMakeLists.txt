set(CORE_HEADERS
    src/Hazel/Core/Application.h
    src/Hazel/Core/Log.h
    src/Hazel/Core/KeyCodes.h
    src/Hazel/Core/Events/Event.h
    src/Hazel/Core/Events/KeyEvent.h
    src/Hazel/Core/Events/MouseEvent.h
    src/Hazel/Core/Events/ApplicationEvent.h
    src/Hazel/Core/Window.h
)

set(CORE_SOURCES
    src/Hazel/Core/Application.cpp
    src/Hazel/Core/Log.cpp
    src/Hazel/Core/Window.cpp
)

set(PLATFORM_SOURCES
    src/Hazel/Platform/WindowsWindow.h
    src/Hazel/Platform/WindowsWindow.cpp
)

set(MISC_FILES
    src/Hazel/EntryPoint.h
    Hazel.h
    hzpch.h
    hzpch.cpp
)

# 添加静态库
add_library(Hazel STATIC 
    ${CORE_HEADERS}
    ${CORE_SOURCES}
    ${PLATFORM_SOURCES}
    ${MISC_FILES}
)

# 设置预编译头
target_precompile_headers(Hazel PRIVATE hzpch.h)

# 包含目录
target_include_directories(Hazel PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog/include
)

# 添加spdlog库
add_subdirectory(vendor/spdlog)
target_link_libraries(Hazel PUBLIC spdlog::spdlog)

# 添加和链接GLFW库
add_subdirectory(vendor/glfw)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Don't build the GLFW examples")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Don't build the GLFW tests")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Don't build the GLFW docs")
set(GLFW_INSTALL OFF CACHE BOOL "Don't generate install target")
target_link_libraries(Hazel PUBLIC glfw)

# 查找和链接OpenGL库
find_package(OpenGL REQUIRED)
target_link_libraries(Hazel PUBLIC OpenGL::GL)

# 使用source_group组织文件
source_group("source\\core"     FILES ${CORE_HEADERS} ${CORE_SOURCES})
source_group("source\\platform" FILES ${PLATFORM_SOURCES})
source_group("source"           FILES ${MISC_FILES})
