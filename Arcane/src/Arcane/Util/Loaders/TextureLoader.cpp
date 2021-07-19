#include "arcpch.h"
#include "TextureLoader.h"

#include <Arcane/Graphics/Texture/Cubemap.h>
#include <Arcane/Graphics/Texture/Texture.h>

namespace Arcane
{
	// Static declarations
	std::unordered_map<std::string, Texture*> TextureLoader::m_TextureCache;
	Texture *TextureLoader::s_DefaultAlbedo;
	Texture *TextureLoader::s_DefaultNormal;
	Texture *TextureLoader::s_WhiteTexture; Texture *TextureLoader::s_BlackTexture;

	Texture* TextureLoader::Load2DTexture(std::string &path, TextureSettings *settings) {
		// Check the cache
		auto iter = m_TextureCache.find(path);
		if (iter != m_TextureCache.end()) {
			return iter->second;
		}

		// Load the texture
		int width, height, numComponents;
		unsigned char *data = stbi_load(path.c_str(), &width, &height, &numComponents, 0);
		if (!data) {
			ARC_LOG_ERROR("Failed to load texture path: {0}", path);
			stbi_image_free(data);
			return nullptr;
		}

		GLenum dataFormat;
		switch (numComponents) {
		case 1: dataFormat = GL_RED;  break;
		case 3: dataFormat = GL_RGB;  break;
		case 4: dataFormat = GL_RGBA; break;
		}

		Texture *texture = nullptr;
		if (settings != nullptr) {
			texture = new Texture(*settings);
		}
		else {
			texture = new Texture();
		}

		texture->Generate2DTexture(width, height, dataFormat, GL_UNSIGNED_BYTE, data);

		m_TextureCache.insert(std::pair<std::string, Texture*>(path, texture));
		stbi_image_free(data);

		return m_TextureCache[path];
	}

	Cubemap* TextureLoader::LoadCubemapTexture(const std::string &right, const std::string &left, const std::string &top, const std::string &bottom, const std::string &back, const std::string &front, CubemapSettings *settings) {
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

	void TextureLoader::InitializeDefaultTextures() {
		// Setup texture and minimal filtering because they are 1x1 textures so they require none
		TextureSettings srgbTextureSettings;
		srgbTextureSettings.IsSRGB = true;

		s_DefaultAlbedo = Load2DTexture(std::string("res/textures/default/defaultAlbedo.png"), &srgbTextureSettings);
		s_DefaultAlbedo->Bind();
		s_DefaultAlbedo->SetAnisotropicFilteringMode(1.0f);
		s_DefaultAlbedo->SetTextureMinFilter(GL_NEAREST);
		s_DefaultAlbedo->SetTextureMagFilter(GL_NEAREST);
		s_DefaultNormal = Load2DTexture(std::string("res/textures/default/defaultNormal.png"));
		s_DefaultNormal->Bind();
		s_DefaultNormal->SetAnisotropicFilteringMode(1.0f);
		s_DefaultNormal->SetTextureMinFilter(GL_NEAREST);
		s_DefaultNormal->SetTextureMagFilter(GL_NEAREST);
		s_WhiteTexture = Load2DTexture(std::string("res/textures/default/white.png"));
		s_WhiteTexture->Bind();
		s_WhiteTexture->SetAnisotropicFilteringMode(1.0f);
		s_WhiteTexture->SetTextureMinFilter(GL_NEAREST);
		s_WhiteTexture->SetTextureMagFilter(GL_NEAREST);
		s_BlackTexture = Load2DTexture(std::string("res/textures/default/black.png"));
		s_BlackTexture->Bind();
		s_BlackTexture->SetAnisotropicFilteringMode(1.0f);
		s_BlackTexture->SetTextureMinFilter(GL_NEAREST);
		s_BlackTexture->SetTextureMagFilter(GL_NEAREST);
	}
}
