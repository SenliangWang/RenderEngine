# 头文件
set(HAZEL_HEADERS
    src/Hazel/Core/Application.h
    src/Hazel/Core/Log.h
    src/Hazel/Core/KeyCodes.h
    src/Hazel/Core/Events/Event.h
    src/Hazel/Core/Events/KeyEvent.h
    src/Hazel/Core/Events/MouseEvent.h
    src/Hazel/Core/Events/MouseEvent.h
    src/Hazel/Core/Window.h
    hzpch.h
    Hazel.h
)

# 源文件
set(HAZEL_SOURCES 
    src/Hazel/Core/Application.cpp
    src/Hazel/Core/Log.cpp
    src/Hazel/Core/Window.cpp

    src/Hazel/Platform/WindowsWindow.h
    src/Hazel/Platform/WindowsWindow.cpp
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

# 添加spdlog库
add_subdirectory(vendor/spdlog)

# 在目标中链接spdlog
target_link_libraries(Hazel PUBLIC spdlog::spdlog)

add_subdirectory(vendor/glfw)

# 设置GLFW选项
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Don't build the GLFW examples")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Don't build the GLFW tests")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Don't build the GLFW docs")
set(GLFW_INSTALL OFF CACHE BOOL "Don't generate install target")

# 链接GLFW库
target_link_libraries(Hazel PUBLIC glfw)

# 查找OpenGL库
find_package(OpenGL REQUIRED)

# 链接OpenGL库
target_link_libraries(Hazel PUBLIC OpenGL::GL)
