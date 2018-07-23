#pragma once

#include <map>
#include <stb_image_aug.h>

#include "../../graphics/texture/Texture.h"

namespace arcane { namespace utils {

	class TextureLoader {
	public:
		static graphics::Texture* Load2DTexture(const char *path);
		// TOOO: Cubemap support, then our skybox class can use it
	private:
		static std::map<const char*, graphics::Texture> m_TextureCache;
	};

} }