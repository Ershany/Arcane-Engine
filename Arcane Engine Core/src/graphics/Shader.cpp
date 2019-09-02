#include "pch.h"
#include "Shader.h"

namespace arcane {

	Shader::Shader(const char *computePath) 
		: m_VertPath(""), m_FragPath(""), m_GeomPath(""), m_HullPath(""), m_DomainPath(""), m_ComputePath(computePath)
	{
		m_ShaderID = load();
	}

	Shader::Shader(const char *vertPath, const char *fragPath)
		: m_VertPath(vertPath), m_FragPath(fragPath), m_GeomPath(""), m_HullPath(""), m_DomainPath(""), m_ComputePath("")
	{
		m_ShaderID = load();
	}

	Shader::Shader(const char *vertPath, const char *fragPath, const char *geoPath)
		: m_VertPath(vertPath), m_FragPath(fragPath), m_GeomPath(geoPath), m_HullPath(""), m_DomainPath(""), m_ComputePath("")
	{
		m_ShaderID = load();
	}

	Shader::Shader(const char *vertPath, const char *fragPath, const char *hullPath, const char *domainPath)
		: m_VertPath(vertPath), m_FragPath(fragPath), m_GeomPath(""), m_HullPath(hullPath), m_DomainPath(domainPath), m_ComputePath("")
	{
		m_ShaderID = load();
	}

	Shader::Shader(const char *vertPath, const char *fragPath, const char *geoPath, const char *hullPath, const char *domainPath)
		: m_VertPath(vertPath), m_FragPath(fragPath), m_GeomPath(geoPath), m_HullPath(hullPath), m_DomainPath(domainPath), m_ComputePath("")
	{
		m_ShaderID = load();
	}

	Shader::~Shader() {
		glDeleteProgram(m_ShaderID);
	}

