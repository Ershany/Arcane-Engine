#pragma once

#include "../utils/FileUtils.h"
#include <GL\glew.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace arcane { namespace graphics {

	class Shader {
	private:
		GLuint m_ShaderID;
		const char *m_VertPath, *m_FragPath;
	public:
		Shader(const char *vertPath, const char *fragPath);
		~Shader();

		void setUniform1f(const GLchar* name, float value);
		void setUniform1i(const GLchar* name, int value);
		void setUniform2f(const GLchar* name, const glm::vec2& vector);
		void setUniform3f(const GLchar* name, const glm::vec3& vector);
		void setUniform4f(const GLchar* name, const glm::vec4& vector);
		void setUniformMat4(const GLchar* name, const glm::mat4& matrix);

		void enable() const;
		void disable() const;

		inline GLuint getShaderID() { return m_ShaderID; }
	private:
		GLint getUniformLocation(const GLchar* name);
		GLuint load();
	};

} }