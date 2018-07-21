#include "Texture.h"

namespace arcane { namespace graphics {

	Texture::Texture() {
		// Constructon handled by the texture loader unless for some reason you don't want to go through the texture loading system
	}

	Texture::~Texture() {
		// Destruction handled by the texture loader unless for some reason you don't want to go through the texture loading system
	}

	void Texture::Generate2DTexture(unsigned int width, unsigned int height, GLenum textureFormat, GLenum dataFormat, const void *data) {
		m_TextureTarget = GL_TEXTURE_2D;
		m_Width = width;
		m_Height = height;
		m_TextureFormat = textureFormat;

		glGenTextures(1, &m_TextureId);
		Bind();
		glTexImage2D(GL_TEXTURE_2D, 0, textureFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);

		// Texture wrapping
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_TextureWrapSMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_TextureWrapTMode);

		// Texture filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_TextureMinificationFilterMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_TextureMagnificationFilterMode);

		// Mipmapping
		if (m_HasMips) {
			glGenerateMipmap(GL_TEXTURE_2D); 
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, m_MipBias);
		}

		// Anisotropic filtering (TODO: Move the anistropyAmount calculation to Defs.h to avoid querying the OpenGL driver everytime)
		float maxAnisotropy;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
		float anistropyAmount = glm::min(maxAnisotropy, m_TextureAnisotropyLevel);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anistropyAmount);

		Unbind();
	}

	void Texture::Bind(int unit) {
		if (unit >= 0)
			glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(m_TextureTarget, m_TextureId);
	}

	void Texture::Unbind() {
		glBindTexture(m_TextureTarget, 0);
	}



	void Texture::SetTextureWrapS(GLenum textureWrapMode, bool shouldBind) {
		if (m_TextureWrapSMode == textureWrapMode)
			return;

		m_TextureWrapSMode = textureWrapMode;
		if (shouldBind)
			Bind();
		glTexParameteri(m_TextureTarget, GL_TEXTURE_WRAP_S, m_TextureWrapSMode);
	}

	void Texture::SetTextureWrapT(GLenum textureWrapMode, bool shouldBind) {
		if (m_TextureWrapTMode == textureWrapMode)
			return;

		m_TextureWrapTMode = textureWrapMode;
		if (shouldBind)
			Bind();
		glTexParameteri(m_TextureTarget, GL_TEXTURE_WRAP_T, m_TextureWrapTMode);
	}

	void Texture::SetTextureMinFilter(GLenum textureFilterMode, bool shouldBind) {
		if (m_TextureMinificationFilterMode == textureFilterMode)
			return;

		m_TextureMinificationFilterMode = textureFilterMode;
		if (shouldBind)
			Bind();
		glTexParameteri(m_TextureTarget, GL_TEXTURE_MIN_FILTER, m_TextureMinificationFilterMode);
	}

	void Texture::SetTextureMagFilter(GLenum textureFilterMode, bool shouldBind) {
		// If mag filter mode exceeds GL_Linear (bilinear) report an error because it is useless to perform more expensive filtering with magnification
		if (textureFilterMode > GL_LINEAR)
			utils::Logger::getInstance().warning("logged_files/textures.txt", "Texture Filter Tuning", "Texture's magnification filter exceeded bilinear filtering which won't result in any visual improvements and will just cost more");

		if (m_TextureMagnificationFilterMode == textureFilterMode)
			return;

		m_TextureMagnificationFilterMode = textureFilterMode;
		if (shouldBind)
			Bind();
		glTexParameteri(m_TextureTarget, GL_TEXTURE_MAG_FILTER, m_TextureMagnificationFilterMode);
	}

	void Texture::SetAnisotropicFilteringMode(float textureAnisotropyLevel, bool shouldBind) {
		if (m_TextureAnisotropyLevel == textureAnisotropyLevel)
			return;

		m_TextureAnisotropyLevel = textureAnisotropyLevel;
		if (shouldBind)
			Bind();

		float maxAnisotropy;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
		float anistropyAmount = glm::min(maxAnisotropy, m_TextureAnisotropyLevel);
		glTexParameterf(m_TextureTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, anistropyAmount);
	}


	void Texture::SetMipMode(bool shouldGenMips, int mipBias) {
		m_HasMips = shouldGenMips;
		m_MipBias = mipBias;
	}

} }