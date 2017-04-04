#pragma once

#include <iostream>
#include <fstream>

namespace arcane {

	class Logger {
	private:
		enum {
			DEBUG, INFO, WARNING, ERROR
		};

		std::ofstream filestream;
		std::string file; // Default value set to: "info/log.txt"
	public:
		Logger(const std::string &filePath = "info/log.txt");

		inline void setOutputFile(const std::string &filename) {
			file = filename;
		}

		inline void debug(const std::string &module, const std::string &message) {
			logMessage(DEBUG, module, message);
		}

		inline void info(const std::string &module, const std::string &message) {
			logMessage(INFO, module, message);
		}

		inline void warning(const std::string &module, const std::string &message) {
			logMessage(WARNING, module, message);
		}

		inline void error(const std::string &module, const std::string &message) {
			logMessage(ERROR, module, message);
		}
	private:
		void logMessage(const int &priority, const std::string &module, const std::string &message);
	};

}