#include "Utility.h"

namespace arcane { namespace opengl {

	GLuint Utility::loadTextureFromFile(const char *path) {
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
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			// Texture filtering
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// Free now that the memory is 
			stbi_image_free(data);
		}
		else {
			std::cout << "Texture failed to load at path: " << path << std::endl; // TODO log this
			stbi_image_free(data);
		}

		return textureID;
	}

} }