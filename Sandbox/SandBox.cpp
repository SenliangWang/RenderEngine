#include <Hazel.h>
#include <Hazel/Platform/OpenGL/OpenGLShader.h> // For UploadUniform
#include <Hazel/Renderer/Framebuffer.h>
#include <imgui.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h> // Native OpenGL calls

class ExampleLayer : public Hazel::Layer
{
public:
    ExampleLayer()
        : Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f)
    {
        m_VertexArray.reset(Hazel::VertexArray::Create());

        float vertices[3 * 7] = {
            -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
             0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
             0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
        };

        Hazel::Ref<Hazel::VertexBuffer> vertexBuffer;
        vertexBuffer.reset(Hazel::VertexBuffer::Create(vertices, sizeof(vertices)));
        Hazel::BufferLayout layout = {
            { Hazel::ShaderDataType::Float3, "a_Position" },
            { Hazel::ShaderDataType::Float4, "a_Color" }
        };
        vertexBuffer->SetLayout(layout);
        m_VertexArray->AddVertexBuffer(vertexBuffer);

        uint32_t indices[3] = { 0, 1, 2 };
        Hazel::Ref<Hazel::IndexBuffer> indexBuffer;
        indexBuffer.reset(Hazel::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
        m_VertexArray->SetIndexBuffer(indexBuffer);

        m_SquareVA.reset(Hazel::VertexArray::Create());

        float squareVertices[5 * 4] = {
            -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
             0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
             0.5f,  0.5f, 0.0f,  1.0f, 1.0f,
            -0.5f,  0.5f, 0.0f,  0.0f, 1.0f
        };

        Hazel::Ref<Hazel::VertexBuffer> squareVB;
        squareVB.reset(Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
        squareVB->SetLayout({
            { Hazel::ShaderDataType::Float3, "a_Position" }, 
            { Hazel::ShaderDataType::Float2, "a_TexCoord" }
        });
        m_SquareVA->AddVertexBuffer(squareVB);

        uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
        Hazel::Ref<Hazel::IndexBuffer> squareIB;
        squareIB.reset(Hazel::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
        m_SquareVA->SetIndexBuffer(squareIB);

        std::string vertexSrc = R"(
            #version 330 core
            
            layout(location = 0) in vec3 a_Position;
            layout(location = 1) in vec4 a_Color;

            uniform mat4 u_ViewProjection;
            uniform mat4 u_Transform;

            out vec3 v_Position;
            out vec4 v_Color;

            void main()
            {
                v_Position = a_Position;
                v_Color = a_Color;
                gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);    
            }
        )";

        std::string fragmentSrc = R"(
            #version 330 core
            
            layout(location = 0) out vec4 color;

            in vec3 v_Position;
            in vec4 v_Color;

            void main()
            {
                color = vec4(v_Position * 0.5 + 0.5, 1.0);
                color = v_Color;
            }
        )";

        m_Shader = Hazel::Shader::Create("VertexPosColor", vertexSrc, fragmentSrc);

        std::string flatColorShaderVertexSrc = R"(
            #version 330 core
            
            layout(location = 0) in vec3 a_Position;

            uniform mat4 u_ViewProjection;
            uniform mat4 u_Transform;

            out vec3 v_Position;

            void main()
            {
                v_Position = a_Position;
                gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);    
            }
        )";

        std::string flatColorShaderFragmentSrc = R"(
            #version 330 core
            
            layout(location = 0) out vec4 color;

            in vec3 v_Position;
            
            uniform vec3 u_Color;

            void main()
            {
                color = vec4(u_Color, 1.0);
            }
        )";

        m_FlatColorShader = Hazel::Shader::Create("FlatColor", flatColorShaderVertexSrc, flatColorShaderFragmentSrc);
        auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

        m_Texture = Hazel::Texture2D::Create("checkerBoard.png");
        m_ChernoLogoTexture = Hazel::Texture2D::Create("ChernoLogo.png");

        std::dynamic_pointer_cast<Hazel::OpenGLShader>(textureShader)->Bind();
        std::dynamic_pointer_cast<Hazel::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);
    }

    void OnUpdate(Hazel::Timestep ts) override
    {
        if (Hazel::Input::IsKeyPressed(HZ_KEY_LEFT))
            m_CameraPosition.x -= m_CameraMoveSpeed * ts;
        else if (Hazel::Input::IsKeyPressed(HZ_KEY_RIGHT))
            m_CameraPosition.x += m_CameraMoveSpeed * ts;

        if (Hazel::Input::IsKeyPressed(HZ_KEY_UP))
            m_CameraPosition.y += m_CameraMoveSpeed * ts;
        else if (Hazel::Input::IsKeyPressed(HZ_KEY_DOWN))
            m_CameraPosition.y -= m_CameraMoveSpeed * ts;

        if (Hazel::Input::IsKeyPressed(HZ_KEY_A))
            m_CameraRotation += m_CameraRotationSpeed * ts;
        if (Hazel::Input::IsKeyPressed(HZ_KEY_D))
            m_CameraRotation -= m_CameraRotationSpeed * ts;

        Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
        Hazel::RenderCommand::Clear();

        m_Camera.SetPosition(m_CameraPosition);
        m_Camera.SetRotation(m_CameraRotation);

        Hazel::Renderer::BeginScene(m_Camera);

        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

        std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->Bind();
        std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);

        for (int y = 0; y < 20; y++)
        {
            for (int x = 0; x < 20; x++)
            {
                glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
                glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
                Hazel::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
            }
        }

        auto textureShader = m_ShaderLibrary.Get("Texture");

        m_Texture->Bind();
        Hazel::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
        m_ChernoLogoTexture->Bind();
        Hazel::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

        // Triangle
        // Hazel::Renderer::Submit(m_Shader, m_VertexArray);


        Hazel::Renderer::EndScene();
    }

    virtual void OnImGuiRender()
    {
        ImGui::Begin("Settings");
        ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
        ImGui::End();
    }

    void OnEvent(Hazel::Event& event) override
    {
    }

