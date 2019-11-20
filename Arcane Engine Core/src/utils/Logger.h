#pragma once

#include "Singleton.h"

namespace arcane {

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
