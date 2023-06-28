#pragma once
#ifndef RENDERPASSTYPE_H
#define RENDERPASSTYPE_H

#ifndef FRAMEBUFFER_H
#include <Arcane/Platform/OpenGL/Framebuffer/Framebuffer.h>
#endif

#ifndef GBUFFER_H
#include <Arcane/Platform/OpenGL/Framebuffer/GBuffer.h>
#endif

namespace Arcane
{
	class Cubemap;

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

		glm::mat4 spotLightViewProjMatrix;
		Framebuffer *spotLightShadowmapFramebuffer = nullptr;
		float spotLightShadowmapBias;

		bool hasPointLightShadows; // Need to have this since the point light shadow cubemap always needs to be bound even if we never use it (thanks to the OpenGL Driver)
		Cubemap* pointLightShadowCubemap = nullptr;
		float pointLightShadowmapBias;
		float pointLightFarPlane;
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
#endif