private:
    Hazel::ShaderLibrary m_ShaderLibrary;
    Hazel::Ref<Hazel::Shader> m_Shader;
    Hazel::Ref<Hazel::VertexArray> m_VertexArray;

    Hazel::Ref<Hazel::Shader> m_FlatColorShader;
    Hazel::Ref<Hazel::VertexArray> m_SquareVA;

    Hazel::Ref<Hazel::Texture2D> m_Texture;
    Hazel::Ref<Hazel::Texture2D> m_ChernoLogoTexture;

    Hazel::OrthographicCamera m_Camera;
    glm::vec3 m_CameraPosition;
    float m_CameraMoveSpeed = 5.0f;

    float m_CameraRotation = 0.0f;
    float m_CameraRotationSpeed = 180.0f;

    glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };
};

class BrushLayer : public Hazel::Layer
{
public:
    BrushLayer() : Layer("Brush") {}

    virtual void OnAttach() override
    {
        // 1. 初始化 Framebuffer (画布)
        uint32_t width = Hazel::Application::Get().GetWindow().GetWidth();
        uint32_t height = Hazel::Application::Get().GetWindow().GetHeight();

        Hazel::FramebufferSpecification fbSpec;
        fbSpec.Width = width;
        fbSpec.Height = height;
        fbSpec.Attachments = { Hazel::FramebufferTextureFormat::RGBA8 };
        m_Framebuffer = Hazel::Framebuffer::Create(fbSpec);

        // 初始化画布为白色
        m_Framebuffer->Bind();
        Hazel::RenderCommand::SetClearColor({ 1.0f, 1.0f, 1.0f, 1.0f });
        Hazel::RenderCommand::Clear();
        m_Framebuffer->Unbind();

        // 2. 生成软笔刷纹理
        CreateBrushTexture(128);

        // 3. 创建几何体 (Quad)
        // 笔刷 Quad (-0.5 ~ 0.5)
        m_BrushVA.reset(Hazel::VertexArray::Create());
        float brushVertices[] = {
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
             0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
             0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
            -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
        };
        Hazel::Ref<Hazel::VertexBuffer> brushVB;
        brushVB.reset(Hazel::VertexBuffer::Create(brushVertices, sizeof(brushVertices)));
        brushVB->SetLayout({
            { Hazel::ShaderDataType::Float3, "a_Position" },
            { Hazel::ShaderDataType::Float2, "a_TexCoord" }
        });
        m_BrushVA->AddVertexBuffer(brushVB);
        
        uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };
        Hazel::Ref<Hazel::IndexBuffer> ib;
        ib.reset(Hazel::IndexBuffer::Create(indices, 6));
        m_BrushVA->SetIndexBuffer(ib);

