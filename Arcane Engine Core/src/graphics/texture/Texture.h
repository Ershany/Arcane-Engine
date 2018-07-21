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
		void Generate2DTexture(unsigned int width, unsigned int height, GLenum textureFormat, GLenum dataFormat, const void *data);

		void Bind(int unit = -1);
		void Unbind();

		// Texture Tuning Functions
		void SetTextureWrapS(GLenum textureWrapMode, bool shouldBind = false);
		void SetTextureWrapT(GLenum textureWrapMode, bool shouldBind = false);
		void SetTextureMinFilter(GLenum textureFilterMode, bool shouldBind = false);
		void SetTextureMagFilter(GLenum textureFilterMode, bool shouldBind = false);
		void SetAnisotropicFilteringMode(float textureAnisotropyLevel, bool shouldBind = false);

		// Pre-generation controls only
		void SetMipMode(bool shouldGenMips, int mipBias);
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