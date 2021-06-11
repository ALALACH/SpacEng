#pragma once
#include "Spaceng/Window/Window.h"
#include <glfw/include/GLFW/glfw3.h>

namespace Spaceng {

	static bool s_glfwInit = false;

	struct WindowData
	{
		std::string Tittle;
		uint32_t Width, Height;
		int posx, posy;
		bool Vsync;
		EventCallbackFn EventCallback;
	}m_Data;

	class WindowsWindow : public Window
	{
	public :
		WindowsWindow(const WindowSettings& Settings);
		virtual ~WindowsWindow();
		virtual void ShutDownWin() override;

	
		virtual void OnUpdate(float Timestep) override;

		virtual std::pair<int, int> GetPos() const override;
		virtual void SetPos(int Xpos, int Ypos) override;
		virtual void SetToFullScreen() override;


		virtual void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		virtual void SetVsync(bool Enabled) override;
		virtual bool IsVsync() const override;
		virtual void UpdateTittle(const std::string tittle)  override;


		inline std::pair<uint32_t, uint32_t> GetSize() const  override { return { m_Data.Height , m_Data.Width }; }
		inline  uint32_t GetWidth()  const override { return m_Data.Width; }
		inline  uint32_t GetHeight() const override { return m_Data.Height; }
		inline const std::string GetTitle() const override { return m_Data.Tittle; }
		inline virtual void* GetGlfwWindow() const override { return m_Window; }
		
	
	private:
		void Init(const WindowSettings& Settings);


	private:
		GLFWwindow* m_Window;
		GLFWmonitor* m_Monitor;
	};





}