        // 屏幕 Quad (NDC -1 ~ 1)
        m_ScreenVA.reset(Hazel::VertexArray::Create());
        float screenVertices[] = {
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f
        };
        Hazel::Ref<Hazel::VertexBuffer> screenVB;
        screenVB.reset(Hazel::VertexBuffer::Create(screenVertices, sizeof(screenVertices)));
        screenVB->SetLayout({
            { Hazel::ShaderDataType::Float3, "a_Position" },
            { Hazel::ShaderDataType::Float2, "a_TexCoord" }
        });
        m_ScreenVA->AddVertexBuffer(screenVB);
        m_ScreenVA->SetIndexBuffer(ib);

        // 4. 创建 Shaders
        std::string brushVs = R"(
            #version 330 core
            layout (location = 0) in vec3 aPos;
            layout (location = 1) in vec2 aTexCoord;
            out vec2 TexCoord;
            uniform mat4 projection;
            uniform vec2 offset;
            uniform float size;
            void main() {
                vec2 pos = aPos.xy * size + offset;
                gl_Position = projection * vec4(pos, 0.0, 1.0);
                TexCoord = aTexCoord;
            }
        )";

        std::string brushFs = R"(
            #version 330 core
            out vec4 FragColor;
            in vec2 TexCoord;
            uniform sampler2D brushTexture;
            uniform vec3 color;
            void main() {
                float alpha = texture(brushTexture, TexCoord).a; // 使用 Alpha 通道
                if(alpha < 0.01) discard;
                FragColor = vec4(color, alpha); 
            }
        )";

        m_BrushShader = Hazel::Shader::Create("Brush", brushVs, brushFs);

        std::string screenVs = R"(
            #version 330 core
            layout (location = 0) in vec3 aPos;
            layout (location = 1) in vec2 aTexCoord;
            out vec2 TexCoord;
            void main() {
                gl_Position = vec4(aPos, 1.0); 
                TexCoord = aTexCoord;
            }
        )";

        std::string screenFs = R"(
            #version 330 core
            out vec4 FragColor;
            in vec2 TexCoord;
            uniform sampler2D screenTexture;
            void main() {
                FragColor = texture(screenTexture, TexCoord);
            }
        )";

        m_ScreenShader = Hazel::Shader::Create("Screen", screenVs, screenFs);
    }

    virtual void OnUpdate(Hazel::Timestep ts) override
    {
        // 1. 输入处理
        if (Hazel::Input::IsMouseButtonPressed(0)) // Left Click
        {
            auto [x, y] = Hazel::Input::GetMousePosition();
            
            if (!m_IsDrawing)
            {
                m_IsDrawing = true;
                m_LastX = x;
                m_LastY = y;
                PaintStroke(x, y); // 点击的第一下
            }
            else
            {
                // 插值绘制
                double dx = x - m_LastX;
                double dy = y - m_LastY;
                double dist = std::sqrt(dx * dx + dy * dy);

                if (dist >= m_BrushSpacing)
                {
                    double dirX = dx / dist;
                    double dirY = dy / dist;

                    for (double step = 0; step <= dist; step += m_BrushSpacing)
                    {
                        PaintStroke(m_LastX + dirX * step, m_LastY + dirY * step);
                    }
                    m_LastX = x;
                    m_LastY = y;
                }
            }
        }
        else
        {
            m_IsDrawing = false;
        }

        // 2. 渲染最终画面 (将 FBO 贴到屏幕)
        Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
        Hazel::RenderCommand::Clear();

        std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_ScreenShader)->Bind();
        // 绑定 FBO 的颜色附件 texture 到 slot 0
        glBindTextureUnit(0, m_Framebuffer->GetColorAttachmentRendererID());
        
        m_ScreenVA->Bind();
        Hazel::RenderCommand::DrawIndexed(m_ScreenVA);
    }

    virtual void OnImGuiRender() override
    {
        ImGui::Begin("Brush Settings");
        ImGui::SliderFloat("Size", &m_BrushSize, 1.0f, 100.0f);
        ImGui::ColorEdit3("Color", glm::value_ptr(m_BrushColor));
        ImGui::SliderFloat("Spacing", &m_BrushSpacing, 1.0f, 50.0f);
        if (ImGui::Button("Clear Canvas"))
        {
            m_Framebuffer->Bind();
            Hazel::RenderCommand::SetClearColor({ 1, 1, 1, 1 });
            Hazel::RenderCommand::Clear();
            m_Framebuffer->Unbind();
        }
        ImGui::End();
    }

    void OnEvent(Hazel::Event& event) override {}

