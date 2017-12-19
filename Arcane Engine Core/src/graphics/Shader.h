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
		const char *m_VertPath, *m_FragPath, *m_GeomPath;
	public:
		/**
		* Loads the shader by calling the load function and stores its shader ID
		*
		* @param vertPath The path of the vertex shader. ie src/shaders/someShader.vert
		* @param fragPath The path of the fragment/pixel shader. ie src/shaders/someShader.frag
		*/
		Shader(const char *vertPath, const char *fragPath);
		Shader(const char *vertPath, const char *fragPath, const char *geoPath);
		~Shader();


		/**
		* Assigns a new value to a uniform float in the shader
		*
		* @param name Name of the uniform float that is being modified
		* @param value The new value that gets assigned to the uniform float
		*/
		void setUniform1f(const GLchar* name, float value);

		/**
		* Assigns a new value to a uniform integer(or a texture's texture unit) in the shader
		*
		* @param name Name of the uniform integer that is being modified
		* @param value The new value that gets assigned to the uniform integer
		*/
		void setUniform1i(const GLchar* name, int value);

		/**
		* Assigns a new vector to a uniform vec2 in the shader
		*
		* @param name Name of the uniform vec2 that is being modified
		* @param vector The new vector that gets assigned to the uniform vec2
		*/
		void setUniform2f(const GLchar* name, const glm::vec2& vector);

		/**
		* Assigns a new vector to a uniform vec3 in the shader
		*
		* @param name Name of the uniform vec3 that is being modified
		* @param vector The new vector that gets assigned to the uniform vec3
		*/
		void setUniform3f(const GLchar* name, const glm::vec3& vector);

		/**
		* Assigns a new vector to a uniform vec4 in the shader
		*
		* @param name Name of the uniform vec4 that is being modified
		* @param vector The new vector that gets assigned to the uniform vec4
		*/
		void setUniform4f(const GLchar* name, const glm::vec4& vector);

		/**
		* Assigns a new matrix to a uniform mat4 in the shader
		*
		* @param name Name of the uniform mat4 that is being modified
		* @param matrix The new matrix that gets assigned to the uniform mat4
		*/
		void setUniformMat4(const GLchar* name, const glm::mat4& matrix);


		/**
		* Binds the shader
		*/
		void enable() const;

		/**
		* Unbinds the shader
		*/
		void disable() const;

		inline GLuint getShaderID() { return m_ShaderID; }
	private:
		/**
		* Helper function to get the location of a uniform variable in the shader
		* Used by all of the uniform setters, to get the location of the uniform variable
		*
		* @param name Name of the uniform variable that is being modified
		*/
		GLint getUniformLocation(const GLchar* name);

		/**
		* Creates the shader program and will display any errors to the console
		*/
		GLuint load();
	};

} }