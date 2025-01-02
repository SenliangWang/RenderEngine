# glad库的CMake配置
set(GLAD_SOURCES
    src/glad.c
)

add_library(glad STATIC ${GLAD_SOURCES})

# 包含glad头文件
target_include_directories(glad PUBLIC include)