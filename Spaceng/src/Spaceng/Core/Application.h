#pragma once
#include "Core.h"
#include "Spaceng/Window/Window.h"
#include "LayerStack.h"
#include "Event.h"

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
		void OnUpdate();
		void OnEvent(Event& Event);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnKeyPressed(KeyPressedEvent& e);

		static inline Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_AppWindow; }

		//@poly
		virtual void OnInit() {}
		virtual void OnShutdown() {}


	private:
		std::unique_ptr<Window> m_AppWindow , window1;
		static Application* s_Instance;
		bool m_Running = true;
		bool m_Minimized = false;
		float m_Timestep=0.0f, m_lastframetime=0.0f;
		LayerStack m_LayerStack;

	};

	//implemented in client
	Application* CreateApplication();

}