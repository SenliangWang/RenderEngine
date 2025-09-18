// freeglut + OpenGL 固定功能管线 纹理映射 Demo
// 在一个旋转的四边形上显示程序生成的棋盘纹理

#include <GL/freeglut.h>
#include <cmath>
#include <vector>

namespace {
    GLuint g_textureId = 0;
    float g_angleDeg = 0.0f;
    int g_winWidth = 800;
    int g_winHeight = 600;

    void createCheckerTexture(int width, int height, int checkSize)
    {
        std::vector<unsigned char> pixels(width * height * 3, 0);

        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                int cx = (x / checkSize) & 1;
                int cy = (y / checkSize) & 1;
                unsigned char c = static_cast<unsigned char>((cx ^ cy) ? 255 : 30);
                int idx = (y * width + x) * 3;
                pixels[idx + 0] = c;      // R
                pixels[idx + 1] = c;      // G
                pixels[idx + 2] = 255;    // B (带点蓝色调便于观察调制效果)
            }
        }

        glGenTextures(1, &g_textureId);
        glBindTexture(GL_TEXTURE_2D, g_textureId);

        // 固定管线常用参数：过滤、环绕、环境模式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // 使用固定管线环境模式：调制（与片段主颜色相乘）
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        // 上传纹理数据（RGB 8位）
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGB,
            width,
            height,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            pixels.data());
    }

    void initGL()
    {
        glClearColor(0.1f, 0.12f, 0.15f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D); // 固定管线中的 2D 纹理开关

        createCheckerTexture(256, 256, 16);
    }

    void reshape(int w, int h)
    {
        g_winWidth = (w > 1) ? w : 1;
        g_winHeight = (h > 1) ? h : 1;

        glViewport(0, 0, g_winWidth, g_winHeight);

        // 使用固定管线投影矩阵；这里用简单的透视近似（不依赖 GLU）
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        float aspect = static_cast<float>(g_winWidth) / static_cast<float>(g_winHeight);
        // 近裁面 1.5，远裁面 100，视锥左右上下按纵向 -1..1 缩放到宽高比
        glFrustum(-aspect, aspect, -1.0, 1.0, 1.5, 100.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    void drawTexturedQuad()
    {
        const float s = 1.0f; // 半尺寸
        const float repeat = 2.0f; // 纹理坐标重复两次

        glBegin(GL_QUADS);
            glColor3f(1.0f, 1.0f, 1.0f); // 与 GL_MODULATE 配合，保持原纹理颜色

            glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, -s, 0.0f);
            glTexCoord2f(repeat, 0.0f); glVertex3f( s, -s, 0.0f);
            glTexCoord2f(repeat, repeat); glVertex3f( s,  s, 0.0f);
            glTexCoord2f(0.0f, repeat); glVertex3f(-s,  s, 0.0f);
        glEnd();
    }

    void display()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glLoadIdentity();
        glTranslatef(0.0f, 0.0f, -4.0f);
        glRotatef(g_angleDeg, 0.0f, 1.0f, 0.0f);
        glRotatef(g_angleDeg * 0.5f, 1.0f, 0.0f, 0.0f);

        glBindTexture(GL_TEXTURE_2D, g_textureId);
        drawTexturedQuad();

        glutSwapBuffers();
    }

    void timer(int value)
    {
        (void)value;
        g_angleDeg += 0.5f;
        if (g_angleDeg >= 360.0f) g_angleDeg -= 360.0f;
        glutPostRedisplay();
        glutTimerFunc(16, timer, 0); // ~60 FPS
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(g_winWidth, g_winHeight);
    glutCreateWindow("FreeGLUT Fixed-Function Texture Demo");

    initGL();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(16, timer, 0);

    glutMainLoop();
    return 0;
}

