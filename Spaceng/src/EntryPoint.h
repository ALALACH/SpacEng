#pragma once
#ifdef SE_PLATFORM_WIN

extern Spaceng::Application* Spaceng::CreateApplication(int argc, char** argv);

	int main(int argc, char** argv)
	{
		Spaceng::log::init();
		SE_LOG_INFO("init...")
		Spaceng::Application* app = Spaceng::CreateApplication(argc,argv);
		app->Run();
		app->OnShutdown();
		delete app;
		SE_LOG_CRITICAL("Shutdown...");
		return 0;
	}
#endif