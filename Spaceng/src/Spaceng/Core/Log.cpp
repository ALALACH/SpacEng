#include "PCH.h"
#include "Log.h"



 namespace Spaceng {


	 std::shared_ptr<spdlog::logger> log::s_logger;

	 void log::init()
	 {
		 spdlog::set_pattern("%^[%T] : %v%$");
		 s_logger = spdlog::stdout_color_mt("LOG");
		 s_logger->set_level(spdlog::level::debug);
	 }
}