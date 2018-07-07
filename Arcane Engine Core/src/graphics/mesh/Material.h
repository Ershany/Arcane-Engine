#pragma once

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include "../Shader.h"

namespace arcane { namespace graphics {

	// TODO: Move to a PBR system
	class Material {
	public:
		Material(Shader *shader = nullptr, GLuint diffuseMap = 0, GLuint specularMap = 0, GLuint emissionMap = 0, GLuint normalMap = 0, float shininess = 32) 
			: m_Shader(shader), m_DiffuseMap(diffuseMap), m_SpecularMap(specularMap), m_EmissionMap(emissionMap), m_NormalMap(normalMap), m_Shininess(shininess)
		{
		}
	private:
		GLuint m_DiffuseMap, m_SpecularMap, m_NormalMap, m_EmissionMap;
		float m_Shininess;

		Shader *m_Shader;
	};

} }