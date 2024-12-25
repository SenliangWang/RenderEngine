#pragma once

#include "hzpch.h"
#include "Window.h"
#include "Hazel/Core/Events/ApplicationEvent.h"

namespace Hazel 
{
    class Application 
    {
    public:
        Application();
        virtual ~Application();

        void Run();
        void OnEvent(Event& e);

    private:
        bool OnWindowClose(WindowCloseEvent& e);

    private:
        std::unique_ptr<Window> m_Window;
        bool m_Running = true;
    
    };

    extern Application* CreateApplication();
}
