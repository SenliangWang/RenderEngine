#include "Hazel.h"

class Sandbox : public Hazel::Application 
{
public:
	Sandbox() 
	{
		PushOverLayer(new Hazel::ImGuiLayer());
	}

	~Sandbox() 
	{  

	}
};

Hazel::Application* Hazel::CreateApplication()  
{
	return new Sandbox();
}
