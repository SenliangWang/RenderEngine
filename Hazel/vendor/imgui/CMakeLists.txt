set(IMGUI_SOURCES
    imgui.cpp
    imgui_draw.cpp
    imgui_demo.cpp
    imgui_widgets.cpp
    imgui_tables.cpp
    backends/imgui_impl_glfw.cpp
    backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})

target_include_directories(imgui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/backends
)

# 链接GLFW和OpenGL，因为ImGui的后端实现依赖于它们
target_link_libraries(imgui PUBLIC glfw OpenGL::GL) 