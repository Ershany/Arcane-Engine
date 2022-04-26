#pragma once

namespace Arcane
{
	class Texture;
	struct TextureSettings;
	class Cubemap;
	struct CubemapSettings;

	class TextureLoader {
	public:
		static void InitializeDefaultTextures();

		// TODO: HDR loading
		static Texture* Load2DTexture(std::string &path, TextureSettings *settings = nullptr);
		static Cubemap* LoadCubemapTexture(const std::string &right, const std::string &left, const std::string &top, const std::string &bottom, const std::string &back, const std::string &front, CubemapSettings *settings = nullptr);

		inline static Texture* GetWhiteTexture() { return s_WhiteTexture; }
		inline static Texture* GetBlackTexture() { return s_BlackTexture; }
		inline static Texture* GetDefaultNormal() { return s_DefaultNormal; }
		inline static Texture* GetDefaultAO() { return s_WhiteTexture; }
		inline static Texture* GetFullMetallic() { return s_WhiteTexture; }
		inline static Texture* GetNoMetallic() { return s_BlackTexture; }
		inline static Texture* GetFullRoughness() { return s_WhiteTexture; }
		inline static Texture* GetNoRoughness() { return s_BlackTexture; }
	private:
		static std::unordered_map<std::string, Texture*> m_TextureCache;
		
		// Default Textures
		static Texture *s_DefaultNormal;
		static Texture *s_WhiteTexture, *s_BlackTexture;
	};
}
