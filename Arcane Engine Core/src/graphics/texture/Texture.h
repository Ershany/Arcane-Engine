#pragma once

#include <GL/glew.h>
#include <glm/common.hpp>

#include "../../Defs.h"
#include "../../utils/Logger.h"

namespace arcane { namespace graphics {

	class Texture {
	public:
		Texture();
		~Texture();

		// Generation functions
		void generate2DTexture(unsigned int width, unsigned int height, GLenum textureFormat, GLenum dataFormat, const void *data);
		void generate2DMultisampleTexture(unsigned int width, unsigned int height, GLenum textureFormat, int numSamples);

		void bind(int unit = -1);
		void unbind();

		// Texture Tuning Functions (Works for pre-generation and post-generation)
		void setTextureWrapS(GLenum textureWrapMode, bool shouldBind = false);
		void setTextureWrapT(GLenum textureWrapMode, bool shouldBind = false);
		void setTextureMinFilter(GLenum textureFilterMode, bool shouldBind = false);
		void setTextureMagFilter(GLenum textureFilterMode, bool shouldBind = false);
		void setAnisotropicFilteringMode(float textureAnisotropyLevel, bool shouldBind = false);

		// Pre-generation controls only
		void setMipMode(bool shouldGenMips, int mipBias);

		// Don't use this to bind the texture and use it. Call the Bind() function instead
		unsigned int getTextureId() { return m_TextureId; }
	private:
		unsigned int m_TextureId;
		GLenum m_TextureTarget;

		unsigned int m_Width, m_Height;
		GLenum m_TextureFormat;

		// Texture wrapping options
		GLenum m_TextureWrapSMode = GL_REPEAT;
		GLenum m_TextureWrapTMode = GL_REPEAT;

		// Texture filtering options
		GLenum m_TextureMinificationFilterMode = GL_LINEAR_MIPMAP_LINEAR; // Filtering mode when the texture moves further away and multiple texels map to one pixel (trilinear for best quality)
		GLenum m_TextureMagnificationFilterMode = GL_LINEAR; // Filtering mode when the texture gets closer and multiple pixels map to a single texel (Never needs to be more than bilinear because that is as accurate as it gets in this sitation)
		float m_TextureAnisotropyLevel = ANISOTROPIC_FILTERING_LEVEL; // Specified independent of texture min and mag filtering, should be a power of 2 (1.0 means the usual isotropic texture filtering is used which means anisotropic filtering isn't used)

		bool m_HasMips = true;
		int m_MipBias = 0; // positive means blurrier textures selected, negative means sharper texture which can show texture aliasing
	};

} }