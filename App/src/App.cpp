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

	virtual void OnInit() override
	{
		EditorLayer* m_EditorLayer = new EditorLayer("EditorLayer");
		PushLayer(m_EditorLayer);
		
	}	
	
	virtual void OnShutdown() override
	{
	}
};
static Spaceng::Application* Spaceng::CreateApplication()
{
	return new App();
}
}