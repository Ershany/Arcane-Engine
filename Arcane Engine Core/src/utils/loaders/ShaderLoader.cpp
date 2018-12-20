#include "pch.h"
#include "ShaderLoader.h"

namespace arcane {

	// Static declarations
	std::unordered_map<std::size_t, Shader*> ShaderLoader::s_ShaderCache;
	std::hash<std::string> ShaderLoader::s_Hasher;

	Shader* ShaderLoader::loadShader(const char *vertPath, const char *fragPath) {
		return loadShader(vertPath, fragPath, "");
	}

	Shader* ShaderLoader::loadShader(const char *vertPath, const char *fragPath, const char *geoPath) {
		std::string shaderName = std::string(vertPath) + std::string(fragPath) + std::string(geoPath);
		std::size_t hash = s_Hasher(shaderName);

		// Check the cache
		auto iter = s_ShaderCache.find(hash);
		if (iter != s_ShaderCache.end()) {
			return iter->second;
		}

		// Load the shader
		Shader *shader = new Shader(vertPath, fragPath, geoPath);

		s_ShaderCache.insert(std::pair<std::size_t, Shader*>(hash, shader));
		return s_ShaderCache[hash];
	}

}
