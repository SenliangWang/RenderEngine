#pragma once

#include "hzpch.h"

namespace Hazel {
    class Application {
    public:
        Application();
        virtual ~Application();

        void Run();
    };

    extern Application* CreateApplication();
}
