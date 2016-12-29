#pragma once

#include <string>

namespace arcane { 

	class FileUtils {
	public:
		/**
		* Opens a filestream and returns the contents of the file
		*
		* @param filepath The filepath of the file that will be read. ie src/shaders/someShader.vert
		* @return returns a string the contains the contents of the file
		*/
		static std::string readFile(const char *filepath);
	};

} 