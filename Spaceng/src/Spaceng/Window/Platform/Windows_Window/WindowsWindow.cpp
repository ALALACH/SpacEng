#include "PCH.h"
#include "WindowsWindow.h"


namespace Spaceng {

	static void glfw_error_callback(int error, const char* description)
	{
		fprintf(stderr, "Glfw Error %d: %s\n", error, description);
	}

	Window* Window::Create(const WindowSettings& Settings)
	{
		return new WindowsWindow(Settings);
	}
	
	WindowsWindow::WindowsWindow(const WindowSettings& Settings)
		:m_Settings(Settings)
	{
	}

	WindowsWindow::~WindowsWindow()
	{
		glfwTerminate();
		s_glfwInit = false;
	}

	void WindowsWindow::InitWindow(VulkanRenderer* Renderer)
	{
		Window_Data.Tittle = m_Settings.Tittle;
		Window_Data.Width = m_Settings.Width;
		Window_Data.Height = m_Settings.Height;
		Window_Data.posx = m_Settings.Posx;
		Window_Data.posy = m_Settings.Posy;

		if(!s_glfwInit)
		{ 
			glfwInit();
			glfwSetErrorCallback(glfw_error_callback);
			s_glfwInit = true;
		}
		SE_ASSERT(glfwVulkanSupported(), "GLFW not supported");
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

		m_Window = glfwCreateWindow(Window_Data.Width, Window_Data.Height, Window_Data.Tittle.c_str(), nullptr, nullptr);
		glfwSetWindowPos(m_Window, Window_Data.posx, Window_Data.posy);

		
		Renderer->CreateSurfacePrimitives(m_Window);
		Renderer->CreateDisplayTemplate(&Window_Data.Width,&Window_Data.Height,Window_Data.Vsync);

		glfwSetWindowUserPointer(m_Window, &Window_Data);


		/* Setting glfw Callbacks */
		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				auto& data = *(WindowData*)glfwGetWindowUserPointer(window);;
				WindowCloseEvent e(window);
				data.EventCallback(e);
				SE_LOG_DEBUG("Window Closed EventCallback")
			});

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				auto& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowResizeEvent e((uint32_t)width,(uint32_t)height);
				data.EventCallback(e);
				data.Height = height;
				data.Width = width;
				SE_LOG_DEBUG("Window Resize EventCallback :({0},{1})",height,width)
			});
		
		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				auto& data = *(WindowData*)glfwGetWindowUserPointer(window);
				switch (action)
				{ 
					case GLFW_PRESS:
					{
						MouseButtonPressedEvent e((Code)button);
						data.EventCallback(e);
						SE_LOG_DEBUG("MousePressed EventCallback")
						break;
					}
					case GLFW_RELEASE:
					{
						MouseButtonReleasedEvent e((Code)button);
						data.EventCallback(e);
						SE_LOG_DEBUG("Mouse Released EventCallback")
						break;
					}
				}
			});
		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				auto& data = *(WindowData*)glfwGetWindowUserPointer(window);
				switch (action)
				{
					case GLFW_PRESS:
					{
						KeyPressedEvent e((Code)key, 0);
						data.EventCallback(e);
						SE_LOG_DEBUG("{0} key Pressed EventCallback",(char)key)
						break;
					}
					case GLFW_REPEAT:
					{
						KeyPressedEvent e((Code)key, 1);
						data.EventCallback(e);
						SE_LOG_DEBUG("key OnRepeat EventCallback")
						break;
					}
					case GLFW_RELEASE:
					{
						keyReleasedEvent e((Code)key);
						data.EventCallback(e);
						SE_LOG_DEBUG("key Released EventCallback")
						break;
					}
					}
			});
		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				auto& data = *((WindowData*)glfwGetWindowUserPointer(window));

				MouseScrolledEvent event((float)xOffset, (float)yOffset);
				data.EventCallback(event);
				SE_LOG_DEBUG("MouseScrolled EventCallback")
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double XPOS, double YPOS)
			{
				auto& data = *((WindowData*)glfwGetWindowUserPointer(window));

				MouseMovedEvent event((float)XPOS, (float)YPOS);
				data.EventCallback(event);

			});   
		
	}


	void WindowsWindow::ShutDownWin()
	{
		glfwDestroyWindow(m_Window);
	}

	void WindowsWindow::PollEvents()
	{
		glfwPollEvents();
	}
	
	

	std::pair<int, int> WindowsWindow::GetPos() const
	{
		glfwGetWindowPos(m_Window, &Window_Data.posx, &Window_Data.posy);
		return { Window_Data.posx, Window_Data.posy };
	}
	
	void WindowsWindow::SetPos(int Xpos , int Ypos)
	{
		glfwSetWindowPos(m_Window, Xpos, Ypos);
	}
	void WindowsWindow::SetToFullScreen()
	{
		glfwMaximizeWindow(m_Window);
	}

	void WindowsWindow::SetVsync(bool Enabled)
	{
		/*if (Enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);*/
		Window_Data.Vsync = Enabled;	
	}

	bool WindowsWindow::IsVsync() const
	{
		return Window_Data.Vsync;
	}

	void WindowsWindow::UpdateTittle(const std::string tittle)
	{
		Window_Data.Tittle = tittle;
		glfwSetWindowTitle(m_Window, Window_Data.Tittle.c_str());
	}
}