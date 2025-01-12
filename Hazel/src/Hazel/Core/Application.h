#pragma once

#include "hzpch.h"
#include "Window.h"
#include "Hazel/Core/Events/ApplicationEvent.h"
#include "Hazel/Core/LayerStack.h"

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

        static Application* s_Instance;
    };

    extern Application* CreateApplication();
}
