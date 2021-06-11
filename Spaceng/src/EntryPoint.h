#pragma once
#include "Spaceng.h"
#include <iostream>

#ifdef SE_PLATFORM_WIN

extern Spaceng::Application* Spaceng::CreateApplication();

int main(int argc, char** argv)
{
	Spaceng::log::init();
	SE_LOG_INFO("init...")
	Spaceng::Application* app = Spaceng::CreateApplication();
	app->Run();
	SE_LOG_CRITICAL("Shutdown...")
	delete app;
}
#endif