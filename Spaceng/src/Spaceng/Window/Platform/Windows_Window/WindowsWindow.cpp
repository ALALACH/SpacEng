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
	{
		Init(Settings);
	}
	
	void WindowsWindow::Init(const WindowSettings& Settings)
	{
		m_Data.Tittle = Settings.Tittle;
		m_Data.Width = Settings.Width;
		m_Data.Height = Settings.Height;
		m_Data.posx = Settings.Posx;
		m_Data.posy = Settings.Posy;

		if(!s_glfwInit)
		{ 
			glfwInit();
			glfwSetErrorCallback(glfw_error_callback);
			s_glfwInit = true;
		}

		m_Window = glfwCreateWindow(m_Data.Width, m_Data.Height, m_Data.Tittle.c_str(), nullptr, nullptr);
		glfwSetWindowPos(m_Window, m_Data.posx, m_Data.posy);

		glfwMakeContextCurrent(m_Window);
		glfwSetWindowUserPointer(m_Window, &m_Data);


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
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double XPOS, double YPOS)
			{
				auto& data = *((WindowData*)glfwGetWindowUserPointer(window));

				MouseMovedEvent event((float)XPOS, (float)YPOS);
				data.EventCallback(event);
			});
		
	}

	WindowsWindow::~WindowsWindow()
	{
	}

	void WindowsWindow::ShutDownWin()
	{
		glfwDestroyWindow(m_Window);
	}

	void WindowsWindow::OnUpdate(float Timestep)
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	std::pair<int, int> WindowsWindow::GetPos() const
	{
		glfwGetWindowPos(m_Window, &m_Data.posx, &m_Data.posy);
		return { m_Data.posx, m_Data.posy };
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
		if (Enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);
		m_Data.Vsync = Enabled;	
		SE_ASSERT(m_Data.Vsync,"vSync not enabled Err");
	}

	bool WindowsWindow::IsVsync() const
	{
		return m_Data.Vsync;
	}

	void WindowsWindow::UpdateTittle(const std::string tittle)
	{
		m_Data.Tittle = tittle;
		glfwSetWindowTitle(m_Window, m_Data.Tittle.c_str());
	}
}