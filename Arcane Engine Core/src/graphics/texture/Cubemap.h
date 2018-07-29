#pragma once

#include <gl/glew.h>
#include "../../Defs.h"

namespace arcane { namespace graphics {

	class Cubemap {
	public:
		Cubemap();
		~Cubemap();

		void generateCubemap();

		void bind(int unit = -1);
		void unbind();
	private:
		unsigned int m_CubemapId;

		unsigned int m_FaceWidth, m_FaceHeight;
		GLenum m_TextureFormat;

		// Texture wrapping options
		GLenum m_TextureWrapSMode = GL_CLAMP_TO_EDGE;
		GLenum m_TextureWrapTMode = GL_CLAMP_TO_EDGE;
		GLenum m_TextureWrapRMode = GL_CLAMP_TO_EDGE;

		// Texture filtering options
		GLenum m_TextureMinificationFilterMode = GL_LINEAR; // Filtering mode when the texture moves further away and multiple texels map to one pixel (trilinear for best quality)
		GLenum m_TextureMagnificationFilterMode = GL_LINEAR; // Filtering mode when the texture gets closer and multiple pixels map to a single texel (Never needs to be more than bilinear because that is as accurate as it gets in this sitation)
		float m_TextureAnisotropyLevel = ANISOTROPIC_FILTERING_LEVEL; // Specified independent of texture min and mag filtering, should be a power of 2 (1.0 means the usual isotropic texture filtering is used which means anisotropic filtering isn't used)

		bool m_HasMips = true;
		int m_MipBias = 0; // positive means blurrier textures selected, negative means sharper texture which can show texture aliasing
	};

} }