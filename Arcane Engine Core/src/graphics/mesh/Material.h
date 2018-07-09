#pragma once

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include "../Shader.h"

namespace arcane { namespace graphics {

	// TODO: Move to a PBR material system
	class Material {
	public:
		Material(Shader *shader = nullptr, unsigned int diffuseMap = 0, unsigned int specularMap = 0, unsigned int normalMap = 0, unsigned int emissionMap = 0, float shininess = 32);

		// Assumes the shader is already bound
		void BindMaterialInformation();

		inline unsigned int getDiffuseMapId() { return m_DiffuseMap; }
		inline unsigned int getSpecularMapId() { return m_SpecularMap; }
		inline unsigned int getNormalMapId() { return m_NormalMap; }
		inline unsigned int getEmissionMapId() { return m_EmissionMap; }
		inline float getShininess() { return m_Shininess; }
		inline unsigned int getShaderId() { return m_Shader->getShaderID(); }

		inline void setDiffuseMapId(unsigned int id) { m_DiffuseMap = id; }
		inline void setSpecularMapId(unsigned int id) { m_SpecularMap = id; }
		inline void setNormalMapId(unsigned int id) { m_NormalMap = id; }
		inline void setEmissionMapId(unsigned int id) { m_EmissionMap = id; }
		inline void setShininess(float shininess) { m_Shininess = shininess; }
	private:
		unsigned int m_DiffuseMap, m_SpecularMap, m_NormalMap, m_EmissionMap;
		float m_Shininess;

		Shader *m_Shader;
	};

} }