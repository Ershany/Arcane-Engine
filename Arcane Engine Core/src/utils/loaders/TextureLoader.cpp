#include "pch.h"
#include "TextureLoader.h"

namespace arcane {

	// Static declarations
	std::unordered_map<std::string, Texture*> TextureLoader::m_TextureCache;
	Texture *TextureLoader::s_DefaultAlbedo;
	Texture *TextureLoader::s_DefaultNormal;
	Texture *TextureLoader::s_FullMetallic, *TextureLoader::s_NoMetallic;
	Texture *TextureLoader::s_FullRoughness, *TextureLoader::s_NoRoughness;
	Texture *TextureLoader::s_DefaultAO;
	Texture *TextureLoader::s_DefaultEmission;

	Texture* TextureLoader::load2DTexture(std::string &path, TextureSettings *settings) {
		// Check the cache
		auto iter = m_TextureCache.find(path);
		if (iter != m_TextureCache.end()) {
			return iter->second;
		}

		// Load the texture
		int width, height, numComponents;
		unsigned char *data = stbi_load(path.c_str(), &width, &height, &numComponents, 0);
		if (!data) {
			Logger::getInstance().error("logged_files/texture_loading.txt", "texture load fail - path:", path);
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

		texture->generate2DTexture(width, height, dataFormat, data);

		m_TextureCache.insert(std::pair<std::string, Texture*>(path, texture));
		stbi_image_free(data);

		return m_TextureCache[path];
	}

	Cubemap* TextureLoader::loadCubemapTexture(const std::string &right, const std::string &left, const std::string &top, const std::string &bottom, const std::string &back, const std::string &front, CubemapSettings *settings) {
		Cubemap *cubemap = new Cubemap();
		if (settings != nullptr)
			cubemap->setCubemapSettings(*settings);

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

				cubemap->generateCubemapFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, width, height, dataFormat, data);
				stbi_image_free(data);
			}
			else {
				Logger::getInstance().error("logged_files/error.txt", "Cubemap initialization", "Couldn't load cubemap using 6 filepaths. Filepath error: " + faces[i]);
				stbi_image_free(data);
				return cubemap;
			}
		}
		
		return cubemap;
	}

	void TextureLoader::initializeDefaultTextures() {
		// Setup texture and minimal filtering because they are 1x1 textures so they require none
		TextureSettings srgbTextureSettings;
		srgbTextureSettings.IsSRGB = true;

		s_DefaultAlbedo = load2DTexture(std::string("res/textures/default/defaultAlbedo.png"), &srgbTextureSettings);
		s_DefaultAlbedo->bind();
		s_DefaultAlbedo->setAnisotropicFilteringMode(1.0f);
		s_DefaultAlbedo->setTextureMinFilter(GL_NEAREST);
		s_DefaultAlbedo->setTextureMagFilter(GL_NEAREST);
		s_DefaultNormal = load2DTexture(std::string("res/textures/default/defaultNormal.png"));
		s_DefaultNormal->bind();
		s_DefaultNormal->setAnisotropicFilteringMode(1.0f);
		s_DefaultNormal->setTextureMinFilter(GL_NEAREST);
		s_DefaultNormal->setTextureMagFilter(GL_NEAREST);
		s_FullMetallic = load2DTexture(std::string("res/textures/default/white.png"));
		s_FullMetallic->bind();
		s_FullMetallic->setAnisotropicFilteringMode(1.0f);
		s_FullMetallic->setTextureMinFilter(GL_NEAREST);
		s_FullMetallic->setTextureMagFilter(GL_NEAREST);
		s_NoMetallic = load2DTexture(std::string("res/textures/default/black.png"));
		s_NoMetallic->bind();
		s_NoMetallic->setAnisotropicFilteringMode(1.0f);
		s_NoMetallic->setTextureMinFilter(GL_NEAREST);
		s_NoMetallic->setTextureMagFilter(GL_NEAREST);
		s_FullRoughness = load2DTexture(std::string("res/textures/default/white.png"));
		s_FullRoughness->bind();
		s_FullRoughness->setAnisotropicFilteringMode(1.0f);
		s_FullRoughness->setTextureMinFilter(GL_NEAREST);
		s_FullRoughness->setTextureMagFilter(GL_NEAREST);
		s_NoRoughness = load2DTexture(std::string("res/textures/default/black.png"));
		s_NoRoughness->bind();
		s_NoRoughness->setAnisotropicFilteringMode(1.0f);
		s_NoRoughness->setTextureMinFilter(GL_NEAREST);
		s_NoRoughness->setTextureMagFilter(GL_NEAREST);
		s_DefaultAO = load2DTexture(std::string("res/textures/default/white.png"));
		s_DefaultAO->bind();
		s_DefaultAO->setAnisotropicFilteringMode(1.0f);
		s_DefaultAO->setTextureMinFilter(GL_NEAREST);
		s_DefaultAO->setTextureMagFilter(GL_NEAREST);
		s_DefaultEmission = load2DTexture(std::string("res/textures/default/black.png"), &srgbTextureSettings);
		s_DefaultEmission->bind();
		s_DefaultEmission->setAnisotropicFilteringMode(1.0f);
		s_DefaultEmission->setTextureMinFilter(GL_NEAREST);
		s_DefaultEmission->setTextureMagFilter(GL_NEAREST);
	}

}
