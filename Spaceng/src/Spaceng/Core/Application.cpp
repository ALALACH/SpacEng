#include "PCH.h"
#include "Application.h"
#include <glfw/include/GLFW/glfw3.h>

namespace Spaceng {


	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSettings& Settings)
	{
		s_Instance = this;
		m_AppWindow = std::unique_ptr<Window>
			(Window::Create(WindowSettings(Settings.Name, Settings.WindowWidth, Settings.WindowHeight, Settings.WindowPosx,Settings.WindowPosy)));
		window1= std::unique_ptr<Window>
			(Window::Create(WindowSettings(Settings.Name, Settings.WindowWidth, Settings.WindowHeight)));

		SE_LOG_WARN("{0}: ({1}/{2})",Settings.Name,Settings.WindowWidth,Settings.WindowHeight)

		m_AppWindow->SetEventCallback(SE_BIND_EVENT(Application::OnEvent));
		window1->SetEventCallback(SE_BIND_EVENT(Application::OnEvent));

		SE_LOG_DEBUG("{}: EventCallback Enabled", m_AppWindow->GetTitle())

		m_AppWindow->SetVsync(true); //disable and Debug
		
		

	}

	Application::~Application()
	{
	}
	void Application::Run()
	{
		OnInit();
		while (m_Running)
		{
			if (!m_Minimized)
			{ 
				Application* app = this;

				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(m_Timestep);

			}

			if(m_AppWindow)
				m_AppWindow->OnUpdate(m_Timestep);


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

	void Application::OnUpdate()
	{

	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		if (m_AppWindow->GetGlfwWindow() == e.GetWindowID())
		{ 
			m_Running = false;
		}
		else if 
			(window1->GetGlfwWindow() == e.GetWindowID())
			{ 
				window1->ShutDownWin();
			}

		//m_Running = false;
		return true;
	}
	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		//todo : Resize viewport & ImGui Pannels
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
	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverLay(layer);
		layer->OnAttach();
	}
}