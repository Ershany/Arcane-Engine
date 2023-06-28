#include "arcpch.h"
#include "Shader.h"

#include <Arcane/Util/Loaders/ShaderLoader.h>
#include <Arcane/Util/FileUtils.h>

namespace Arcane
{
	Shader::Shader(const std::string &path) : m_ShaderFilePath(path) {
		std::string shaderBinary = FileUtils::ReadFile(m_ShaderFilePath);
		auto shaderSources = PreProcessShaderBinary(shaderBinary);
		Compile(shaderSources);
	}

	Shader::~Shader() {
		glDeleteProgram(m_ShaderID);
	}

	void Shader::Enable() const {
		glUseProgram(m_ShaderID);
	}

	void Shader::Disable() const {
		glUseProgram(0);
	}

	void Shader::SetUniform(const char* name, float value) {
		glUniform1f(GetUniformLocation(name), value);
	}

	void Shader::SetUniform(const char* name, int value) {
		glUniform1i(GetUniformLocation(name), value);
	}

	void Shader::SetUniform(const char* name, const glm::vec2& vector) {
		glUniform2f(GetUniformLocation(name), vector.x, vector.y);
	}

	void Shader::SetUniform(const char *name, const glm::ivec2& vector) {
		glUniform2i(GetUniformLocation(name), vector.x, vector.y);
	}

	void Shader::SetUniform(const char* name, const glm::vec3& vector) {
		glUniform3f(GetUniformLocation(name), vector.x, vector.y, vector.z);
	}

	void Shader::SetUniform(const char *name, const glm::ivec3& vector) {
		glUniform3i(GetUniformLocation(name), vector.x, vector.y, vector.z);
	}

	void Shader::SetUniform(const char* name, const glm::vec4& vector) {
		glUniform4f(GetUniformLocation(name), vector.x, vector.y, vector.z, vector.w);
	}

	void Shader::SetUniform(const char* name, const glm::ivec4& vector) {
		glUniform4i(GetUniformLocation(name), vector.x, vector.y, vector.z, vector.w);
	}

	void Shader::SetUniform(const char* name, const glm::mat3& matrix) {
		glUniformMatrix3fv(glGetUniformLocation(m_ShaderID, name), 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void Shader::SetUniform(const char* name, const glm::mat4& matrix) {
		glUniformMatrix4fv(glGetUniformLocation(m_ShaderID, name), 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void Shader::SetUniformArray(const char *name, int arraySize, const float *value) {
		glUniform1fv(glGetUniformLocation(m_ShaderID, name), arraySize, value);
	}

	void Shader::SetUniformArray(const char *name, int arraySize, const int *value) {
		glUniform1iv(glGetUniformLocation(m_ShaderID, name), arraySize, value);
	}

	void Shader::SetUniformArray(const char *name, int arraySize, const glm::vec2 *value) {
		glUniform2fv(glGetUniformLocation(m_ShaderID, name), arraySize, glm::value_ptr(*value));
	}

	void Shader::SetUniformArray(const char *name, int arraySize, const glm::ivec2 *value) {
		glUniform2iv(glGetUniformLocation(m_ShaderID, name), arraySize, glm::value_ptr(*value));
	}

	void Shader::SetUniformArray(const char *name, int arraySize, const glm::vec3 *value) {
		glUniform3fv(glGetUniformLocation(m_ShaderID, name), arraySize, glm::value_ptr(*value));
	}

	void Shader::SetUniformArray(const char *name, int arraySize, const glm::ivec3 *value) {
		glUniform3iv(glGetUniformLocation(m_ShaderID, name), arraySize, glm::value_ptr(*value));
	}

	void Shader::SetUniformArray(const char *name, int arraySize, const glm::vec4 *value) {
		glUniform4fv(glGetUniformLocation(m_ShaderID, name), arraySize, glm::value_ptr(*value));
	}

	void Shader::SetUniformArray(const char *name, int arraySize, const glm::ivec4 *value) {
		glUniform4iv(glGetUniformLocation(m_ShaderID, name), arraySize, glm::value_ptr(*value));
	}

	void Shader::SetUniformArray(const char *name, int arraySize, const glm::mat3 *value) {
		glUniformMatrix3fv(glGetUniformLocation(m_ShaderID, name), arraySize, GL_FALSE, glm::value_ptr(*value));
	}

	void Shader::SetUniformArray(const char *name, int arraySize, const glm::mat4 *value) {
		glUniformMatrix4fv(glGetUniformLocation(m_ShaderID, name), arraySize, GL_FALSE, glm::value_ptr(*value));
	}

	int Shader::GetUniformLocation(const char* name) {
		return glGetUniformLocation(m_ShaderID, name);
	}

	GLenum Shader::ShaderTypeFromString(const std::string &type) {
		if (type == "vertex") {
			return GL_VERTEX_SHADER;
		}
		else if (type == "fragment") {
			return GL_FRAGMENT_SHADER;
		}
		else if (type == "geometry") {
			return GL_GEOMETRY_SHADER;
		}
		else if (type == "hull") {
			return GL_TESS_CONTROL_SHADER;
		}
		else if (type == "domain") {
			return GL_TESS_EVALUATION_SHADER;
		}
		else if (type == "compute") {
			return GL_COMPUTE_SHADER;
		}

		// TODO: Should assert false here, unknown shader type: 'type'
		return 0;
	}

	std::unordered_map<GLenum, std::string> Shader::PreProcessShaderBinary(std::string &source) {
		std::unordered_map<GLenum, std::string> shaderSources;

		const char *shaderTypeToken = "#shader-type";
		size_t shaderTypeTokenLength = strlen(shaderTypeToken);
		size_t pos = source.find(shaderTypeToken);
		while (pos != std::string::npos) {
			size_t eol = source.find_first_of("\r\n", pos);
			// TODO: eol == std::string::npos, if so then we have a syntax error
			size_t begin = pos + shaderTypeTokenLength + 1;
			std::string shaderType = source.substr(begin, eol - begin);
			// TODO: type != "vertex" || fragment || hull || domain || compute, if so then we have an invalid shader type specified

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(shaderTypeToken, nextLinePos);
			shaderSources[ShaderTypeFromString(shaderType)] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}

		return shaderSources;
	}

	void Shader::Compile(const std::unordered_map<GLenum, std::string> &shaderSources) {
		m_ShaderID = glCreateProgram();

		// Attach different components of the shader (vertex, fragment, geometry, hull, domain, or compute)
		for (auto &item : shaderSources) {
			GLenum type = item.first;
			const std::string &source = item.second;

			GLuint shader = glCreateShader(type);
			const GLchar *shaderSource = source.c_str();
			glShaderSource(shader, 1, &shaderSource, NULL);
			glCompileShader(shader);

			// Check to see if compiling was successful
			GLint wasCompiled;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &wasCompiled);
			if (wasCompiled == GL_FALSE || source.empty()) {
				int length;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

				if (length > 0) {
					std::vector<char> error(length);
					glGetShaderInfoLog(shader, length, &length, &error[0]);
					std::string errorString(error.begin(), error.end());

					ARC_LOG_ERROR("Shader Compile Error: {0} - {1}", m_ShaderFilePath, errorString);
				}
				else {
					ARC_LOG_ERROR("Shader Compile Error: {0} - Unknown Error", m_ShaderFilePath);
				}
				glDeleteShader(shader);
				break;
			}

			glAttachShader(m_ShaderID, shader);
			glDeleteShader(shader);
		}

		// Validate shader
		glLinkProgram(m_ShaderID);
		glValidateProgram(m_ShaderID);
	}
}