#pragma once

#include <Arcane/Platform/OpenGL/Framebuffer/Framebuffer.h>
#include <Arcane/Platform/OpenGL/Framebuffer/GBuffer.h>

namespace Arcane
{
	enum RenderPassType
	{
		MaterialRequired,
		NoMaterialRequired
	};

	struct PostProcessPassOutput
	{
		Framebuffer *outFramebuffer = nullptr;
	};

	struct ShadowmapPassOutput
	{
		glm::mat4 directionalLightViewProjMatrix;
		Framebuffer *directionalShadowmapFramebuffer = nullptr;
		float directionalShadowmapBias;
	};

	struct LightingPassOutput
	{
		Framebuffer *outputFramebuffer = nullptr;
	};

	struct WaterPassOutput
	{
		Framebuffer *outputFramebuffer = nullptr;
	};

	struct GeometryPassOutput
	{
		GBuffer *outputGBuffer = nullptr;
	};

	struct PreLightingPassOutput
	{
		Texture *ssaoTexture = nullptr;
	};

	struct EditorPassOutput
	{
		Framebuffer *outFramebuffer = nullptr;
	};
}
