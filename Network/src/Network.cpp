//Entry
#include "Spaceng/Core/Application.h"
#include "EntryPoint.h"


#include "PCH.h"
#include "EditorLayer.h"

namespace Spaceng
{
class App : public Spaceng::Application
{
public:
	App(int argc, char** argv)
		:Application(argc , argv) {}

	virtual void OnInit() override
	{
		EditorLayer* m_EditorLayer = new EditorLayer("EditorLayer");
		
		PushLayer(m_EditorLayer);
	}	
	
	virtual void OnShutdown() override
	{
	}
};
static Spaceng::Application* Spaceng::CreateApplication(int argc, char** argv)
{
	return new App(argc,  argv);
}
}