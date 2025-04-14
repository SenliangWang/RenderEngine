## Core
set(CORE_HEADERS
    src/Hazel/Core/Application.h
    src/Hazel/Core/Log.h
    src/Hazel/Core/Base.h
    src/Hazel/Core/Timestep.h

    src/Hazel/Core/KeyCodes.h
    src/Hazel/Core/Events/Event.h
    src/Hazel/Core/Events/KeyEvent.h
    src/Hazel/Core/Events/MouseEvent.h
    src/Hazel/Core/Events/ApplicationEvent.h


    src/Hazel/Core/Window.h
    src/Hazel/Core/Layer.h
)

set(CORE_SOURCES
    src/Hazel/Core/Application.cpp
    src/Hazel/Core/Log.cpp

    src/Hazel/Core/Window.cpp
    src/Hazel/Core/Layer.cpp
    src/Hazel/Core/LayerStack.cpp

    src/Hazel/Core/Input.h
)

## ImGui
set(IMGUI_SOURCES
    src/Hazel/Imgui/ImGuiLayer.h
    src/Hazel/Imgui/ImGuiLayer.cpp
)

## Platform
set(PLATFORM_SOURCES
    src/Hazel/Platform/WindowsWindow.h
    src/Hazel/Platform/WindowsWindow.cpp
    src/Hazel/Platform/WindowsInput.h
    src/Hazel/Platform/WindowsInput.cpp
)

set(PLATFORM_OPENGL_SOURCES
    src/Hazel/Platform/OpenGL/ImGuiOpenGLRenderer.h
    src/Hazel/Platform/OpenGL/ImGuiOpenGLRenderer.cpp
    
    src/Hazel/Platform/OpenGL/OpenGLContext.h
    src/Hazel/Platform/OpenGL/OpenGLContext.cpp

    src/Hazel/Platform/OpenGL/OpenGLBuffer.h
    src/Hazel/Platform/OpenGL/OpenGLBuffer.cpp

    src/Hazel/Platform/OpenGL/OpenGLVertexArray.h
    src/Hazel/Platform/OpenGL/OpenGLVertexArray.cpp

    src/Hazel/Platform/OpenGL/OpenGLRendererAPI.h
    src/Hazel/Platform/OpenGL/OpenGLRendererAPI.cpp

    src/Hazel/Platform/OpenGL/OpenGLShader.h
    src/Hazel/Platform/OpenGL/OpenGLShader.cpp

    src/Hazel/Platform/OpenGL/OpenGLTexture.h
    src/Hazel/Platform/OpenGL/OpenGLTexture.cpp
)

## Renderer
set(RENDERER_SOURCES
    src/Hazel/Renderer/GraphicsContext.h

    src/Hazel/Renderer/Shader.h
    src/Hazel/Renderer/Shader.cpp

    src/Hazel/Renderer/Buffer.h
    src/Hazel/Renderer/Buffer.cpp

    src/Hazel/Renderer/Renderer.h
    src/Hazel/Renderer/Renderer.cpp
    
    src/Hazel/Renderer/VertexArray.h
    src/Hazel/Renderer/VertexArray.cpp

    src/Hazel/Renderer/RenderCommand.h
    src/Hazel/Renderer/RenderCommand.cpp

    src/Hazel/Renderer/RendererAPI.h
    src/Hazel/Renderer/RendererAPI.cpp 

    src/Hazel/Renderer/OrthographicCamera.h
    src/Hazel/Renderer/OrthographicCamera.cpp

    src/Hazel/Renderer/Texture.h
    src/Hazel/Renderer/Texture.cpp
)

set(STB_IMAGE_SOURCES
    vendor/stb_image/stb_image.h
    vendor/stb_image/stb_image.cpp
)


## Misc
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
    ${PLATFORM_OPENGL_SOURCES}
    ${MISC_FILES}
    ${IMGUI_SOURCES}
    ${RENDERER_SOURCES}
    ${STB_IMAGE_SOURCES}
)

# 设置预编译头
target_precompile_headers(Hazel PRIVATE hzpch.h)

# 包含目录
target_include_directories(Hazel PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor
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

# 添加glad库
add_subdirectory(vendor/glad)

# 链接glad库
target_link_libraries(Hazel PUBLIC glad)

# 添加ImGui库
add_subdirectory(vendor/imgui)

# 链接ImGui库
target_link_libraries(Hazel PUBLIC imgui)

# 添加glm库
add_subdirectory(vendor/glm)

# 链接glm库
target_link_libraries(Hazel PUBLIC glm)

# 使用source_group组织文件
source_group("source\\core"             FILES ${CORE_HEADERS} ${CORE_SOURCES})
source_group("source\\platform"         FILES ${PLATFORM_SOURCES})
source_group("source\\platform\\opengl" FILES ${PLATFORM_OPENGL_SOURCES})
source_group("source\\renderer"         FILES ${RENDERER_SOURCES})
source_group("source\\Imgui"            FILES ${IMGUI_SOURCES})
source_group("source\\stb_image"        FILES ${STB_IMAGE_SOURCES})
source_group("source"                   FILES ${MISC_FILES})
