#include "Hazel/Core/Application.h"
#include "Hazel/Core/Log.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "Hazel/Renderer/Renderer.h"
#include "Hazel/Renderer/RenderCommand.h"

namespace Hazel  
{
    Application* Application::s_Instance = nullptr;

    #define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

    Application::Application() 
    {
        s_Instance = this;
        m_Window = std::unique_ptr<Window>(Window::Create());
        m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

        Renderer::Init();

        m_ImGuiLayer = new ImGuiLayer();
        PushOverLayer(m_ImGuiLayer);
    }

    Application::~Application() 
    {

    }

    void Application::Run()
    {
        while (m_Running)
        {
            float time = (float)glfwGetTime();
            Timestep timestep = time - m_LastFrameTime;
            m_LastFrameTime = time;

            for (Layer* layer : m_LayerStack)
                layer->OnUpdate(timestep);

            m_ImGuiLayer->Begin();
            for (Layer* layer : m_LayerStack)
                layer->OnImGuiRender();
            m_ImGuiLayer->End();

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