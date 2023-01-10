#include "arcpch.h"
#include "TextureLoader.h"

#include <Arcane/Graphics/Texture/Cubemap.h>
#include <Arcane/Util/Loaders/AssetManager.h>

namespace Arcane
{
	// Static declarations
	Texture *TextureLoader::s_DefaultNormal;
	Texture *TextureLoader::s_WhiteTexture; Texture *TextureLoader::s_BlackTexture;

	void TextureLoader::Load2DTextureData(std::string &path, TextureGenerationData &inOutData)
	{
		// Load the texture data
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

	void TextureLoader::Generate2DTexture(std::string &path, TextureGenerationData &inOutData)
	{
		inOutData.texture->Generate2DTexture(inOutData.width, inOutData.height, inOutData.dataFormat, GL_UNSIGNED_BYTE, inOutData.data);
		stbi_image_free(inOutData.data);
	}

	Cubemap* TextureLoader::LoadCubemapTexture(const std::string &right, const std::string &left, const std::string &top, const std::string &bottom, const std::string &back, const std::string &front, CubemapSettings *settings)
	{
		Cubemap *cubemap = new Cubemap();
		if (settings != nullptr)
			cubemap->SetCubemapSettings(*settings);

		std::vector<std::string> faces = { right, left, top, bottom, back, front };

		// Load the textures for the cubemap
		int width, height, numComponents;
		for (unsigned int i = 0; i < 6; ++i) {
			unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &numComponents, 0);

			if (data) {
				GLenum dataFormat;
				switch (numComponents) {
				case 1: dataFormat = GL_RED;  break;
				case 3: dataFormat = GL_RGB;  break;
				case 4: dataFormat = GL_RGBA; break;
				}

				cubemap->GenerateCubemapFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, width, height, dataFormat, data);
				stbi_image_free(data);
			}
			else {
				ARC_LOG_ERROR("Couldn't load cubemap using 6 filepaths. Filepath error: {0}", faces[i]);
				stbi_image_free(data);
				return cubemap;
			}
		}
		
		return cubemap;
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
		s_WhiteTexture = assetManager.Load2DTexture(std::string("res/textures/default/white.png"), &settings);
		s_BlackTexture = assetManager.Load2DTexture(std::string("res/textures/default/black.png"), &settings);
	}
}
