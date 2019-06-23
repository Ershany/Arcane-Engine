#pragma once

#include <graphics/texture/Cubemap.h>
#include <graphics/texture/Texture.h>

namespace arcane {

	class TextureLoader {
	public:
		static void initializeDefaultTextures();

		// TODO: HDR loading
		static Texture* load2DTexture(std::string &path, TextureSettings *settings = nullptr);
		static Cubemap* loadCubemapTexture(const std::string &right, const std::string &left, const std::string &top, const std::string &bottom, const std::string &back, const std::string &front, bool isSRGB, CubemapSettings *settings = nullptr);

		inline static Texture* getDefaultAlbedo() { return s_DefaultAlbedo; }
		inline static Texture* getDefaultNormal() { return s_DefaultNormal; }
		inline static Texture* getDefaultMetallic() { return s_NoMetallic; }
		inline static Texture* getDefaultRoughness() { return s_NoRoughness; }
		inline static Texture* getDefaultAO() { return s_DefaultAO; }
		inline static Texture* getDefaultEmission() { return s_DefaultEmission; }
		inline static Texture* getFullMetallic() { return s_FullMetallic; }
		inline static Texture* getNoMetallic() { return s_NoMetallic; }
		inline static Texture* getFullRoughness() { return s_FullRoughness; }
		inline static Texture* getNoRoughness() { return s_NoRoughness; }
	private:
		static std::unordered_map<std::string, Texture*> m_TextureCache;
		
		// Default Textures
		static Texture *s_DefaultAlbedo;
		static Texture *s_DefaultNormal;
		static Texture *s_FullMetallic, *s_NoMetallic;
		static Texture *s_FullRoughness, *s_NoRoughness;
		static Texture *s_DefaultAO;
		static Texture *s_DefaultEmission;
	};

}
