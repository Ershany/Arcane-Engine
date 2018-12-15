#include "pch.h"
#include "Logger.h"

namespace arcane {

	Logger::Logger() {
		file = "logged_files/log.txt";
	}

	Logger& Logger::getInstance() {
		static Logger logger;
		return logger;
	}

	void Logger::setOutputFile(const std::string &filename) {
		file = filename;

		// Add this file if it hasn't been written to yet
		if (std::find(filePaths.begin(), filePaths.end(), filename) == filePaths.end()) {
			filePaths.push_back(filename);
			clearFileContents();
		}
	}

	void Logger::debug(const std::string &filePath, std::string &module, const std::string &message) {
		setOutputFile(filePath);
		logMessage(DEBUG, module, message);
	}

	void Logger::info(const std::string &filePath, const std::string &module, const std::string &message) {
		setOutputFile(filePath);
		logMessage(INFO, module, message);
	}

	void Logger::warning(const std::string &filePath, const std::string &module, const std::string &message) {
		setOutputFile(filePath);
		logMessage(WARNING, module, message);
	}

	void Logger::error(const std::string &filePath, const std::string &module, const std::string &message) {
		setOutputFile(filePath);
		logMessage(ERROR, module, message);
	}

	void Logger::logMessage(const int &priority, const std::string &module, const std::string &message) {
		std::cout << module.c_str() << " : " << message.c_str() << std::endl;
		filestream.open(file, std::ofstream::app);
		if (!filestream) {
			std::cout << "Error: Logger Can't Log To: " << file.c_str() << std::endl;
		}
		filestream << "(" << module.c_str() << "): " << message.c_str() << std::endl;
		filestream.close();
	}

	// TODO: This function will clear the same file multiple times
	void Logger::clearFileContents() {
		filestream.open(file, std::ofstream::out);
		if (!filestream) {
			error("logged_files/log.txt", "Logger Dtor", "Could not empty the contents of file: " + file);
		}
		filestream.close();
	}

}
