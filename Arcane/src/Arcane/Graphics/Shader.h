#pragma once

#include <Arcane/Util/Loaders/ShaderLoader.h>

namespace Arcane
{
	class Shader
	{
		friend class ShaderLoader;
	private:
		Shader(const std::string &path);
	public:
		~Shader();

		void Enable() const;
		void Disable() const;

		void SetUniform(const char *name, float value);
		void SetUniform(const char *name, int value);
		void SetUniform(const char *name, const glm::vec2& vector);
		void SetUniform(const char *name, const glm::ivec2& vector);
		void SetUniform(const char *name, const glm::vec3& vector);
		void SetUniform(const char *name, const glm::ivec3& vector);
		void SetUniform(const char *name, const glm::vec4& vector);
		void SetUniform(const char *name, const glm::ivec4& vector);
		void SetUniform(const char *name, const glm::mat3& matrix);
		void SetUniform(const char *name, const glm::mat4& matrix);

		void SetUniformArray(const char *name, int arraySize, const float *value);
		void SetUniformArray(const char *name, int arraySize, const int *value);
		void SetUniformArray(const char *name, int arraySize, const glm::vec2 *value);
		void SetUniformArray(const char *name, int arraySize, const glm::ivec2 *value);
		void SetUniformArray(const char *name, int arraySize, const glm::vec3 *value);
		void SetUniformArray(const char *name, int arraySize, const glm::ivec3 *value);
		void SetUniformArray(const char *name, int arraySize, const glm::vec4 *value);
		void SetUniformArray(const char *name, int arraySize, const glm::ivec4 *value);
		void SetUniformArray(const char *name, int arraySize, const glm::mat3 *value);
		void SetUniformArray(const char *name, int arraySize, const glm::mat4 *value);

		inline unsigned int GetShaderID() { return m_ShaderID; }
	private:
		int GetUniformLocation(const char *name);

		static GLenum ShaderTypeFromString(const std::string &type);
		std::unordered_map<GLenum, std::string> PreProcessShaderBinary(std::string &source);
		void Compile(const std::unordered_map<GLenum, std::string> &shaderSources);
	private:
		unsigned int m_ShaderID;
		std::string m_ShaderFilePath;
	};
}
