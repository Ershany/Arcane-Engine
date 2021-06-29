#include "pch.h"
#include "FileUtils.h"

namespace arcane {

	std::string FileUtils::readFile(const std::string &filepath) {
		std::ifstream ifs(filepath, std::ios::in, std::ios::binary);
		std::string result;

		if (ifs) {
			result = std::string((std::istreambuf_iterator<char>(ifs)),
				(std::istreambuf_iterator<char>()));
			ifs.close();
		}
		else {
			ARC_LOG_WARN("Failed to read file: {0}", filepath);
		}

		return result;
	}

}
