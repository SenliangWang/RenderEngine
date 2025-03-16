#include "Hazel/Core/Application.h"
#include "Hazel/Core/Log.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

namespace Hazel  
{
	Application* Application::s_Instance = nullptr;

	#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case Hazel::ShaderDataType::Float:    return GL_FLOAT;
		case Hazel::ShaderDataType::Float2:   return GL_FLOAT;
		case Hazel::ShaderDataType::Float3:   return GL_FLOAT;
		case Hazel::ShaderDataType::Float4:   return GL_FLOAT;
		case Hazel::ShaderDataType::Mat3:     return GL_FLOAT;
		case Hazel::ShaderDataType::Mat4:     return GL_FLOAT;
		case Hazel::ShaderDataType::Int:      return GL_INT;
		case Hazel::ShaderDataType::Int2:     return GL_INT;
		case Hazel::ShaderDataType::Int3:     return GL_INT;
		case Hazel::ShaderDataType::Int4:     return GL_INT;
		case Hazel::ShaderDataType::Bool:     return GL_BOOL;
		}

		HZ_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	Application::Application() 
	{
		s_Instance = this;
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		glGenVertexArrays(1, &m_VertexArray);
		glBindVertexArray(m_VertexArray);

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f, 1.0f,
			 0.0f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f, 1.0f
		};

		m_VertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));
		m_VertexBuffer->Bind();


		BufferLayout layout = 
		{
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" }
		};
		m_VertexBuffer->SetLayout(layout);

		uint32_t index = 0;
		const auto& bufferLayout = m_VertexBuffer->GetLayout();
		for (const auto& element : bufferLayout)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				bufferLayout.GetStride(),
				(const void*)element.Offset);
			index++;
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = vec4(a_Position, 1.0);	
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

		m_Shader.reset(new Shader(vertexSrc, fragmentSrc));
	}

	Application::~Application() 
	{

	}

	void Application::Run()
	{
		while (m_Running)
		{
			glClearColor(0.1, 0.2, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			m_Shader->Bind();
			glBindVertexArray(m_VertexArray);
			glDrawArrays(GL_TRIANGLES, 0, 3);

			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();

			m_Window->OnUpdate();
		}
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
			{
				break;
			}
		}

		// HZ_CORE_TRACE("{0}", e.ToString());
	}

	void Application::PushLayer(Layer* pLayer)
	{
		m_LayerStack.PushLayer(pLayer);
		pLayer->OnAttach();
	}

	void Application::PushOverLayer(Layer* pLayer)
	{
		m_LayerStack.PushOverlay(pLayer);
		pLayer->OnAttach();
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
}