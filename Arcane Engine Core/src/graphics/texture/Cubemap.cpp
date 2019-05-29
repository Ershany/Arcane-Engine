#include "pch.h"
#include "Cubemap.h"

namespace arcane {

	Cubemap::Cubemap() : m_CubemapID(0), m_FaceWidth(0), m_FaceHeight(0), m_FacesGenerated(0), m_TextureFormat(0), m_CubemapSettings() {}

	Cubemap::Cubemap(CubemapSettings &settings) : m_CubemapID(0), m_FaceWidth(0), m_FaceHeight(0), m_FacesGenerated(0), m_TextureFormat(0), m_CubemapSettings(settings) {}

	Cubemap::~Cubemap() {
		glDeleteTextures(1, &m_CubemapID);
	}

	void Cubemap::generateCubemapFace(GLenum face, unsigned int faceWidth, unsigned int faceHeight, GLenum textureFormat, GLenum dataFormat, const unsigned char *data)
	{
		// Generate cubemap if this is the first face being generated
		if (m_CubemapID == 0) {
			glGenTextures(1, &m_CubemapID);

			m_TextureFormat = textureFormat;
			m_FaceWidth = faceWidth;
			m_FaceHeight = faceHeight;
		}

		bind();

		glTexImage2D(face, 0, m_TextureFormat, m_FaceWidth, m_FaceHeight, 0, dataFormat, GL_UNSIGNED_BYTE, data);
		++m_FacesGenerated;

		if (m_FacesGenerated >= 6) {
			// Texture filtering
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, m_CubemapSettings.TextureMagnificationFilterMode);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, m_CubemapSettings.TextureMinificationFilterMode);

			// Texture wrapping
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, m_CubemapSettings.TextureWrapSMode);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, m_CubemapSettings.TextureWrapTMode);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, m_CubemapSettings.TextureWrapRMode);

			// Mip settings
			if (m_CubemapSettings.HasMips)
				glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		}

		unbind();
	}

	void Cubemap::bind(int unit) {
		if (unit >= 0)
			glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubemapID);
	}

	void Cubemap::unbind() {
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

}
