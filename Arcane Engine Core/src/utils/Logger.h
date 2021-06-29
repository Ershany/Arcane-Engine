#pragma once

#include "Singleton.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace arcane
{
	class Logger : public Singleton
	{
	private:
		Logger();
		~Logger();
	public:
		static Logger& GetInstance();
		inline static std::shared_ptr<spdlog::logger> GetEngineLogger() { return GetInstance().s_EngineLogger; }
	private:
		static void Init();
		static void Shutdown();
	private:
		static std::shared_ptr<spdlog::logger> s_EngineLogger;
	};
}

// Engine Log Macros
#ifdef ARC_FINAL
#define ARC_LOG_FATAL
#define ARC_LOG_ERROR
#define ARC_LOG_WARN
#define ARC_LOG_INFO
#define ARC_LOG_TRACE
#else
#define ARC_LOG_FATAL(...) arcane::Logger::GetEngineLogger()->critical(__VA_ARGS__)
#define ARC_LOG_ERROR(...) arcane::Logger::GetEngineLogger()->error(__VA_ARGS__)
#define ARC_LOG_WARN(...)  arcane::Logger::GetEngineLogger()->warn(__VA_ARGS__)
#define ARC_LOG_INFO(...)  arcane::Logger::GetEngineLogger()->info(__VA_ARGS__)
#define ARC_LOG_TRACE(...) arcane::Logger::GetEngineLogger()->trace(__VA_ARGS__)
#endif
