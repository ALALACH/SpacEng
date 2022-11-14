#include "EditorLayer.h"
#include "PCH.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui.h"



namespace Spaceng
{

	EditorLayer::EditorLayer(const std::string Name)
		:m_DebugName(Name)
	{

	}
	EditorLayer::~EditorLayer()
	{
		//free Dynamically all members
	}

	void EditorLayer::OnAttach()
	{
		LoadAsset("Object1",MeshType, "");
	}

	void EditorLayer::OnDetach()
	{

	}

	void EditorLayer::OnUpdate(float Timestep)
	{
		Application::Get().GetWindow().UpdateTittle("SpaceEngine");
	}

	void EditorLayer::OnEvent(Event& Event)
	{
		EventDispatcher dispatcher(Event);
		dispatcher.Dispatch<KeyPressedEvent>(SE_BIND_EVENT(EditorLayer::OnKeyPressed));

	}
	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		switch (e.GetKeyCode())
		{
		case Key::C:

			break;
		}
		return false;
	}


	void EditorLayer::LoadAsset(std::string name,AssetType type,std::string filename)
	{
		Application::Get().PrepareAsset(name,type , filename);
	}
}