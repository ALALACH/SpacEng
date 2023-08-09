#pragma once

#include "Core.h"
#include "LayerStack.h"
#include "Event.h"
#include "Spaceng/Window/Window.h"
#include <glfw/include/GLFW/glfw3.h>

#include "Spaceng/Renderer/VulkanRenderer.h"
#include "Spaceng/Networking/Networking.h"

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
		Application(int argc, char** argv,const ApplicationSettings& Settings = { "SpacEng",1280,720,400,200 });
		virtual ~Application();
		std::string getProjectDirectory();


		void PrepareAsset(std::string name ,AssetType Type , bool DepthStencil);
		void DestroyAsset(VkGLTFAsset* Asset);
		void Render();

		void Run();
		void SetEvent(Event& Event);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		void PopLayer(Layer* layer);

		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnKeyPressed(KeyPressedEvent& e);

		static inline Application& Get() { return *s_Instance;}
		inline Window& GetWindow() { return *m_AppWindow; }
		inline Server& GetServer() { return *Myserver; }
		inline Client& GetClient() { return *Myclient; }
		inline VulkanRenderer& GetRenderer() { return *m_Renderer; }
		
		// Implemented in Client
		virtual void OnInit() {}
		virtual void OnShutdown() {}

	
	private:
		std::filesystem::path ExecutablePath;
		bool m_Running = true;
		bool m_Minimized = false;


		LayerStack m_LayerStack;
		static Application* s_Instance;
		ImGuiLayer* m_ImGuiLayer;


		Server* Myserver;
		Client* Myclient;
		VulkanRenderer* m_Renderer;
		//the elements of an unordered_set are immutable  todo: separate dynamic and static assets
		std::vector<VkGLTFAsset*> m_Assets;
		std::unique_ptr<Window> m_AppWindow;

		bool Asset_Nr_Changed = false;
		uint32_t textureIndexDebugging=0;
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
	Application* CreateApplication(int argc, char** argv);

}