#pragma once

namespace Arcane
{
	struct CubemapSettings {
		// Texture format
		GLenum TextureFormat = GL_NONE; // If set to GL_NONE, the data format will be used

		/* isSRGB will let the loader know that the texture needs to be "linearlized" before it is sampled in the shaders (ie the texture is in a non liner space)
		 * Anything that will be used for colour in a renderer should be linearlized. However textures that contain data (Heightfields, normal maps, metallic maps etc.) should not be,
		 * thus they are not in SRGB space. Note: If you generate your own data and it is already in linear space (like light probes), be careful */
		bool IsSRGB = false;

		// Texture wrapping options
		GLenum TextureWrapSMode = GL_CLAMP_TO_EDGE;
		GLenum TextureWrapTMode = GL_CLAMP_TO_EDGE;
		GLenum TextureWrapRMode = GL_CLAMP_TO_EDGE;

		// Texture filtering options
		GLenum TextureMinificationFilterMode = GL_LINEAR; // Filtering mode when the texture moves further away and multiple texels map to one pixel (trilinear for best quality)
		GLenum TextureMagnificationFilterMode = GL_LINEAR; // Filtering mode when the texture gets closer and multiple pixels map to a single texel (Never needs to be more than bilinear because that is as accurate as it gets in this sitation)
		float TextureAnisotropyLevel = ANISOTROPIC_FILTERING_LEVEL; // Specified independent of texture min and mag filtering, should be a power of 2 (1.0 means the usual isotropic texture filtering is used which means anisotropic filtering isn't used)

		// Mip Settings
		bool HasMips = false;
		int MipBias = 0; // positive means blurrier texture selected, negative means sharper texture which can show texture aliasing
	};

	class Cubemap {
	public:
		Cubemap();
		Cubemap(CubemapSettings &settings);
		~Cubemap();

		void GenerateCubemapFace(GLenum face, unsigned int faceWidth, unsigned int faceHeight, GLenum dataFormat, const unsigned char *data);

		void Bind(int unit = 0);
		void Unbind();

		// Pre-generation controls only
		inline void SetCubemapSettings(CubemapSettings settings) { m_CubemapSettings = settings; }

		// Getters
		unsigned int GetCubemapID() { return m_CubemapID; }
		inline unsigned int GetFaceWidth() { return m_FaceWidth; }
		inline unsigned int GetFaceHeight() { return m_FaceHeight; }
	private:
		void ApplyCubemapSettings();
	private:
		unsigned int m_CubemapID;

		unsigned int m_FaceWidth, m_FaceHeight;
		unsigned int m_FacesGenerated;

		CubemapSettings m_CubemapSettings;
	};
}
