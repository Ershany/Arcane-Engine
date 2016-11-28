#pragma once

#include <string>

namespace arcane { 

	class FileUtils {
	public:
		static std::string readFile(const char *filepath);
	};

} 