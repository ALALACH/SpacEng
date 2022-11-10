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
	Spaceng::EditorLayer* m_EditorLayer;

	
	virtual void OnInit() override
	{
		m_EditorLayer = new Spaceng::EditorLayer("EditorLayer");
		PushLayer(m_EditorLayer);
	}	
	
	virtual void OnShutdown() override
	{
		PopLayer(m_EditorLayer);
		delete m_EditorLayer;
	}
};
static Spaceng::Application* Spaceng::CreateApplication()
{
	return new App();
}
}