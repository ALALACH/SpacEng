#pragma once
#ifdef SE_PLATFORM_WIN

extern Spaceng::Application* Spaceng::CreateApplication(int argc, char** argv);

	int main(int argc, char** argv)
	{
		Spaceng::log::init();
		SE_LOG_INFO("init...")

		std::filesystem::path exePath = std::filesystem::canonical(std::filesystem::path(argv[0]));
		std::filesystem::path Parent = exePath.parent_path().parent_path().parent_path().parent_path();
		std::string appName = exePath.filename().string();
		size_t lastDotPos = appName.rfind(".");
		if (lastDotPos != std::string::npos) {
			appName = appName.substr(0, lastDotPos);
		}
		std::filesystem::path ReleativePath = Parent / appName;
		SE_LOG_DEBUG("appName : {0}", appName)
		SE_LOG_DEBUG("EXEPath : {0}", exePath)
		SE_LOG_DEBUG("ParentPath : {0}", Parent)
		SE_LOG_DEBUG("ReleativeTexturePath : {0}", ReleativePath)

		// Project Specified Extern
		Spaceng::Application* app = Spaceng::CreateApplication(argc,argv);
		app->Run();
		app->OnShutdown();
		delete app;
		SE_LOG_CRITICAL("Shutdown...");
		return 0;
	}
#endif