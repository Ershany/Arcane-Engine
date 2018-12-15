#pragma once

#include "Singleton.h"

namespace arcane {

	class Logger : public Singleton {
	private:
		Logger();
	public:
		static Logger& getInstance();

		/**
		* Logs an debug message
		*
		* @param module The module the debug information is assosciated with
		* @param message The debug message that will be logged
		*/
		void debug(const std::string &filePath, std::string &module, const std::string &message);

		/**
		* Logs an information message
		*
		* @param module The module the info is assosciated with
		* @param message The info message that will be logged
		*/
		void info(const std::string &filePath, const std::string &module, const std::string &message);

		/**
		* Logs an warning message
		*
		* @param module The module the warning is assosciated with
		* @param message The warning message that will be logged
		*/
		void warning(const std::string &filePath, const std::string &module, const std::string &message);

		/**
		* Logs an error message
		*
		* @param module The module the error is assosciated with
		* @param message The error message that will be logged
		*/
		void error(const std::string &filePath, const std::string &module, const std::string &message);
	private:
		/**
		* Logs a message
		*
		* @param priority The level of priority that the message is given (higher = more important)
		* @param module The module the message is assosciated with
		* @param message The message that will be logged
		*/
		void logMessage(const int &priority, const std::string &module, const std::string &message);

		/**
		* Clears out the contents all of the different files that have been assigned to
		*/
		void clearFileContents();

		/**
		* Sets the output file
		*
		* @param filename The file that you want to set the logger to output to
		*/
		void setOutputFile(const std::string &filename);

		enum {
			DEBUG, INFO, WARNING, ERROR
		};
		std::vector<std::string> filePaths;

		std::ofstream filestream;
		std::string file; // Default value set to: "logged_files/log.txt"
	};

}