	unsigned int Shader::load() {
		// Create the program
		unsigned int program = glCreateProgram();
		int result;

		// Vertex Shader (optional)
		unsigned int vertex;
		if (m_VertPath != "") {
			vertex = glCreateShader(GL_VERTEX_SHADER);
			std::string vertSourceString = FileUtils::readFile(m_VertPath);
			const char *vertSource = vertSourceString.c_str();

			glShaderSource(vertex, 1, &vertSource, NULL);
			glCompileShader(vertex);

			// Check to see if it was successful
			glGetShaderiv(vertex, GL_COMPILE_STATUS, &result);
			if (result == GL_FALSE || vertSourceString.empty()) {
				int length;
				glGetShaderiv(vertex, GL_INFO_LOG_LENGTH, &length);
				if (length > 0) {
					std::vector<char> error(length);
					glGetShaderInfoLog(vertex, length, &length, &error[0]);
					std::string errorString(error.begin(), error.end());

					Logger::getInstance().error("logged_files/shader_compile_error.txt", m_VertPath, errorString);
				}
				else {
					Logger::getInstance().error("logged_files/shader_compile_error.txt", m_VertPath, "unknown error");
				}
				glDeleteShader(vertex);
				return 0;
			}
		}

		// Fragment Shader (optional)
		unsigned int fragment;
		if (m_FragPath != "") {
			fragment = glCreateShader(GL_FRAGMENT_SHADER);
			std::string fragSourceString = FileUtils::readFile(m_FragPath);
			const char *fragSource = fragSourceString.c_str();

			glShaderSource(fragment, 1, &fragSource, NULL);
			glCompileShader(fragment);

			// Check to see if it was successful
			glGetShaderiv(fragment, GL_COMPILE_STATUS, &result);
			if (result == GL_FALSE || fragSourceString.empty()) {
				int length;
				glGetShaderiv(fragment, GL_INFO_LOG_LENGTH, &length);
				if (length > 0) {
					std::vector<char> error(length);
					glGetShaderInfoLog(fragment, length, &length, &error[0]);
					std::string errorString(error.begin(), error.end());

					Logger::getInstance().error("logged_files/shader_compile_error.txt", m_FragPath, errorString);
				}
				else {
					Logger::getInstance().error("logged_files/shader_compile_error.txt", m_FragPath, "error unknown");
				}
				glDeleteShader(fragment);
				return 0;
			}
		}

		// Geometry shader (optional)
		unsigned int geometry;
		if (m_GeomPath != "") {
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			std::string geomSourceString = FileUtils::readFile(m_GeomPath);
			const char *geomSource = geomSourceString.c_str();

			glShaderSource(geometry, 1, &geomSource, NULL);
			glCompileShader(geometry);

			// Check to see if it was successful
			glGetShaderiv(geometry, GL_COMPILE_STATUS, &result);
			if (result == GL_FALSE || geomSourceString.empty()) {
				int length;
				glGetShaderiv(geometry, GL_INFO_LOG_LENGTH, &length);
				if (length > 0) {
					std::vector<char> error(length);
					glGetShaderInfoLog(geometry, length, &length, &error[0]);
					std::string errorString(error.begin(), error.end());

					Logger::getInstance().error("logged_files/shader_compile_error.txt", m_GeomPath, errorString);
				}
				else {
					Logger::getInstance().error("logged_files/shader_compile_error.txt", m_GeomPath, "error unknown");
				}
				glDeleteShader(geometry);
				return 0;
			}
		}

		// Hull Shader (optional)
		unsigned int hull;
		if (m_HullPath != "") {
			hull = glCreateShader(GL_TESS_CONTROL_SHADER);
			std::string hullSourceString = FileUtils::readFile(m_HullPath);
			const char *hullSource = hullSourceString.c_str();

			glShaderSource(hull, 1, &hullSource, NULL);
			glCompileShader(hull);

			glGetShaderiv(hull, GL_COMPILE_STATUS, &result);
			if (result == GL_FALSE || hullSourceString.empty()) {
				int length;
				glGetShaderiv(hull, GL_INFO_LOG_LENGTH, &length);
				if (length > 0) {
					std::vector<char> error(length);
					glGetShaderInfoLog(hull, length, &length, &error[0]);
					std::string errorString(error.begin(), error.end());

					Logger::getInstance().error("logged_files/shader_compile_error.txt", m_HullPath, errorString);
				}
				else {
					Logger::getInstance().error("logged_files/shader_compile_error.txt", m_HullPath, "error unknown");
				}
				glDeleteShader(hull);
				return 0;
			}
		}

		// Domain Shader (optional)
		unsigned int domain;
		if (m_DomainPath != "") {
			domain = glCreateShader(GL_TESS_EVALUATION_SHADER);
			std::string domainSourceString = FileUtils::readFile(m_DomainPath);
			const char *domainSource = domainSourceString.c_str();

			glShaderSource(domain, 1, &domainSource, NULL);
			glCompileShader(domain);

			glGetShaderiv(domain, GL_COMPILE_STATUS, &result);
			if (result == GL_FALSE || domainSourceString.empty()) {
				int length;
				glGetShaderiv(domain, GL_INFO_LOG_LENGTH, &length);
				if (length > 0) {
					std::vector<char> error(length);
					glGetShaderInfoLog(domain, length, &length, &error[0]);
					std::string errorString(error.begin(), error.end());

					Logger::getInstance().error("logged_files/shader_compile_error.txt", m_DomainPath, errorString);
				}
				else {
					Logger::getInstance().error("logged_files/shader_compile_error.txt", m_DomainPath, "error unknown");
				}
				glDeleteShader(domain);
				return 0;
			}
		}

		unsigned int compute;
		if (m_ComputePath != "") {
			compute = glCreateShader(GL_COMPUTE_SHADER);
			std::string computeSourceString = FileUtils::readFile(m_ComputePath);
			const char *computeSource = computeSourceString.c_str();

			glShaderSource(compute, 1, &computeSource, NULL);
			glCompileShader(compute);

			glGetShaderiv(compute, GL_COMPILE_STATUS, &result);
			if (result == GL_FALSE || computeSourceString.empty()) {
				int length;
				glGetShaderiv(compute, GL_INFO_LOG_LENGTH, &length);
				if (length > 0) {
					std::vector<char> error(length);
					glGetShaderInfoLog(compute, length, &length, &error[0]);
					std::string errorString(error.begin(), error.end());

					Logger::getInstance().error("logged_files/shader_compile_error.txt", m_ComputePath, errorString);
				}
				else {
					Logger::getInstance().error("logged_files/shader_compile_error.txt", m_ComputePath, "error unknown");
				}
				glDeleteShader(compute);
				return 0;
			}
		}

		// Attach the shaders to the program and link them
		if (m_VertPath != "")
			glAttachShader(program, vertex);
		if (m_FragPath != "")
			glAttachShader(program, fragment);
		if (m_GeomPath != "")
			glAttachShader(program, geometry);
		if (m_HullPath != "")
			glAttachShader(program, hull);
		if (m_DomainPath != "")
			glAttachShader(program, domain);
		if (m_ComputePath != "")
			glAttachShader(program, compute);

		glLinkProgram(program);
		glValidateProgram(program);

		// Delete the vertex and fragment shaders
		if (m_VertPath != "")
			glDeleteShader(vertex);
		if (m_FragPath != "")
			glDeleteShader(fragment);
		if (m_GeomPath != "")
			glDeleteShader(geometry);
		if (m_HullPath != "")
			glDeleteShader(hull);
		if (m_DomainPath != "")
			glDeleteShader(domain);
		if (m_ComputePath != "")
			glDeleteShader(compute);

		// Return the program id
		return program;
	}

