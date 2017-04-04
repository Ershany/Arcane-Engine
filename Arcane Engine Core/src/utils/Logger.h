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
		std::string file; // Default value set to: "logged_files/log.txt"
	public:
		Logger(const std::string &filePath = "logged_files/log.txt");

		/**
		* Sets the output file
		*
		* @param filename The file that you want to set the logger to output to
		*/
		inline void setOutputFile(const std::string &filename) {
			file = filename;
		}

		/**
		* Clears out the contents in the file
		*/
		void clearFileContents();

		/**
		* Logs an debug message
		*
		* @param module The module the debug information is assosciated with
		* @param message The debug message that will be logged
		*/
		inline void debug(const std::string &module, const std::string &message) {
			logMessage(DEBUG, module, message);
		}

		/**
		* Logs an information message
		*
		* @param module The module the info is assosciated with
		* @param message The info message that will be logged
		*/
		inline void info(const std::string &module, const std::string &message) {
			logMessage(INFO, module, message);
		}

		/**
		* Logs an warning message
		*
		* @param module The module the warning is assosciated with
		* @param message The warning message that will be logged
		*/
		inline void warning(const std::string &module, const std::string &message) {
			logMessage(WARNING, module, message);
		}

		/**
		* Logs an error message
		*
		* @param module The module the error is assosciated with
		* @param message The error message that will be logged
		*/
		inline void error(const std::string &module, const std::string &message) {
			logMessage(ERROR, module, message);
		}
	private:
		/**
		* Logs a message
		*
		* @param priority The level of priority that the message is given (higher = more important)
		* @param module The module the message is assosciated with
		* @param message The message that will be logged
		*/
		void logMessage(const int &priority, const std::string &module, const std::string &message);
	};

}