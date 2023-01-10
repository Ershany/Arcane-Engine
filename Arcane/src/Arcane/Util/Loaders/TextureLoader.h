#pragma once

#include <Arcane/Graphics/Texture/Texture.h>

namespace Arcane
{
	class Texture;
	struct TextureSettings;
	class Cubemap;
	struct CubemapSettings;

	struct TextureGenerationData
	{
		int width, height;
		GLenum dataFormat;
		unsigned char *data;
		Texture *texture;
	};

	class TextureLoader
	{
		friend class AssetManager;
		friend class Application;
	public:
		// TODO: HDR loading
		static Cubemap* LoadCubemapTexture(const std::string &right, const std::string &left, const std::string &top, const std::string &bottom, const std::string &back, const std::string &front, CubemapSettings *settings = nullptr);
	private:
		static void InitializeDefaultTextures();

		static void Load2DTextureData(std::string &path, TextureGenerationData &inOutData);
		static void Generate2DTexture(std::string &path, TextureGenerationData &inOutData);
	private:
		// Default Textures
		static Texture *s_DefaultNormal;
		static Texture *s_WhiteTexture, *s_BlackTexture;
	};
}