	int Shader::getUniformLocation(const char* name) {
		return glGetUniformLocation(m_ShaderID, name);
	}

	void Shader::setUniform(const char* name, float value) {
		glUniform1f(getUniformLocation(name), value);
	}

	void Shader::setUniform(const char* name, int value) {
		glUniform1i(getUniformLocation(name), value);
	}

	void Shader::setUniform(const char* name, const glm::vec2& vector) {
		glUniform2f(getUniformLocation(name), vector.x, vector.y);
	}

	void Shader::setUniform(const char *name, const glm::ivec2& vector) {
		glUniform2i(getUniformLocation(name), vector.x, vector.y);
	}

	void Shader::setUniform(const char* name, const glm::vec3& vector) {
		glUniform3f(getUniformLocation(name), vector.x, vector.y, vector.z);
	}

	void Shader::setUniform(const char *name, const glm::ivec3& vector) {
		glUniform3i(getUniformLocation(name), vector.x, vector.y, vector.z);
	}

	void Shader::setUniform(const char* name, const glm::vec4& vector) {
		glUniform4f(getUniformLocation(name), vector.x, vector.y, vector.z, vector.w);
	}

	void Shader::setUniform(const char* name, const glm::ivec4& vector) {
		glUniform4i(getUniformLocation(name), vector.x, vector.y, vector.z, vector.w);
	}

	void Shader::setUniform(const char* name, const glm::mat3& matrix) {
		glUniformMatrix3fv(glGetUniformLocation(m_ShaderID, name), 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void Shader::setUniform(const char* name, const glm::mat4& matrix) {
		glUniformMatrix4fv(glGetUniformLocation(m_ShaderID, name), 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void Shader::setUniformArray(const char *name, int arraySize, float *value) {
		glUniform1fv(glGetUniformLocation(m_ShaderID, name), arraySize, value);
	}

	void Shader::setUniformArray(const char *name, int arraySize, int *value) {
		glUniform1iv(glGetUniformLocation(m_ShaderID, name), arraySize, value);
	}

	void Shader::setUniformArray(const char *name, int arraySize, glm::vec2 *value) {
		glUniform2fv(glGetUniformLocation(m_ShaderID, name), arraySize, glm::value_ptr(*value));
	}

	void Shader::setUniformArray(const char *name, int arraySize, glm::ivec2 *value) {
		glUniform2iv(glGetUniformLocation(m_ShaderID, name), arraySize, glm::value_ptr(*value));
	}

	void Shader::setUniformArray(const char *name, int arraySize, glm::vec3 *value) {
		glUniform3fv(glGetUniformLocation(m_ShaderID, name), arraySize, glm::value_ptr(*value));
	}

	void Shader::setUniformArray(const char *name, int arraySize, glm::ivec3 *value) {
		glUniform3iv(glGetUniformLocation(m_ShaderID, name), arraySize, glm::value_ptr(*value));
	}

	void Shader::setUniformArray(const char *name, int arraySize, glm::vec4 *value) {
		glUniform4fv(glGetUniformLocation(m_ShaderID, name), arraySize, glm::value_ptr(*value));
	}

	void Shader::setUniformArray(const char *name, int arraySize, glm::ivec4 *value) {
		glUniform4iv(glGetUniformLocation(m_ShaderID, name), arraySize, glm::value_ptr(*value));
	}

	void Shader::enable() const {
		glUseProgram(m_ShaderID);
	}

	void Shader::disable() const {
		glUseProgram(0);
	}

}