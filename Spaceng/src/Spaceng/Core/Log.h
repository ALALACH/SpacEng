#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"





namespace Spaceng {


	class log
	{
	public:

		static void init();
		inline static std::shared_ptr<spdlog::logger>& GetLogger() { return s_logger; };

	private:
		static std::shared_ptr<spdlog::logger> s_logger;
	};
}



