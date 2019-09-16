#pragma once

#include <graphics/Shader.h>

namespace arcane {

	class ShaderLoader {
	public:
		static Shader* loadShader(const std::string &path);
	private:
		static std::unordered_map<std::size_t, Shader*> s_ShaderCache;
		static std::hash<std::string> s_Hasher;
	};

}
