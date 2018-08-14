#include "Shader.h"

#include "../utils/Logger.h"

namespace arcane { namespace graphics {

	Shader::Shader(const char *vertPath, const char *fragPath)
		: m_VertPath(vertPath), m_FragPath(fragPath), m_GeomPath("")
	{
		m_ShaderID = load();
	}

	Shader::Shader(const char *vertPath, const char *fragPath, const char *geoPath)
		: m_VertPath(vertPath), m_FragPath(fragPath), m_GeomPath(geoPath)
	{
		m_ShaderID = load();
	}

	Shader::~Shader() {
		glDeleteProgram(m_ShaderID);
	}

	unsigned int Shader::load() {
		// Create the program and shaders
		unsigned int program = glCreateProgram();
		unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
		unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);

		// Variables need to be declared or the character pointers will become dangling pointers
		std::string vertSourceString = FileUtils::readFile(m_VertPath);
		std::string fragSourceString = FileUtils::readFile(m_FragPath);
		const char *vertSource = vertSourceString.c_str();
		const char *fragSource = fragSourceString.c_str();

		// Vertex Shader
		glShaderSource(vertex, 1, &vertSource, NULL);
		glCompileShader(vertex);
		int result;

		// Check to see if it was successful
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE) {
			int length;
			glGetShaderiv(vertex, GL_INFO_LOG_LENGTH, &length);
			std::vector<char> error(length);
			glGetShaderInfoLog(vertex, length, &length, &error[0]);
			std::cout << "Failed to Compile Vertex Shader" << std::endl << &error[0] << std::endl;
			utils::Logger::getInstance().error("logged_files/shader_creation.txt", "shader initialization", "failed to compile vertex shader " + error[0]);
			glDeleteShader(vertex);
			return 0;
		}

		//Fragment Shader
		glShaderSource(fragment, 1, &fragSource, NULL);
		glCompileShader(fragment);

		// Check to see if it was successful
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE) {
			int length;
			glGetShaderiv(fragment, GL_INFO_LOG_LENGTH, &length);
			std::vector<char> error(length);
			glGetShaderInfoLog(fragment, length, &length, &error[0]);
			std::cout << "Failed to Compile Fragment Shader" << std::endl << &error[0] << std::endl;
			utils::Logger::getInstance().error("logged_files/shader_creation.txt", "shader initialization", "failed to compile fragment shader " + error[0]);
			glDeleteShader(fragment);
			return 0;
		}

		unsigned int geometry;
		// Check to see if a geometry shader was supplied
		if (m_GeomPath != "") {
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			std::string geomSourceString = FileUtils::readFile(m_GeomPath);
			const char *geomSource = geomSourceString.c_str();

			// Geometry Shader
			glShaderSource(geometry, 1, &geomSource, NULL);
			glCompileShader(geometry);
			int result;

			// Check to see if it was successful
			glGetShaderiv(geometry, GL_COMPILE_STATUS, &result);
			if (result == GL_FALSE) {
				int length;
				glGetShaderiv(geometry, GL_INFO_LOG_LENGTH, &length);
				std::vector<char> error(length);
				glGetShaderInfoLog(geometry, length, &length, &error[0]);
				std::cout << "Failed to Compile Geometry Shader" << std::endl << &error[0] << std::endl;
				utils::Logger::getInstance().error("logged_files/shader_creation.txt", "shader initialization", "failed to compile geometry shader " + error[0]);
				glDeleteShader(geometry);
				return 0;
			}
		}

		// Attach the shaders to the program and link them
		glAttachShader(program, vertex);
		glAttachShader(program, fragment);
		if (m_GeomPath != "")
			glAttachShader(program, geometry);
		glLinkProgram(program);
		glValidateProgram(program);

		// Delete the vertex and fragment shaders
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		if (m_GeomPath != "")
			glDeleteShader(geometry);

		// Return the program id
		return program;
	}

	int Shader::getUniformLocation(const char* name) {
		return glGetUniformLocation(m_ShaderID, name);
	}

	void Shader::setUniform1f(const char* name, float value) {
		glUniform1f(getUniformLocation(name), value);
	}

	void Shader::setUniform1i(const char* name, int value) {
		glUniform1i(getUniformLocation(name), value);
	}

	void Shader::setUniform2f(const char* name, const glm::vec2& vector) {
		glUniform2f(getUniformLocation(name), vector.x, vector.y);
	}

	void Shader::setUniform3f(const char* name, const glm::vec3& vector) {
		glUniform3f(getUniformLocation(name), vector.x, vector.y, vector.z);
	}

	void Shader::setUniform4f(const char* name, const glm::vec4& vector) {
		glUniform4f(getUniformLocation(name), vector.x, vector.y, vector.z, vector.w);
	}

	void Shader::setUniformMat4(const char* name, const glm::mat4& matrix) {
		glUniformMatrix4fv(glGetUniformLocation(m_ShaderID, name), 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void Shader::enable() const {
		glUseProgram(m_ShaderID);
	}

	void Shader::disable() const {
		glUseProgram(0);
	}

} }