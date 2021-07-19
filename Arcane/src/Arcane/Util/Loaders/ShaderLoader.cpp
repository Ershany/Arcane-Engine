#include "arcpch.h"
#include "ShaderLoader.h"

#include <Arcane/Graphics/Shader.h>

namespace Arcane
{
	// Static declarations
	std::string ShaderLoader::s_ShaderFilepath;
	std::unordered_map<std::size_t, Shader*> ShaderLoader::s_ShaderCache;
	std::hash<std::string> ShaderLoader::s_Hasher;

	Shader* ShaderLoader::LoadShader(const std::string &path) {
		std::string shaderPath = s_ShaderFilepath + path;
		std::size_t hash = s_Hasher(shaderPath);

		// Check the cache
		auto iter = s_ShaderCache.find(hash);
		if (iter != s_ShaderCache.end()) {
			return iter->second;
		}

		// Load the shader
		Shader *shader = new Shader(shaderPath);

		s_ShaderCache.insert(std::pair<std::size_t, Shader*>(hash, shader));
		return s_ShaderCache[hash];
	}
}
