cmake_minimum_required(VERSION 3.16)
project(ffp_glut_texture LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

add_executable(ffp_glut_texture
    src/main.cpp
)

target_link_libraries(ffp_glut_texture
    PRIVATE
        OpenGL::GL
        GLUT::GLUT
)

if (UNIX)
    # 一些发行版需要显式链接到 dl/pthread（GLUT 间接需要）
    find_package(Threads REQUIRED)
    target_link_libraries(ffp_glut_texture PRIVATE Threads::Threads)
endif ()

