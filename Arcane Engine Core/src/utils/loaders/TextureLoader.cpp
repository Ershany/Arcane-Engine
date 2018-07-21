#include "TextureLoader.h"

namespace arcane { namespace utils {

	// Note: We could return by reference if we always returned the cached texture, but our current implementation allows the user to evade the cache if they choose
	graphics::Texture TextureLoader::Load2DTexture(const char *path, bool useCache) {
		// Check the cache
		std::map<const char*, graphics::Texture>::iterator iter = m_TextureCache.find(path);
		if (useCache && iter != m_TextureCache.end()) {
			return iter->second;
		}

		// Load the texture
		int width, height, numComponents;
		unsigned char *data = stbi_load(path, &width, &height, &numComponents, 0);
		if (!data) {
			utils::Logger::getInstance().error("logged_files/texture_loading.txt", "texture load fail - path:", path);
			stbi_image_free(data);
			return;
		}

		GLenum format;
		switch (numComponents) {
		case 1: format = GL_RED;  break;
		case 3: format = GL_RGB;  break;
		case 4: format = GL_RGBA; break;
		}

		graphics::Texture texture;
		texture.Generate2DTexture(width, height, format, format, data);

		// Add the texture to the cache (unless the user wants full control over the texture they loaded)
		if (useCache) {
			m_TextureCache.insert(std::pair<const char*, graphics::Texture>(path, texture));
		}

		return texture;
	}

} }