#pragma once

#include <GL\glew.h>
#include <GLFW\glfw3.h>

namespace arcane { namespace graphics {

	class Material {
	public:
		// TODO: Move to a PBR system
		GLuint diffuseMap, specularMap, emissionMap;
		float shininess;
		GLuint normalMap;



		// A shader
	};

} }