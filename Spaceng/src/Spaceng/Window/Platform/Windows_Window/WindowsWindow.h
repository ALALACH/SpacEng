#pragma once
#include "Spaceng/Window/Window.h"
#include <glfw/include/GLFW/glfw3.h>


#define GLFW_INCLUDE_VULKAN

namespace Spaceng {

	static bool s_glfwInit = false;

	struct WindowData
	{
		std::string Tittle;
		uint32_t Width, Height;
		int posx, posy;
		bool Vsync = false;
		bool Fullscreen = false;
		bool UIOverlay = false;
		EventCallbackFn EventCallback;
	}Window_Data;

	class WindowsWindow : public Window
	{
	public :
		WindowsWindow(const WindowSettings& Settings);
		virtual ~WindowsWindow();
		virtual void ShutDownWin() override;


		virtual void InitWindow(VulkanRenderer* Renderer) override;
		virtual void PollEvents() override;

		virtual std::pair<int, int> GetPos() const override;
		virtual void SetPos(int Xpos, int Ypos) override;
		virtual void SetToFullScreen() override;


		virtual void SetEventCallback(const EventCallbackFn& callback) override { Window_Data.EventCallback = callback; }
		virtual void SetVsync(bool Enabled) override;
		virtual bool IsVsync() const override;
		virtual void UpdateTittle(const std::string tittle)  override;


		inline std::pair<uint32_t, uint32_t> GetSize() const  override { return { Window_Data.Height , Window_Data.Width }; }
		inline  uint32_t* GetWidth()  const override { return &Window_Data.Width; }
		inline  uint32_t* GetHeight() const override { return &Window_Data.Height; }
		inline bool GetVsync() const override { return Window_Data.Vsync; }
		inline bool GetFullScreen() const override { return Window_Data.Fullscreen; }
		inline bool GetUIOverlay() const override { return Window_Data.UIOverlay; }
		inline const std::string GetTitle() const override { return Window_Data.Tittle; }
		inline virtual GLFWwindow* GetGlfwWindow() const override { return m_Window; }


	private:
		GLFWwindow* m_Window;
		WindowSettings m_Settings;
		VulkanRenderer* m_Renderer;
	};





}