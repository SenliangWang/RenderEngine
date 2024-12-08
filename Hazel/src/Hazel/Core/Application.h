#pragma once

namespace Hazel {
    class Application {
    public:
        Application();
        virtual ~Application();

        void Run();
    };

    extern Application* CreateApplication();
}
