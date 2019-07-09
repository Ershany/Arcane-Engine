#include "pch.h"
#include "Cubemap.h"

namespace arcane {

	Cubemap::Cubemap(CubemapSettings &settings) : m_CubemapID(0), m_FaceWidth(0), m_FaceHeight(0), m_FacesGenerated(0), m_CubemapSettings(settings) {}

	Cubemap::~Cubemap() {
		glDeleteTextures(1, &m_CubemapID);
	}

	void Cubemap::generateCubemapFace(GLenum face, unsigned int faceWidth, unsigned int faceHeight, GLenum dataFormat, const unsigned char *data)
	{
		// Generate cubemap if this is the first face being generated
		if (m_CubemapID == 0) {
			glGenTextures(1, &m_CubemapID);

			m_FaceWidth = faceWidth;
			m_FaceHeight = faceHeight;

			// If GL_NONE is specified, set the texture format to the data format
			if (m_CubemapSettings.TextureFormat == GL_NONE) {
				m_CubemapSettings.TextureFormat = dataFormat;
			}
			// Check if the texture is SRGB, if so change the texture format
			if (m_CubemapSettings.IsSRGB) {
				switch (dataFormat) {
				case GL_RGB: m_CubemapSettings.TextureFormat = GL_SRGB; break;
				case GL_RGBA: m_CubemapSettings.TextureFormat = GL_SRGB_ALPHA; break;
				}
			}
		}

		bind();

		glTexImage2D(face, 0, m_CubemapSettings.TextureFormat, m_FaceWidth, m_FaceHeight, 0, dataFormat, GL_UNSIGNED_BYTE, data);
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
			if (m_CubemapSettings.HasMips) {
				glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_LOD_BIAS, m_CubemapSettings.MipBias);
			}
		}

		unbind();
	}

	void Cubemap::bind(int unit) {
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubemapID);
	}

	void Cubemap::unbind() {
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

}