private:
    void CreateBrushTexture(uint32_t size)
    {
        std::vector<unsigned char> data(size * size * 4);
        float center = size / 2.0f;
        float radius = size / 2.0f;

        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                float dist = std::sqrt(std::pow(x - center, 2) + std::pow(y - center, 2));
                float alpha = 0.0f;
                if (dist < radius) {
                    float t = dist / radius;
                    alpha = 1.0f - t;
                    alpha = alpha * alpha; 
                }
                
                int index = (y * size + x) * 4;
                data[index + 0] = 255; // R
                data[index + 1] = 255; // G
                data[index + 2] = 255; // B
                data[index + 3] = (unsigned char)(alpha * 255); // A
            }
        }
        
        m_BrushTexture = Hazel::Texture2D::Create(size, size);
        m_BrushTexture->SetData(data.data(), data.size() * sizeof(unsigned char));
    }

    void PaintStroke(float x, float y)
    {
        m_Framebuffer->Bind();
        // 注意：Input::GetMousePosition 返回的是窗口坐标，y轴向下。OpenGL 纹理坐标y轴向上。
        // 但我们投影矩阵如果是 ortho(0, w, h, 0)，那么坐标系就和鼠标一致了。
        
        uint32_t width = m_Framebuffer->GetSpecification().Width;
        uint32_t height = m_Framebuffer->GetSpecification().Height;

        // 设置 Viewport 匹配 FBO
        glViewport(0, 0, width, height);

        // 使用 Shader
        auto shader = std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_BrushShader);
        shader->Bind();
        
        glm::mat4 projection = glm::ortho(0.0f, (float)width, (float)height, 0.0f);
        shader->UploadUniformMat4("projection", projection);
        shader->UploadUniformFloat2("offset", { x, y });
        shader->UploadUniformFloat("size", m_BrushSize);
        shader->UploadUniformFloat3("color", m_BrushColor);
        shader->UploadUniformInt("brushTexture", 0);

        m_BrushTexture->Bind(0);
        m_BrushVA->Bind();
        Hazel::RenderCommand::DrawIndexed(m_BrushVA);

        m_Framebuffer->Unbind();
    }

private:
    Hazel::Ref<Hazel::Framebuffer> m_Framebuffer;
    Hazel::Ref<Hazel::VertexArray> m_BrushVA, m_ScreenVA;
    Hazel::Ref<Hazel::Shader> m_BrushShader, m_ScreenShader;
    Hazel::Ref<Hazel::Texture2D> m_BrushTexture;

    bool m_IsDrawing = false;
    double m_LastX = 0, m_LastY = 0;
    
    float m_BrushSize = 25.0f;
    float m_BrushSpacing = 2.0f;
    glm::vec3 m_BrushColor = { 0.2f, 0.6f, 1.0f };
};

// 三维剖面演示层
class CrossSectionLayer : public Hazel::Layer
{
public:
    CrossSectionLayer() 
        : Layer("CrossSection"), 
          m_Camera(45.0f, 16.0f/9.0f, 0.1f, 100.0f)
    {
    }

    virtual void OnAttach() override
    {
        // 创建立方体几何体（包含法线）
        CreateCubeGeometry();
        
        // 加载着色器
        m_CrossSectionShader = m_ShaderLibrary.Load("assets/shaders/CrossSection.glsl");
        
        // 创建剖切面几何体（用于可视化）
        CreateClipPlaneGeometry();
        
        // 初始化相机
        m_CameraPosition = glm::vec3(3.0f, 3.0f, 3.0f);
        m_Camera.SetPosition(m_CameraPosition);
        m_Camera.LookAt(glm::vec3(0.0f));
        
        // 启用深度测试
        glEnable(GL_DEPTH_TEST);
    }

    virtual void OnUpdate(Hazel::Timestep ts) override
    {
        // 相机控制
        UpdateCamera(ts);
        
        // 清除缓冲
        Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
        Hazel::RenderCommand::Clear();
        glClear(GL_DEPTH_BUFFER_BIT);
        
        // 计算剖切平面方程 (Ax + By + Cz + D = 0)
        glm::vec4 clipPlane = glm::vec4(m_ClipPlaneNormal, m_ClipPlaneDistance);
        
        // 渲染立方体
        auto shader = std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_CrossSectionShader);
        shader->Bind();
        
