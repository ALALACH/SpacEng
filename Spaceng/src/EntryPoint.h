#pragma once
#ifdef SE_PLATFORM_WIN

extern Spaceng::Application* Spaceng::CreateApplication();

	int main(int argc, char** argv)
	{
		Spaceng::log::init();
		SE_LOG_INFO("init...")
		// Project Specified Extern
		Spaceng::Application* app = Spaceng::CreateApplication();
		app->OnInit();
		app->Run();
		app->OnShutdown();
		delete app;
		SE_LOG_CRITICAL("Shutdown...");
		return 0;
	}
#endif