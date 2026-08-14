#pragma once
#ifndef TEXTURE3D_H
#define TEXTURE3D_H

namespace Arcane
{
	struct Texture3DSettings
	{
		// Texture format
		GLenum TextureFormat = GL_NONE; // If set to GL_NONE, the data format will be used

		/* isSRGB will let the loader know that the texture needs to be "linearlized" before it is sampled in the shaders (ie the texture is in a non liner space)
		 * Anything that will be used for colour in a renderer should be linearlized. However textures that contain data (Heightfields, normal maps, metallic maps etc.) should not be,
		 * thus they are not in SRGB space. Note: If you generate your own data and it is already in linear space (like light probes), be careful */
		bool IsSRGB = false;

		// Texture wrapping options
		GLenum TextureWrapSMode = GL_REPEAT;
		GLenum TextureWrapTMode = GL_REPEAT;
		GLenum TextureWrapRMode = GL_REPEAT;
		bool HasBorder = false;
		glm::vec4 BorderColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // Only works when wrap mode set to GL_CLAMP_TO_BORDER

		// Texture filtering options
		GLenum TextureMinificationFilterMode = GL_LINEAR_MIPMAP_LINEAR; // Filtering mode when the texture moves further away and multiple texels map to one pixel (trilinear for best quality)
		GLenum TextureMagnificationFilterMode = GL_LINEAR; // Filtering mode when the texture gets closer and multiple pixels map to a single texel (Never needs to be more than bilinear because that is as accurate as it gets in this sitation)

		// Mip options
		bool HasMips = true;
		int MipBias = 0; // positive means blurrier texture selected, negative means sharper texture which can show texture aliasing
	};

	class Texture3D
	{
	public:
		Texture3D();
		Texture3D(Texture3DSettings& settings);
		Texture3D(const Texture3D& texture); // Copies another texture and its settings
		~Texture3D();

		// Generation functions
		void Generate3DTexture(unsigned int width, unsigned int height, unsigned int depth, GLenum dataFormat, GLenum pixelDataType = GL_UNSIGNED_BYTE, const void* data = nullptr);
		void GenerateMips(); // Generates mipmaps if the texture has already been created

		void Bind(int unit = 0) const;
		void Unbind() const;

		// Texture Tuning Functions (Works pre- and post-generation; for post, texture must be bound first)
		void SetTextureWrapS(GLenum textureWrapMode);
		void SetTextureWrapT(GLenum textureWrapMode);
		void SetTextureWrapR(GLenum textureWrapMode);
		void SetHasBorder(bool hasBorder);
		void SetBorderColour(glm::vec4& borderColour);
		void SetTextureMinFilter(GLenum textureFilterMode);
		void SetTextureMagFilter(GLenum textureFilterMode);
		void SetAnisotropicFilteringMode(float textureAnisotropyLevel);
		void SetMipBias(int mipBias);
		void SetHasMips(bool hasMips);

		// Pre-generation controls only
		inline void SetTextureSettings(Texture3DSettings settings) { m_Texture3DSettings = settings; }
		inline void SetTextureFormat(GLenum format) { m_Texture3DSettings.TextureFormat = format; }

		// Don't use this to bind the texture and use it. Call the Bind() function instead
		inline unsigned int GetTextureId() const { return m_Texture3DId; }
		inline unsigned int GetTextureTarget() const { return m_Texture3DTarget; }
		inline bool IsGenerated() const { return m_Texture3DId != 0; }
		inline unsigned int GetWidth() const { return m_Width; }
		inline unsigned int GetHeight() const { return m_Height; }
		inline unsigned int GetDepth() const { return m_Depth; }
		inline const Texture3DSettings& GetTextureSettings() const { return m_Texture3DSettings; }
	private:
		void ApplyTextureSettings();
	private:
		unsigned int m_Texture3DId;
		GLenum m_Texture3DTarget;

		unsigned int m_Width, m_Height, m_Depth;

		Texture3DSettings m_Texture3DSettings;
	};
}

#endif
