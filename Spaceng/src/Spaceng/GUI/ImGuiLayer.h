#pragma once
#include "Spaceng/Core/Layer.h"
#include <imgui/imgui.h>

namespace Spaceng
{
	class ImGuiLayer : public Spaceng::Layer
	{
	public :

		ImGuiLayer();
		~ImGuiLayer();

		static ImGuiLayer* Create();


		virtual void OnAttach() override;
		virtual void OnDetach() override;
		void Begin();
		void End();
		void SetDarkThemeColors();
	};
	
}