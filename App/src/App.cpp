#include "Spaceng.h"
#include "EntryPoint.h"
#include "EditorLayer.h"


class App : public Spaceng::Application
{
public:
	App()
	{

	}
	virtual void OnInit() override
	{
		PushLayer(new Spaceng::EditorLayer("EditorLayer"));
	}	
	
	virtual void OnShutdown() override
	{
		
	}
};
static Spaceng::Application* Spaceng::CreateApplication()
{
	return new App();
}