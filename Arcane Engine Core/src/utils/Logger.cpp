#include "Logger.h"

namespace arcane {

	Logger::Logger(const std::string &filePath) {
		file = filePath;
	}

	void Logger::logMessage(const int &priority, const std::string &module, const std::string &message) {
		filestream.open(file, std::ofstream::out);
		filestream << "(" << module.c_str() << "): " << message.c_str();
		filestream.close();
	}

}
