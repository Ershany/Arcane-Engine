#include "Utility.h"

#include "../../utils/Logger.h"

namespace arcane { namespace opengl {

	GLuint Utility::loadTextureFromFile(const char *path, bool containsTransparencyOnSides) {
		GLuint textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
		if (data) {
			GLenum format;
			switch (nrComponents) {
			case 1: format = GL_RED;  break;
			case 3: format = GL_RGB;  break;
			case 4: format = GL_RGBA; break;
			}

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			// Texture wrapping
			if (containsTransparencyOnSides) {
				// Can't use GL_REPEAT or interpolation will mess with the transparency
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}
			else {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			}

			// Texture filtering
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.4f); //If you want to sample the mipmap 0.4 levels larger then you openGL does by default

			// Free now that the memory is 
			stbi_image_free(data);
		}
		else {
			std::cout << "Texture failed to load at path: " << path << std::endl;
			utils::Logger::getInstance().error("logged_files/texture_loading.txt", "texture load (OpenGL) fail path:", path);
			stbi_image_free(data);
		}
		return textureID;
	}

	GLuint Utility::loadCubemapFromFiles(const std::vector<const char*> &filePaths) {
		// Size check
		if (filePaths.size() != 6) {
			utils::Logger::getInstance().error("logged_files / error.txt", "Cubemap initialization", "Could not initialize the cubemap since 6 faces were not provided");
			return -1;
		}

		GLuint cubemapID;
		glGenTextures(1, &cubemapID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);

		int width, height, nrComponents;
		for (unsigned int i = 0; i < 6; ++i) {
			unsigned char *data = stbi_load(filePaths[i], &width, &height, &nrComponents, 0);
			if (data) {
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);
			}
			else {
				utils::Logger::getInstance().error("logged_files / error.txt", "Cubemap initialization", "Couldn't load cubemap filepath");
				stbi_image_free(data);
				return -1;
			}
		}
		
		// Texture filtering
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		// Texture wrapping
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return cubemapID;
	}

} }