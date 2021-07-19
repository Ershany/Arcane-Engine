#include "arcpch.h"
#include "Logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace Arcane
{
	std::shared_ptr<spdlog::logger> Logger::s_EngineLogger;

	Logger::Logger()
	{
		Init();
	}

	Logger::~Logger()
	{
		Shutdown();
	}

	Logger& Logger::GetInstance()
	{
		static Logger logger;
		return logger;
	}

	void Logger::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");

		s_EngineLogger = spdlog::stdout_color_mt("Arcane");
		s_EngineLogger->set_level(spdlog::level::trace);
	}

	void Logger::Shutdown()
	{
		s_EngineLogger.reset();
		spdlog::drop_all();
	}
}
