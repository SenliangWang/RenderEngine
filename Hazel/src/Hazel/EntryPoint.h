#pragma once
#include <iostream>
#include "Hazel/Core/Log.h"

// 需要使用 extern 来告诉编译器该函数是在其他文件中定义的
extern  Hazel::Application*   Hazel::CreateApplication();

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) 
{
    Hazel::Log::Init();

    // Let the actual project to implement the CreateApplication() and return the app*
    auto app = Hazel::CreateApplication();
    app->Run();
    delete app;
}
