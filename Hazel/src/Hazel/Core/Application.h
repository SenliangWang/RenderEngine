#pragma once

#include "hzpch.h"
#include "Window.h"
#include "Hazel/Core/Events/ApplicationEvent.h"
#include "Hazel/Core/LayerStack.h"

#include "Hazel/Renderer/Buffer.h"
#include "Hazel/Renderer/Shader.h"

namespace Hazel 
{
    class Application 
    {
    public:
        Application();
        virtual ~Application();

        void Run();
        void OnEvent(Event& e);

        void PushLayer(Layer* pLayer);
        void PushOverLayer(Layer* pLayer);

		inline Window& GetWindow() { return *m_Window; }
		
		static inline Application& Get() { return *s_Instance; }

    private:
        bool OnWindowClose(WindowCloseEvent& e);

    private:
        std::unique_ptr<Window> m_Window;
        bool m_Running = true;
        LayerStack m_LayerStack;

        unsigned int m_VertexArray;
        std::unique_ptr<VertexBuffer> m_VertexBuffer;
        std::unique_ptr<IndexBuffer> m_IndexBuffer;

		std::unique_ptr<Shader> m_Shader;

    private:
        static Application* s_Instance;
    };

    extern Application* CreateApplication();
}
