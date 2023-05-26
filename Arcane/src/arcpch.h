#pragma once

// C/C++
#define NOMINMAX
#include <filesystem>
#include <cmath>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <array>
#include <iterator>
#include <random>
#include <filesystem>

// Dependencies
#include <gl/glew.h>

#include <GLFW/glfw3.h>

#include <Arcane/Vendor/stb/stb_image.h>

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Arcane/Vendor/renderdoc-1.x/renderdoc/api/app/renderdoc_app.h>

// Arcane
#include <Arcane/Defs.h>
#include <Arcane/Util/Logger.h>
#include <Arcane/Core/Base.h>
#include <Arcane/Core/Events/Event.h>
