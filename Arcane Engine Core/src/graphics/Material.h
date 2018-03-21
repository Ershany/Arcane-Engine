#pragma once

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include "Shader.h"

namespace arcane { namespace graphics {

	// TODO: Move to a PBR system
	class Material {
	public:
		Material(GLuint diffuseMap, GLuint specularMap, GLuint emissionMap, GLuint normalMap, float shininess, Shader *shader);
	private:
		GLuint m_DiffuseMap, m_SpecularMap, m_NormalMap, m_EmissionMap;
		float m_Shininess;

		Shader *m_Shader;
	};

} }