#include "TextureLoader.h"

namespace arcane { namespace utils {

	std::map<const char*, graphics::Texture> TextureLoader::m_TextureCache;

	graphics::Texture* TextureLoader::Load2DTexture(const char *path) {
		// Check the cache
		std::map<const char*, graphics::Texture>::iterator iter = m_TextureCache.find(path);
		if (iter != m_TextureCache.end()) {
			return &iter->second;
		}

		// Load the texture
		int width, height, numComponents;
		unsigned char *data = stbi_load(path, &width, &height, &numComponents, 0);
		if (!data) {
			utils::Logger::getInstance().error("logged_files/texture_loading.txt", "texture load fail - path:", path);
			stbi_image_free(data);
			return nullptr;
		}

		GLenum format;
		switch (numComponents) {
		case 1: format = GL_RED;  break;
		case 3: format = GL_RGB;  break;
		case 4: format = GL_RGBA; break;
		}

		graphics::Texture texture;
		texture.Generate2DTexture(width, height, format, format, data);

		m_TextureCache.insert(std::pair<const char*, graphics::Texture>(path, texture));
		return &m_TextureCache[path];
	}

} }