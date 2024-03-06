#pragma once
#ifndef FILEUTILS_H
#define FILEUTILS_H

namespace Arcane
{ 
	class FileUtils
	{
	public:
		static std::string ReadFile(const std::string &filepath);

		// Caller is responsible for deleting the dynamically allocated file when it is done with it
		static char* ReadBinaryFile(const char *fileName, int &size);
	};
}
#endif
