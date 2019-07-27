#pragma once

namespace arcane {

	struct TextureSettings {
		// Texture format
		GLenum TextureFormat = GL_NONE; // If set to GL_NONE, the data format will be used

		/* isSRGB will let the loader know that the texture needs to be "linearlized" before it is sampled in the shaders (ie the texture is in a non liner space)
		 * Anything that will be used for colour in a renderer should be linearlized. However textures that contain data (Heightfields, normal maps, metallic maps etc.) should not be,
		 * thus they are not in SRGB space. Note: If you generate your own data and it is already in linear space (like light probes), be careful */
		bool IsSRGB = false;

		// Texture wrapping options
		GLenum TextureWrapSMode = GL_REPEAT;
		GLenum TextureWrapTMode = GL_REPEAT;
		bool HasBorder = false;
		glm::vec4 BorderColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		// Texture filtering options
		GLenum TextureMinificationFilterMode = GL_LINEAR_MIPMAP_LINEAR; // Filtering mode when the texture moves further away and multiple texels map to one pixel (trilinear for best quality)
		GLenum TextureMagnificationFilterMode = GL_LINEAR; // Filtering mode when the texture gets closer and multiple pixels map to a single texel (Never needs to be more than bilinear because that is as accurate as it gets in this sitation)
		float TextureAnisotropyLevel = ANISOTROPIC_FILTERING_LEVEL; // Specified independent of texture min and mag filtering, should be a power of 2 (1.0 means the usual isotropic texture filtering is used which means anisotropic filtering isn't used)

		// Mip options
		bool HasMips = true;
		int MipBias = 0; // positive means blurrier texture selected, negative means sharper texture which can show texture aliasing
	};

	class Texture {
	public:
		// If nothing is supplied, it will construct default settings
		Texture(TextureSettings &settings = TextureSettings());
		~Texture();

		// Generation functions
		void generate2DTexture(unsigned int width, unsigned int height, GLenum dataFormat, GLenum pixelDataType = GL_UNSIGNED_BYTE, const void *data = nullptr);
		void generate2DMultisampleTexture(unsigned int width, unsigned int height);

		void bind(int unit = 0);
		void unbind();

		// Texture Tuning Functions (Works for pre-generation and post-generation)
		void setTextureWrapS(GLenum textureWrapMode);
		void setTextureWrapT(GLenum textureWrapMode);
		void setHasBorder(bool hasBorder);
		void setBorderColour(glm::vec4 &borderColour);
		void setTextureMinFilter(GLenum textureFilterMode);
		void setTextureMagFilter(GLenum textureFilterMode);
		void setAnisotropicFilteringMode(float textureAnisotropyLevel);
		void setMipBias(int mipBias);

		// Pre-generation controls only
		void setHasMips(bool hasMips);
		inline void setTextureSettings(TextureSettings settings) { m_TextureSettings = settings; }

		// Don't use this to bind the texture and use it. Call the Bind() function instead
		inline unsigned int getTextureId() { return m_TextureId; }
		inline bool isGenerated() { return m_TextureId != 0; }
		inline unsigned int getWidth() { return m_Width; }
		inline unsigned int getHeight() { return m_Height; }
		const TextureSettings& getTextureSettings() { return m_TextureSettings; }
	private:
		unsigned int m_TextureId;
		GLenum m_TextureTarget;

		unsigned int m_Width, m_Height;

		TextureSettings m_TextureSettings;
	};

}
