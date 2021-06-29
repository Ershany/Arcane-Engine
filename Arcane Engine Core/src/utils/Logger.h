#pragma once

#include "Singleton.h"

namespace arcane
{
	class Logger : public Singleton {
	private:
		Logger();
	public:
		static Logger& getInstance();

		void debug(const std::string &filePath, std::string &module, const std::string &message);
		void info(const std::string &filePath, const std::string &module, const std::string &message);
		void warning(const std::string &filePath, const std::string &module, const std::string &message);
		void error(const std::string &filePath, const std::string &module, const std::string &message);
	private:
		void logMessage(const int &priority, const std::string &module, const std::string &message);

		void clearFileContents();
		void setOutputFile(const std::string &filename);

		enum {
			DEBUG, INFO, WARNING, ERROR
		};
		std::set<std::string> filePaths;

		std::ofstream filestream;
		std::string file; // Default value set to: "logged_files/log.txt"
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
