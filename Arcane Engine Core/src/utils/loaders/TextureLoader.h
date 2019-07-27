#pragma once

#include <graphics/texture/Cubemap.h>
#include <graphics/texture/Texture.h>

namespace arcane {

	class TextureLoader {
	public:
		static void initializeDefaultTextures();

		// TODO: HDR loading
		static Texture* load2DTexture(std::string &path, TextureSettings *settings = nullptr);
		static Cubemap* loadCubemapTexture(const std::string &right, const std::string &left, const std::string &top, const std::string &bottom, const std::string &back, const std::string &front, CubemapSettings *settings = nullptr);

		inline static Texture* getWhiteTexture() { return s_WhiteTexture; }
		inline static Texture* getBlackTexture() { return s_BlackTexture; }
		inline static Texture* getDefaultAlbedo() { return s_DefaultAlbedo; }
		inline static Texture* getDefaultNormal() { return s_DefaultNormal; }
		inline static Texture* getDefaultMetallic() { return s_BlackTexture; }
		inline static Texture* getDefaultRoughness() { return s_BlackTexture; }
		inline static Texture* getDefaultAO() { return s_WhiteTexture; }
		inline static Texture* getFullMetallic() { return s_WhiteTexture; }
		inline static Texture* getNoMetallic() { return s_BlackTexture; }
		inline static Texture* getFullRoughness() { return s_WhiteTexture; }
		inline static Texture* getNoRoughness() { return s_BlackTexture; }
	private:
		static std::unordered_map<std::string, Texture*> m_TextureCache;
		
		// Default Textures
		static Texture *s_DefaultAlbedo;
		static Texture *s_DefaultNormal;
		static Texture *s_WhiteTexture, *s_BlackTexture;
	};

}
