#include "pch.h"
#include "FileUtils.h"

namespace arcane {

	std::string FileUtils::readFile(const char *filepath) {
		std::ifstream ifs(filepath);
		std::string result((std::istreambuf_iterator<char>(ifs)),
			(std::istreambuf_iterator<char>()));

		return result;
	}

}
