#pragma once
#include <iostream>
#include "Hazel/Core/Log.h"

#ifdef  HZ_PLATFORM_WINDOWS

// 需要使用 extern 来告诉编译器该函数是在其他文件中定义的
extern  Hazel::Application*   Hazel::CreateApplication();

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) 
{
	Hazel::Log::Init();
	// 原本代码输出日志
	Hazel::Log::GetCoreLogger()->warn("Initialized Log!");
	Hazel::Log::GetClientLogger()->error("Initialized Log Error!");

	// 用宏定义，输出日志
	HZ_CORE_WARN("Initialized Log!");
	int a = 5;
	HZ_INFO("Hello! Var={0}", a);

	// Let the actual project to implement the CreateApplication() and return the app*
	auto app = Hazel::CreateApplication();
	app->Run();
	delete app;
}

#endif //  HZ_PLATFORM_WINDOWS