        // 上传 uniforms
        shader->UploadUniformMat4("u_ViewProjection", m_Camera.GetViewProjectionMatrix());
        shader->UploadUniformMat4("u_Transform", glm::mat4(1.0f));
        shader->UploadUniformMat4("u_Model", glm::mat4(1.0f));
        shader->UploadUniformFloat4("u_ClipPlane", clipPlane);
        shader->UploadUniformFloat3("u_Color", m_CubeColor);
        shader->UploadUniformFloat3("u_LightPos", glm::vec3(5.0f, 5.0f, 5.0f));
        shader->UploadUniformFloat3("u_ViewPos", m_CameraPosition);
        shader->UploadUniformInt("u_EnableClipping", m_EnableClipping ? 1 : 0);
        shader->UploadUniformInt("u_ShowCrossSection", m_ShowCrossSection ? 1 : 0);
        shader->UploadUniformFloat3("u_CrossSectionColor", m_CrossSectionColor);
        
        // 绘制立方体
        m_CubeVA->Bind();
        Hazel::RenderCommand::DrawIndexed(m_CubeVA);
        
        // 如果显示剖切平面，绘制半透明平面
        if (m_ShowClipPlane)
        {
            RenderClipPlane();
        }
    }

    virtual void OnImGuiRender() override
    {
        ImGui::Begin("三维剖面控制");
        
        ImGui::Text("剖切平面设置");
        ImGui::Separator();
        
        ImGui::Checkbox("启用剖切", &m_EnableClipping);
        ImGui::Checkbox("显示剖切平面", &m_ShowClipPlane);
        ImGui::Checkbox("高亮剖切面", &m_ShowCrossSection);
        
        ImGui::Spacing();
        ImGui::Text("剖切平面法线");
        ImGui::SliderFloat("X", &m_ClipPlaneNormal.x, -1.0f, 1.0f);
        ImGui::SliderFloat("Y", &m_ClipPlaneNormal.y, -1.0f, 1.0f);
        ImGui::SliderFloat("Z", &m_ClipPlaneNormal.z, -1.0f, 1.0f);
        
        // 归一化法线
        if (glm::length(m_ClipPlaneNormal) > 0.01f)
        {
            m_ClipPlaneNormal = glm::normalize(m_ClipPlaneNormal);
        }
        
        ImGui::Spacing();
        ImGui::SliderFloat("剖切距离", &m_ClipPlaneDistance, -2.0f, 2.0f);
        
        ImGui::Spacing();
        ImGui::ColorEdit3("立方体颜色", glm::value_ptr(m_CubeColor));
        ImGui::ColorEdit3("剖切面颜色", glm::value_ptr(m_CrossSectionColor));
        
        ImGui::Spacing();
        ImGui::Text("快捷预设");
        if (ImGui::Button("XY平面")) {
            m_ClipPlaneNormal = glm::vec3(0.0f, 0.0f, 1.0f);
            m_ClipPlaneDistance = 0.0f;
        }
        ImGui::SameLine();
        if (ImGui::Button("XZ平面")) {
            m_ClipPlaneNormal = glm::vec3(0.0f, 1.0f, 0.0f);
            m_ClipPlaneDistance = 0.0f;
        }
        ImGui::SameLine();
        if (ImGui::Button("YZ平面")) {
            m_ClipPlaneNormal = glm::vec3(1.0f, 0.0f, 0.0f);
            m_ClipPlaneDistance = 0.0f;
        }
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("相机控制");
        ImGui::Text("鼠标右键拖动旋转");
        ImGui::Text("滚轮缩放");
        ImGui::SliderFloat("旋转速度", &m_CameraRotateSpeed, 0.1f, 5.0f);
        
        ImGui::End();
    }

    void OnEvent(Hazel::Event& event) override
    {
    }

