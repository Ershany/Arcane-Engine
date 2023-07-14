#pragma once
#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

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

	struct CubemapGenerationData
	{
		int width, height;
		GLenum dataFormat;
		unsigned char *data;
		Cubemap *cubemap;
		GLenum face;
	};

	class TextureLoader
	{
		friend class AssetManager;
		friend class Application;
	private:
		static void InitializeDefaultTextures();

		static void Load2DTextureData(const std::string &path, TextureGenerationData &inOutData);
		static void Generate2DTexture(const std::string &path, TextureGenerationData &inOutData);

		static void LoadCubemapTextureData(const std::string &path, CubemapGenerationData &inOutData);
		static void GenerateCubemapTexture(const std::string &path, CubemapGenerationData &inOutData);
	private:
		// Default Textures
		static Texture *s_DefaultNormal, *s_DefaultWaterDistortion;
		static Texture *s_WhiteTexture, *s_BlackTexture;
	};
}
#endif
