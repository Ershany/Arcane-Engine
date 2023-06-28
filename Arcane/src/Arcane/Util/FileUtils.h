#pragma once
#ifndef FILEUTILS_H
#define FILEUTILS_H

namespace Arcane
{ 
	class FileUtils
	{
	public:
		static std::string ReadFile(const std::string &filepath);
	};
}
#endif