private:
    void CreateCubeGeometry()
    {
        // 立方体顶点数据（位置 + 法线 + 纹理坐标）
        float cubeVertices[] = {
            // 前面 (Z+)
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
            
            // 后面 (Z-)
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
            
            // 左面 (X-)
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
            
            // 右面 (X+)
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
            
            // 上面 (Y+)
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
            
            // 下面 (Y-)
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f
        };
        
        uint32_t cubeIndices[] = {
            0,  1,  2,  2,  3,  0,   // 前面
            4,  5,  6,  6,  7,  4,   // 后面
            8,  9, 10, 10, 11,  8,   // 左面
            12, 13, 14, 14, 15, 12,  // 右面
            16, 17, 18, 18, 19, 16,  // 上面
            20, 21, 22, 22, 23, 20   // 下面
        };
        
        m_CubeVA.reset(Hazel::VertexArray::Create());
        
        Hazel::Ref<Hazel::VertexBuffer> cubeVB;
        cubeVB.reset(Hazel::VertexBuffer::Create(cubeVertices, sizeof(cubeVertices)));
        cubeVB->SetLayout({
            { Hazel::ShaderDataType::Float3, "a_Position" },
            { Hazel::ShaderDataType::Float3, "a_Normal" },
            { Hazel::ShaderDataType::Float2, "a_TexCoord" }
        });
        m_CubeVA->AddVertexBuffer(cubeVB);
        
        Hazel::Ref<Hazel::IndexBuffer> cubeIB;
        cubeIB.reset(Hazel::IndexBuffer::Create(cubeIndices, sizeof(cubeIndices) / sizeof(uint32_t)));
        m_CubeVA->SetIndexBuffer(cubeIB);
    }
    
    void CreateClipPlaneGeometry()
    {
        // 创建一个大的平面用于可视化剖切面
        float planeVertices[] = {
            -2.0f, -2.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
             2.0f, -2.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
             2.0f,  2.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
            -2.0f,  2.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f
        };
        
        uint32_t planeIndices[] = { 0, 1, 2, 2, 3, 0 };
        
        m_ClipPlaneVA.reset(Hazel::VertexArray::Create());
        
        Hazel::Ref<Hazel::VertexBuffer> planeVB;
        planeVB.reset(Hazel::VertexBuffer::Create(planeVertices, sizeof(planeVertices)));
        planeVB->SetLayout({
            { Hazel::ShaderDataType::Float3, "a_Position" },
            { Hazel::ShaderDataType::Float3, "a_Normal" },
            { Hazel::ShaderDataType::Float2, "a_TexCoord" }
        });
        m_ClipPlaneVA->AddVertexBuffer(planeVB);
        
        Hazel::Ref<Hazel::IndexBuffer> planeIB;
        planeIB.reset(Hazel::IndexBuffer::Create(planeIndices, 6));
        m_ClipPlaneVA->SetIndexBuffer(planeIB);
    }
    
    void RenderClipPlane()
    {
        // 启用混合以显示半透明平面
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        
        auto shader = std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_CrossSectionShader);
        shader->Bind();
        
        // 计算剖切平面的变换矩阵
        glm::mat4 planeTransform = CalculatePlaneTransform();
        
        shader->UploadUniformMat4("u_ViewProjection", m_Camera.GetViewProjectionMatrix());
        shader->UploadUniformMat4("u_Transform", planeTransform);
        shader->UploadUniformMat4("u_Model", planeTransform);
        shader->UploadUniformFloat4("u_ClipPlane", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        shader->UploadUniformFloat3("u_Color", glm::vec3(1.0f, 1.0f, 0.0f));
        shader->UploadUniformFloat3("u_LightPos", glm::vec3(5.0f, 5.0f, 5.0f));
        shader->UploadUniformFloat3("u_ViewPos", m_CameraPosition);
        shader->UploadUniformInt("u_EnableClipping", 0);
        shader->UploadUniformInt("u_ShowCrossSection", 0);
        shader->UploadUniformFloat3("u_CrossSectionColor", m_CrossSectionColor);
        
        // 修改片段着色器输出的 alpha 值（需要在着色器中处理，或者直接设置固定alpha）
        m_ClipPlaneVA->Bind();
        
        // 临时修改颜色使其半透明
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE); // 不写入深度
        Hazel::RenderCommand::DrawIndexed(m_ClipPlaneVA);
        glDepthMask(GL_TRUE);
        
        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
    }
    
    glm::mat4 CalculatePlaneTransform()
    {
        // 计算从 Z 轴到法线方向的旋转
        glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 normal = m_ClipPlaneNormal;
        
        glm::mat4 rotation = glm::mat4(1.0f);
        if (glm::length(glm::cross(up, normal)) > 0.01f)
        {
            glm::vec3 axis = glm::normalize(glm::cross(up, normal));
            float angle = acos(glm::dot(up, normal));
            rotation = glm::rotate(glm::mat4(1.0f), angle, axis);
        }
        else if (glm::dot(up, normal) < 0.0f)
        {
            rotation = glm::rotate(glm::mat4(1.0f), glm::pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f));
        }
        
        // 平移到剖切距离
        glm::vec3 position = -m_ClipPlaneNormal * m_ClipPlaneDistance;
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);
        
        return translation * rotation;
    }
    
    void UpdateCamera(Hazel::Timestep ts)
    {
        // 鼠标右键旋转相机
        if (Hazel::Input::IsMouseButtonPressed(1)) // 右键
        {
            auto [x, y] = Hazel::Input::GetMousePosition();
            
            if (!m_IsRotating)
            {
                m_IsRotating = true;
                m_LastMouseX = x;
                m_LastMouseY = y;
            }
            else
            {
                float deltaX = (x - m_LastMouseX) * m_CameraRotateSpeed * 0.1f;
                float deltaY = (y - m_LastMouseY) * m_CameraRotateSpeed * 0.1f;
                
                m_CameraYaw += deltaX;
                m_CameraPitch -= deltaY;
                
                // 限制俯仰角
                if (m_CameraPitch > 89.0f) m_CameraPitch = 89.0f;
                if (m_CameraPitch < -89.0f) m_CameraPitch = -89.0f;
                
                m_LastMouseX = x;
                m_LastMouseY = y;
                
                UpdateCameraPosition();
            }
        }
        else
        {
            m_IsRotating = false;
        }
        
        // 滚轮缩放（需要在 Event 中处理，这里简化为键盘）
        if (Hazel::Input::IsKeyPressed(static_cast<int>(HZ_KEY_Q)))
        {
            m_CameraDistance -= 2.0f * ts;
            if (m_CameraDistance < 1.0f) m_CameraDistance = 1.0f;
            UpdateCameraPosition();
        }
        if (Hazel::Input::IsKeyPressed(static_cast<int>(HZ_KEY_E)))
        {
            m_CameraDistance += 2.0f * ts;
            if (m_CameraDistance > 20.0f) m_CameraDistance = 20.0f;
            UpdateCameraPosition();
        }
    }
    
    void UpdateCameraPosition()
    {
        // 球坐标转换为笛卡尔坐标
        float yawRad = glm::radians(m_CameraYaw);
        float pitchRad = glm::radians(m_CameraPitch);
        
        m_CameraPosition.x = m_CameraDistance * cos(pitchRad) * cos(yawRad);
        m_CameraPosition.y = m_CameraDistance * sin(pitchRad);
        m_CameraPosition.z = m_CameraDistance * cos(pitchRad) * sin(yawRad);
        
        m_Camera.SetPosition(m_CameraPosition);
        m_Camera.LookAt(glm::vec3(0.0f));
    }

