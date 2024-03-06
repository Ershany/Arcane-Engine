#include "arcpch.h"
#include "FileUtils.h"

namespace Arcane
{
	std::string FileUtils::ReadFile(const std::string &filepath)
	{
		std::ifstream ifs(filepath, std::ios::in, std::ios::binary);
		std::string result;

		if (ifs)
		{
			result = std::string((std::istreambuf_iterator<char>(ifs)),
				(std::istreambuf_iterator<char>()));
			ifs.close();
		}
		else
		{
			ARC_LOG_WARN("Failed to read file: {0}", filepath);
		}

		return result;
	}

	char* FileUtils::ReadBinaryFile(const char *fileName, int &size)
	{
		FILE *f = nullptr;
		errno_t err = fopen_s(&f, fileName, "rb");
		if (!f)
		{
			ARC_LOG_ERROR("Could not open file: {0}", fileName);
			return nullptr;
		}

		struct stat statBuff;
		int error = stat(fileName, &statBuff);
		if (error)
		{
			ARC_LOG_ERROR("Error getting file size on file: {0}", fileName);
			return nullptr;
		}

		size = statBuff.st_size;
		char *p = (char*)malloc(size);
		size_t bytesRead = fread(p, 1, size, f);
		if (bytesRead != size)
		{
			ARC_LOG_ERROR("File read error on file: {0}", fileName);
			return nullptr;
		}
		fclose(f);

		return p;
	}
}
