#pragma once
#include "PCH.h"
#include "Spaceng/Core/Event.h"
#include "Spaceng/Core/Core.h"


namespace Spaceng {

	using EventCallbackFn = std::function<void(Event&)>;

	struct WindowSettings
	{
		std::string Tittle;
		unsigned int Width;
		unsigned int Height;
		unsigned int Posx;
		unsigned int Posy;

		WindowSettings(const std::string& tittle = "SpacEng"
			, unsigned int width = 1280 
			, unsigned int height=720
			, unsigned int posx=250
			, unsigned int posy=250)
			:Tittle(tittle), Width(width), Height(height), Posx(posx), Posy(posy)
		{}
	};

	class Window
	{
	public:


		static Window* Create(const WindowSettings& Settings =  WindowSettings() );
		virtual ~Window() {}
		virtual void ShutDownWin() = 0;

		virtual void OnUpdate(float Timestep) = 0;

		virtual std::pair<uint32_t, uint32_t> GetSize() const = 0;
		virtual std::pair<int, int> GetPos() const = 0;
		virtual void SetPos(int Xpos, int Ypos) =0;
		virtual void SetToFullScreen() = 0;
		virtual uint32_t GetWidth()  const =0 ;
		virtual uint32_t GetHeight() const = 0 ;


		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVsync(bool Enabled) = 0;
		virtual bool IsVsync() const = 0;


		virtual const std::string GetTitle() const = 0;
		virtual void UpdateTittle(const std::string tittle) = 0;

		virtual void* GetGlfwWindow() const = 0;
	};
}