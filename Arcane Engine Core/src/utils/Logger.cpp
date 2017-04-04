#include "Logger.h"

namespace arcane {

	Logger::Logger(const std::string &filePath) {
		file = filePath;
	}

	void Logger::logMessage(const int &priority, const std::string &module, const std::string &message) {
		filestream.open(file, std::ofstream::app);
		if (!filestream) {
			std::cout << "Error: Logger Can't Log To: " << file.c_str() << std::endl;
		}
		filestream << "(" << module.c_str() << "): " << message.c_str() << std::endl;
		filestream.close();
	}

	void Logger::clearFileContents() {
		filestream.open(file, std::ofstream::out);
		if (!filestream) {
			std::cout << "Error: Logger Couldn't Clear File: " << file.c_str() << std::endl;
		}
		filestream.close();
	}

}
