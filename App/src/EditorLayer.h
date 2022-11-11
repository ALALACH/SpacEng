#pragma once
#include "Spaceng.h"
#include "glm/glm/glm.hpp"


namespace Spaceng
{ 
	class EditorLayer : public Layer
	{
	public:

		EditorLayer(const std::string Name);
		virtual ~EditorLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(float Timestep) override;
		virtual void OnEvent(Event& Event) override;
		bool OnKeyPressed(KeyPressedEvent& e);
		void LoadAsset(AssetType type, std::string filename);
	private:
		std::string m_DebugName;
	};
}
