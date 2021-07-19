#pragma once

namespace Arcane
{
	class Shader;

	class ShaderLoader {
	public:
		static Shader* LoadShader(const std::string &path);
		inline static void SetShaderFilepath(const std::string &path) { s_ShaderFilepath = path; }
	private:
		static std::string s_ShaderFilepath;
		static std::unordered_map<std::size_t, Shader*> s_ShaderCache;
		static std::hash<std::string> s_Hasher;
	};
}
