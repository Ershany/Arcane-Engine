#include "arcpch.h"
#include "TextureLoader.h"

#include <Arcane/Graphics/Texture/Texture.h>
#include <Arcane/Graphics/Texture/Cubemap.h>
#include <Arcane/Util/Loaders/AssetManager.h>

namespace Arcane
{
	// Static declarations
	Texture *TextureLoader::s_DefaultNormal; Texture *TextureLoader::s_DefaultWaterDistortion;
	Texture *TextureLoader::s_WhiteTexture; Texture *TextureLoader::s_BlackTexture;
	Texture *TextureLoader::s_WhiteTextureSRGB; Texture *TextureLoader::s_BlackTextureSRGB;

	void TextureLoader::Load2DTextureData(const std::string &path, TextureGenerationData &inOutData)
	{
		// Load the texture data from file
		int numComponents;
		inOutData.data = stbi_load(path.c_str(), &inOutData.width, &inOutData.height, &numComponents, 0);
		if (!inOutData.data)
		{
			ARC_LOG_ERROR("Failed to load texture path: {0}", path);
			stbi_image_free(inOutData.data);
			return;
		}

		switch (numComponents)
		{
		case 1: inOutData.dataFormat = GL_RED;  break;
		case 3: inOutData.dataFormat = GL_RGB;  break;
		case 4: inOutData.dataFormat = GL_RGBA; break;
		}
	}

	void TextureLoader::Generate2DTexture(const std::string &path, TextureGenerationData &inOutData)
	{
		inOutData.texture->Generate2DTexture(inOutData.width, inOutData.height, inOutData.dataFormat, GL_UNSIGNED_BYTE, inOutData.data);
		stbi_image_free(inOutData.data);
	}
	void TextureLoader::LoadCubemapTextureData(const std::string &path, CubemapGenerationData &inOutData)
	{
		// Load the cubemap data from file
		int numComponents;
		inOutData.data = stbi_load(path.c_str(), &inOutData.width, &inOutData.height, &numComponents, 0);
		if (!inOutData.data)
		{
			ARC_LOG_ERROR("Failed to load cubemap face: {0}, at path: {1} - Reason: {2}", inOutData.face, path, stbi_failure_reason());
			stbi_image_free(inOutData.data);
			return;
		}

		switch (numComponents)
		{
		case 1: inOutData.dataFormat = GL_RED;  break;
		case 3: inOutData.dataFormat = GL_RGB;  break;
		case 4: inOutData.dataFormat = GL_RGBA; break;
		}
	}

	void TextureLoader::GenerateCubemapTexture(const std::string &path, CubemapGenerationData &inOutData)
	{
		inOutData.cubemap->GenerateCubemapFace(inOutData.face, inOutData.width, inOutData.height, inOutData.dataFormat, inOutData.data);
		stbi_image_free(inOutData.data);
	}

	void TextureLoader::InitializeDefaultTextures()
	{
		// Setup texture and minimal filtering because they are 1x1 textures so they require none
		TextureSettings settings;
		settings.TextureAnisotropyLevel = 1.0f;
		settings.TextureMinificationFilterMode = GL_NEAREST;
		settings.TextureMagnificationFilterMode = GL_NEAREST;

		AssetManager &assetManager = AssetManager::GetInstance();

		s_DefaultNormal = assetManager.Load2DTexture(std::string("res/textures/default/defaultNormal.png"), &settings);
		s_DefaultWaterDistortion = assetManager.Load2DTexture(std::string("res/textures/default/defaultWaterDistortion.png"), &settings);
		s_WhiteTexture = assetManager.Load2DTexture(std::string("res/textures/default/white.png"), &settings);
		s_BlackTexture = assetManager.Load2DTexture(std::string("res/textures/default/black.png"), &settings);

		TextureSettings srgbSettings;
		srgbSettings.TextureAnisotropyLevel = 1.0f;
		srgbSettings.TextureMinificationFilterMode = GL_NEAREST;
		srgbSettings.TextureMagnificationFilterMode = GL_NEAREST;
		srgbSettings.IsSRGB = true;
		s_WhiteTextureSRGB = assetManager.Load2DTexture(std::string("res/textures/default/white-srgb.png"), &srgbSettings);
		s_BlackTextureSRGB = assetManager.Load2DTexture(std::string("res/textures/default/black-srgb.png"), &srgbSettings);
	}
}
