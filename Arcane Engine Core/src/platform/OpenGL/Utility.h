#pragma once

#include <string>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <SOIL.h>
#include <iostream>
#include <stb_image_aug.h>

namespace arcane { namespace opengl {

	class Utility {
	public:
		static GLuint loadTextureFromFile(const char *path, bool containsTransparencyOnSides = false);
	};

} }