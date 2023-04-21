#include "arcpch.h"
#include "Framebuffer.h"

#include <Arcane/Util/Loaders/TextureLoader.h>

namespace Arcane
{
	Framebuffer::Framebuffer(unsigned int width, unsigned int height, bool isMultisampled)
		: m_FBO(0), m_Width(width), m_Height(height), m_IsMultisampled(isMultisampled), m_ColourTexture(), m_DepthStencilTexture(), m_DepthStencilRBO(0)
	{
		glGenFramebuffers(1, &m_FBO);
	}

	Framebuffer::~Framebuffer() {
		glDeleteRenderbuffers(1, &m_DepthStencilRBO);

		glDeleteFramebuffers(1, &m_FBO);
	}

	void Framebuffer::CreateFramebuffer() {
		Bind();
		if (!m_ColourTexture.IsGenerated()) {
			// Indicate that there won't be a colour buffer for this FBO
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}

		// Check if the creation failed
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			ARC_LOG_FATAL("Could not initialize the framebuffer");
			return;
		}
		Unbind();
	}

	Framebuffer& Framebuffer::AddColorTexture(ColorAttachmentFormat textureFormat) {
#ifdef ARC_DEV_BUILD
		if (m_ColourTexture.IsGenerated()) {
			ARC_LOG_ERROR("Framebuffer already has a colour attachment");
			return *this;
		}
#endif // ARC_DEV_BUILD
		ARC_ASSERT(m_Width > 0 && m_Height > 0, "Framebuffer width and height need to be > 0 to generate color texture");

		Bind();

		TextureSettings colourTextureSettings;
		colourTextureSettings.TextureFormat = textureFormat;
		colourTextureSettings.TextureWrapSMode = GL_CLAMP_TO_EDGE;
		colourTextureSettings.TextureWrapTMode = GL_CLAMP_TO_EDGE;
		colourTextureSettings.TextureMinificationFilterMode = GL_LINEAR;
		colourTextureSettings.TextureMagnificationFilterMode = GL_LINEAR;
		colourTextureSettings.TextureAnisotropyLevel = 1.0f;
		colourTextureSettings.HasMips = false;
		m_ColourTexture.SetTextureSettings(colourTextureSettings);

		// Generate colour texture attachment
		if (m_IsMultisampled) {
			m_ColourTexture.Generate2DMultisampleTexture(m_Width, m_Height);
			SetColorAttachment(m_ColourTexture.GetTextureId(), GL_TEXTURE_2D_MULTISAMPLE);
		}
		else {
			m_ColourTexture.Generate2DTexture(m_Width, m_Height, GL_RGB);
			SetColorAttachment(m_ColourTexture.GetTextureId(), GL_TEXTURE_2D);
		}

		Unbind();
		return *this;
	}

	Framebuffer& Framebuffer::AddDepthStencilTexture(DepthStencilAttachmentFormat textureFormat) {
#ifdef ARC_DEV_BUILD
		if (m_DepthStencilTexture.IsGenerated()) {
			ARC_LOG_ERROR("Framebuffer already has a depth attachment");
			return *this;
		}
#endif // ARC_DEV_BUILD
		ARC_ASSERT(m_Width > 0 && m_Height > 0, "Framebuffer width and height need to be > 0 to generate depth/stencil texture");

		GLenum attachmentType = GL_DEPTH_STENCIL_ATTACHMENT;
		if (textureFormat == NormalizedDepthOnly) {
			attachmentType = GL_DEPTH_ATTACHMENT;
		}

		Bind();

		TextureSettings depthStencilSettings;
		depthStencilSettings.TextureFormat = textureFormat;
		depthStencilSettings.TextureWrapSMode = GL_CLAMP_TO_BORDER;
		depthStencilSettings.TextureWrapTMode = GL_CLAMP_TO_BORDER;
		depthStencilSettings.TextureMinificationFilterMode = GL_NEAREST;
		depthStencilSettings.TextureMagnificationFilterMode = GL_NEAREST;
		depthStencilSettings.TextureAnisotropyLevel = 1.0f;
		depthStencilSettings.HasBorder = true;
		depthStencilSettings.BorderColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		depthStencilSettings.HasMips = false;
		m_DepthStencilTexture.SetTextureSettings(depthStencilSettings);

		// Generate depth attachment
		if (m_IsMultisampled) {
			m_DepthStencilTexture.Generate2DMultisampleTexture(m_Width, m_Height);
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D_MULTISAMPLE, m_DepthStencilTexture.GetTextureId(), 0);
		}
		else {
			m_DepthStencilTexture.Generate2DTexture(m_Width, m_Height, GL_DEPTH_COMPONENT);
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D, m_DepthStencilTexture.GetTextureId(), 0);
		}

		Unbind();
		return *this;
	}

	Framebuffer& Framebuffer::AddDepthStencilRBO(DepthStencilAttachmentFormat textureFormat) {
#ifdef ARC_DEV_BUILD
		if (m_DepthStencilRBO != 0) {
			ARC_LOG_ERROR("Framebuffer already has a depth+stencil RBO attachment");
			return *this;
		}
#endif // ARC_DEV_BUILD
		ARC_ASSERT(m_Width > 0 && m_Height > 0, "Framebuffer width and height need to be > 0 to generate depth/stencil attachment");

		Bind();

		GLenum attachmentType = GL_DEPTH_STENCIL_ATTACHMENT;
		if (textureFormat == NormalizedDepthOnly) {
			attachmentType = GL_DEPTH_ATTACHMENT;
		}

		// Generate depth+stencil RBO attachment
		glGenRenderbuffers(1, &m_DepthStencilRBO);
		glBindRenderbuffer(GL_RENDERBUFFER, m_DepthStencilRBO);

		if (m_IsMultisampled) {
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_SAMPLE_AMOUNT, textureFormat, m_Width, m_Height);
		}
		else {
			glRenderbufferStorage(GL_RENDERBUFFER, textureFormat, m_Width, m_Height);
		}

		// Attach depth+stencil attachment
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachmentType, GL_RENDERBUFFER, m_DepthStencilRBO);

		Unbind();
		return *this;
	}

	void Framebuffer::SetColorAttachment(unsigned int target, unsigned int targetType, int mipToWriteTo) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, targetType, target, mipToWriteTo);
	}

	void Framebuffer::SetDepthAttachment(DepthStencilAttachmentFormat textureFormat, unsigned int target, unsigned int targetType) {
		GLenum attachmentType = GL_DEPTH_STENCIL_ATTACHMENT;
		if (textureFormat == NormalizedDepthOnly)
		{
			attachmentType = GL_DEPTH_ATTACHMENT;
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, targetType, target, 0);
	}

	void Framebuffer::Bind() {
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	}

	void Framebuffer::Unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::ClearAll() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void Framebuffer::ClearColour()
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void Framebuffer::ClearDepth()
	{
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	void Framebuffer::ClearStencil()
	{
		glClear(GL_STENCIL_BUFFER_BIT);
	}
}
