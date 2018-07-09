#pragma once

#include <string>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <SOIL.h>
#include <iostream>
#include <stb_image_aug.h>
#include <vector>
#include <glm\common.hpp>
#include "../../utils/Logger.h"
#include "../../Defs.h"

namespace arcane { namespace opengl {

	class Utility {
	public:
		static unsigned int loadTextureFromFile(const char *path, bool containsTransparencyOnSides = false);
		static unsigned int loadCubemapFromFiles(const std::vector<const char*> &vec);
	};

} }