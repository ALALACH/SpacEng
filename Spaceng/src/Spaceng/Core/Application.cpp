#include "PCH.h"
#include "Application.h"

namespace Spaceng {


	Application* Application::s_Instance = nullptr;

	Application::Application(int argc, char** argv,const ApplicationSettings& Settings)
	{
		ExecutablePath = std::filesystem::canonical(std::filesystem::path(argv[0]));
		SE_LOG_INFO("ProjectDirectory : {0}", getProjectDirectory())
		s_Instance = this;

		std::string IP = "localhost";
		Myserver = new Server(1338);
		Myserver->_Run();
		
		Myclient = new Client(1338, IP);
		Myclient->connect();
		

		

		m_AppWindow = std::unique_ptr<Window>
			(Window::Create(WindowSettings(Settings.Name, Settings.WindowWidth, Settings.WindowHeight, Settings.WindowPosx, Settings.WindowPosy)));
		SE_ASSERT(m_AppWindow, "Window could not be created");

		m_Renderer = new VulkanRenderer();

		m_Renderer->InitExtensions(
			EnabledInstanceextensions, enabledDeviceExtensions, enabledDeviceFeatures);
		
		m_Renderer->InitRenderer();
		m_AppWindow->InitWindow(m_Renderer); 
		


		SE_LOG_INFO("Window : ""{0}: ({1}/{2})", Settings.Name, Settings.WindowWidth, Settings.WindowHeight)
			m_AppWindow->SetEventCallback(SE_BIND_EVENT(Application::SetEvent));
		SE_LOG_DEBUG("{}: Window EventCallback Binded", m_AppWindow->GetTitle())
			m_AppWindow->SetVsync(true); 
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
		delete Myserver;
		delete Myclient;
		delete m_Renderer;
		m_Renderer = nullptr;

	}


	void Application::PrepareAsset(std::string name ,AssetType type, bool DepthStencil)
	{
		VkGLTFAsset* Asset = new VkGLTFAsset(name, type, DepthStencil, getProjectDirectory());
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
#if 0
				if (textureIndexDebugging==0 || textureIndexDebugging>244)
					textureIndexDebugging = 1;

				m_Renderer->RefreshTextureFromFile(m_Assets[0], textureIndexDebugging);
				m_Renderer->RecordCommandBuffers(&m_Assets);
				Render();
				textureIndexDebugging++;
#endif
				Render();
			}
			m_Timestep = (float)glfwGetTime() - m_lastframetime;
			m_lastframetime = (float)glfwGetTime();	
		}
	}
	
	void Application::SetEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(SE_BIND_EVENT(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(SE_BIND_EVENT(Application::OnWindowResize));	
		dispatcher.Dispatch<KeyPressedEvent>(SE_BIND_EVENT(Application::OnKeyPressed));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		//todo : Resize viewport & ImGui Pannels
		uint32_t width = e.GetWidth(); //Assuming 1 Display per Window
		uint32_t height = e.GetHeight();

		if (width == 0 && height == 0)
		{
			m_Minimized = true;
		}
		else
		{
			m_Renderer->GenerateDisplay(&width, &height, m_AppWindow->GetVsync(), &m_Assets);
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
		case Key::A:
			m_Renderer->RefreshTextureFromBuffer(m_Assets[0], textureIndexDebugging, Myserver->Queue.front());
			m_Renderer->RecordCommandBuffers(&m_Assets);
			break;
		case Key::D:
			Myclient->SendImgData();
			break;
		case Key::F:
			m_Renderer->RefreshTextureFromFile(m_Assets[0], 27);
			m_Renderer->RecordCommandBuffers(&m_Assets);
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
		std::filesystem::path Parent = ExecutablePath.parent_path().parent_path().parent_path().parent_path();
		std::string appName = ExecutablePath.filename().string();
		size_t lastDotPos = appName.rfind(".");
		if (lastDotPos != std::string::npos) {
			appName = appName.substr(0, lastDotPos);
		}
		std::filesystem::path ReleativePath = Parent / appName;
		return ReleativePath.string();
	}
}