private:
    Hazel::ShaderLibrary m_ShaderLibrary;
    Hazel::Ref<Hazel::Shader> m_CrossSectionShader;
    Hazel::Ref<Hazel::VertexArray> m_CubeVA;
    Hazel::Ref<Hazel::VertexArray> m_ClipPlaneVA;
    
    Hazel::PerspectiveCamera m_Camera;
    glm::vec3 m_CameraPosition;
    float m_CameraDistance = 5.0f;
    float m_CameraYaw = -45.0f;
    float m_CameraPitch = 30.0f;
    float m_CameraRotateSpeed = 2.0f;
    
    bool m_IsRotating = false;
    float m_LastMouseX = 0.0f;
    float m_LastMouseY = 0.0f;
    
    // 剖切平面参数
    glm::vec3 m_ClipPlaneNormal = glm::vec3(1.0f, 0.0f, 0.0f);
    float m_ClipPlaneDistance = 0.0f;
    bool m_EnableClipping = true;
    bool m_ShowClipPlane = true;
    bool m_ShowCrossSection = true;
    
    glm::vec3 m_CubeColor = glm::vec3(0.3f, 0.6f, 0.9f);
    glm::vec3 m_CrossSectionColor = glm::vec3(1.0f, 0.8f, 0.2f);
};

class Sandbox : public Hazel::Application
{
public:
    Sandbox()
    {
        // PushLayer(new ExampleLayer());
        // PushLayer(new BrushLayer());
        PushLayer(new CrossSectionLayer());
    }

    ~Sandbox()
    {
    }
};

Hazel::Application* Hazel::CreateApplication()
{
    return new Sandbox();
}
