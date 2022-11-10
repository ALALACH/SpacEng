#include "PCH.h"
#include "Application.h"



namespace Spaceng {


	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSettings& Settings)
	{
		s_Instance = this;
		m_AppWindow = std::unique_ptr<Window>
			(Window::Create(WindowSettings(Settings.Name, Settings.WindowWidth, Settings.WindowHeight, Settings.WindowPosx, Settings.WindowPosy)));
		SE_ASSERT(m_AppWindow, "Window could not be created");

		m_Renderer = new VulkanRenderer();
		m_Renderer->InitRenderer(
			EnabledInstanceextensions, enabledDeviceExtensions, enabledDeviceFeatures);
		m_AppWindow->InitWindow(m_Renderer);
		


		SE_LOG_INFO("Window : ""{0}: ({1}/{2})", Settings.Name, Settings.WindowWidth, Settings.WindowHeight)
			m_AppWindow->SetEventCallback(SE_BIND_EVENT(Application::OnEvent));
		SE_LOG_DEBUG("{}: EventCallback Enabled", m_AppWindow->GetTitle())
			m_AppWindow->SetVsync(true); 



	}
	Application::~Application()
	{
		for (Layer* layer : m_LayerStack)
		{
			layer->OnDetach();
		}
		delete m_Renderer;
	}


	void Application::Run()
	{
		while (m_Running)
		{
			if (!m_Minimized)
			{ 
				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(m_Timestep);

			}
			//if camera.ismoving()    ´==> UpdateUniformBuffers();
			if(m_AppWindow)
				m_AppWindow->PollEvents(m_Timestep);
				//m_Renderer->render();
				

				m_Timestep = (float)glfwGetTime() - m_lastframetime;
				m_lastframetime = (float)glfwGetTime();	
		}
	}
	
	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(SE_BIND_EVENT(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(SE_BIND_EVENT(Application::OnWindowResize));	
		dispatcher.Dispatch<KeyPressedEvent>(SE_BIND_EVENT(Application::OnKeyPressed));
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		if (m_AppWindow->GetGlfwWindow() == e.GetWindowID())
		{ 
			m_Running = false;
		}
#if 0
		else if
			(window1->GetGlfwWindow() == e.GetWindowID())
			{ 
				window1->ShutDownWin();
				//when updated , stop updating first
			}
#endif
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		//todo : Resize viewport & ImGui Pannels
		uint32_t width = e.GetWidth();
		uint32_t height = e.GetHeight();
		m_Renderer->Refresh(&width,&height, m_AppWindow->GetVsync());

		return true;
	}
	bool Application::OnKeyPressed(KeyPressedEvent& e)
	{
		switch (e.GetKeyCode())
		{
		case Key::C:
			m_AppWindow->SetToFullScreen();
			break;
		}
		return false;
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}
	void Application::PopLayer(Layer* layer)
	{
		m_LayerStack.PopLayer(layer);
		layer->OnDetach();
	}
	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverLay(layer);
		layer->OnAttach();
	}
}