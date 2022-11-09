#pragma once

#include "Core.h"
#include "LayerStack.h"
#include "Event.h"

#include "Spaceng/Window/Window.h"
#include <glfw/include/GLFW/glfw3.h>

#include "Spaceng/Renderer/VulkanRenderer.h"

#include "Spaceng/GUI/ImGuiLayer.h"

namespace Spaceng {

	struct ApplicationSettings
	{
		std::string Name;
		uint32_t WindowWidth, WindowHeight;
		uint32_t WindowPosx, WindowPosy;
	};
	
	class Application
	{
	public:
		Application(const ApplicationSettings& Settings = { "SpacEng",1280,720,400,200 });
		virtual ~Application();

		void Run();
		void OnEvent(Event& Event);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		void PopLayer(Layer* layer);

		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnKeyPressed(KeyPressedEvent& e);

		static inline Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_AppWindow; }

		// Implemented in Client
		virtual void OnInit() {}
		virtual void OnShutdown() {}

	private:
		static Application* s_Instance;
		LayerStack m_LayerStack;

		VulkanRenderer* m_Renderer;
		std::unique_ptr<Window> m_AppWindow;

		ImGuiLayer* m_ImGuiLayer;

		bool m_Running = true;
		bool m_Minimized = false;
		float m_Timestep=0.0f, m_lastframetime=0.0f;



		// Renderer Specefic
		std::vector<const char*> EnabledInstanceextensions =
		{
		};
		std::vector<const char*> enabledDeviceExtensions =
		{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
		VkPhysicalDeviceFeatures enabledDeviceFeatures{};
		

		
	};

	//implemented in client
	Application* CreateApplication();

}