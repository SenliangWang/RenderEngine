#pragma once

#include "hzpch.h"
#include "Window.h"

namespace Hazel 
{
    class Application 
    {
    public:
        Application();
        virtual ~Application();

        void Run();

    private:
        std::unique_ptr<Window> m_Window;
        bool m_Running = true;
    
    };

    extern Application* CreateApplication();
}
