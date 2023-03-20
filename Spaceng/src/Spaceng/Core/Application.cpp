#include "PCH.h"
#include "Application.h"

namespace Spaceng {


	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSettings& Settings)
	{

		std::string IP = "localhost";
		asio::io_context IO_context;
		Myserver = new Server(IO_context, 1337);
		//Myserver->Run();
		Myclient = new Client(1337, IP);
		Myclient->connect();
		Myclient->SendData("Hello");
		

		s_Instance = this;
		m_AppWindow = std::unique_ptr<Window>
			(Window::Create(WindowSettings(Settings.Name, Settings.WindowWidth, Settings.WindowHeight, Settings.WindowPosx, Settings.WindowPosy)));
		SE_ASSERT(m_AppWindow, "Window could not be created");

		m_Renderer = new VulkanRenderer();

		m_Renderer->InitExtensions(
			EnabledInstanceextensions, enabledDeviceExtensions, enabledDeviceFeatures);
		
		m_Renderer->InitRenderer();
		m_AppWindow->InitWindow(m_Renderer); 
		


		SE_LOG_INFO("Window : ""{0}: ({1}/{2})", Settings.Name, Settings.WindowWidth, Settings.WindowHeight)
			m_AppWindow->SetEventCallback(SE_BIND_EVENT(Application::OnEvent));
		SE_LOG_DEBUG("{}: EventCallback Enabled", m_AppWindow->GetTitle())
			m_AppWindow->SetVsync(true); 

		//Note :  Virtual OnInit(); Entrypoint
	}


	Application::~Application()
	{
		for (Layer* layer : m_LayerStack)
		{
			layer->OnDetach();
			delete layer;
		}

		size_t Size = m_Assets.size();
		for (uint32_t i = 0; i < Size; i++)
		{
			DestroyAsset(m_Assets[0]);
		}
		//delete Myserver;
		//delete Myclient;
		delete m_Renderer;
		m_Renderer = nullptr;

	}


	void Application::PrepareAsset(std::string name ,AssetType type,std::string filepath, bool DepthStencil)
	{
		VkGLTFAsset* Asset = new VkGLTFAsset(name , type , DepthStencil, filepath);
		m_Renderer->prepareAsset(Asset ,type);
		m_Assets.emplace_back(Asset); 
		SE_LOG_WARN("Asset - {0}- Loaded", Asset->getName());
		Asset_Nr_Changed = true;
	}

	void Application::DestroyAsset(VkGLTFAsset* Asset)
	{
		m_Renderer->CleanUpAsset(Asset);
		std::erase(m_Assets, Asset);
		SE_LOG_WARN("Asset - {0} - removed", Asset->getName());
		delete Asset;
		Asset = nullptr;
		Asset_Nr_Changed = true;
	}

	void Application::Render()
	{
		if (Asset_Nr_Changed)
		{
			m_Renderer->RecordCommandBuffers(&m_Assets);
		}
		Asset_Nr_Changed = false;
		m_Renderer->render(&m_Assets);
	}


	void Application::Run()
	{
		OnInit();
		while (m_Running)
		{
			m_AppWindow->PollEvents();
			if (!m_Minimized)
			{ 
				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(m_Timestep);
				m_Renderer->setView(); 

				if (textureIndexDebugging==0 || textureIndexDebugging>50)
					textureIndexDebugging = 1;

				m_Renderer->RefreshTexture(m_Assets[0], textureIndexDebugging);
				m_Renderer->RecordCommandBuffers(&m_Assets);
				Render();
				textureIndexDebugging++;
			}
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
		uint32_t width = e.GetWidth(); //Assuming 1 Display per Window
		uint32_t height = e.GetHeight();
		m_Renderer->GenerateDisplay(&width,&height, m_AppWindow->GetVsync() , &m_Assets);
		if (width == 0 && height == 0)
		{
			m_Minimized = true;
		}
		else
		{
			m_Minimized = false;
		}
		return true;
	}
	bool Application::OnKeyPressed(KeyPressedEvent& e)
	{
		switch (e.GetKeyCode())
		{
		case Key::C:
			m_AppWindow->SetToFullScreen();
			break;
		case Key::F:
			
			break;
		case Key::D:
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
		delete layer;
	}
	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverLay(layer);
		layer->OnAttach();
	}

	std::string Application::getProjectDirectory() 
	{
		std::filesystem::path workingDirectory = std::filesystem::current_path();
		return workingDirectory.string();
	}
}