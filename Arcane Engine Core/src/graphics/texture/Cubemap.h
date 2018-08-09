#pragma once

#include <gl/glew.h>
#include "../../Defs.h"

namespace arcane { namespace graphics {

	class Cubemap {
	public:
		Cubemap();
		~Cubemap();

		void generateCubemapFace(GLenum face, unsigned int faceWidth, unsigned int faceHeight, GLenum textureFormat, GLenum dataFormat, const unsigned char *data);

		void bind(int unit = -1);
		void unbind();
	private:
		// TODO: Look into better filtering like anisotropic support and also look into proper mips for cubemaps
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
	};

} }