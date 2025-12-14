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

class Sandbox : public Hazel::Application
{
public:
    Sandbox()
    {
        // PushLayer(new ExampleLayer());
        PushLayer(new BrushLayer());
    }

    ~Sandbox()
    {
    }
};

Hazel::Application* Hazel::CreateApplication()
{
    return new Sandbox();
}
