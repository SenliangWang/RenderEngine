FreeGLUT + OpenGL 固定管线 纹理映射 Demo

一个最小示例：使用 freeglut 和 OpenGL 兼容/固定功能管线，在旋转的四边形上显示程序化棋盘纹理。用到了典型的固定管线调用：`glEnable(GL_TEXTURE_2D)`、`glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE)`、`glTexCoord2f` 等。

依赖
- Linux（已安装编译工具链）
- freeglut（开发包）
- OpenGL 开发库

在基于 Debian/Ubuntu 的系统上：

```bash
sudo apt update
sudo apt install -y build-essential cmake freeglut3-dev
```

构建与运行
```bash
cd ffp_glut_texture
cmake -S . -B build
cmake --build build -j
./build/ffp_glut_texture
```

如果遇到链接线程库的问题，可在部分发行版上添加：`-pthread`，本 CMake 已自动处理。

关键点
- 使用固定管线开关与状态：`GL_TEXTURE_2D`、`GL_MODULATE`、过滤与环绕方式。
- 通过 `glTexCoord2f` 配合 `glBegin/glEnd` 指定纹理坐标。
- 未使用着色器或现代 OpenGL API，方便理解传统纹理映射流程。

