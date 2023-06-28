#pragma once
#ifndef SHADOWMAPPASS_H
#define SHADOWMAPPASS_H

#ifndef CUBEMAPCAMERA_H
#include <Arcane/Graphics/Camera/CubemapCamera.h>
#endif

#ifndef RENDERPASS_H
#include <Arcane/Graphics/Renderer/Renderpass/RenderPass.h>
#endif

#ifndef RENDERPASSTYPE_H
#include <Arcane/Graphics/Renderer/Renderpass/RenderPassType.h>
#endif

namespace Arcane
{
	class Cubemap;
	class ICamera;
	class Scene;
	class Shader;
	class Framebuffer;

	class ShadowmapPass : public RenderPass {
	public:
		ShadowmapPass(Scene *scene);
		ShadowmapPass(Scene *scene, Framebuffer *customDirectionalLightShadowFramebuffer, Framebuffer *customSpotLightShadowFramebuffer, Cubemap *customPointLightShadowCubemap);
		virtual ~ShadowmapPass() override;

		ShadowmapPassOutput GenerateShadowmaps(ICamera *camera, bool renderOnlyStatic);
	private:
		void Init();
	private:
		Shader *m_ShadowmapShader, *m_ShadowmapSkinnedShader, *m_ShadowmapLinearShader, *m_ShadowmapLinearSkinnedShader;
		CubemapCamera m_CubemapCamera;
		Framebuffer m_EmptyFramebuffer; // Used for attaching to when rendering (like cubemap faces)

		// Option to use custom shadow framebuffers/cubemaps. Most will go through the light manager and request the specified resolutions for normal rendering
		Framebuffer *m_CustomDirectionalLightShadowFramebuffer = nullptr;
		Framebuffer *m_CustomSpotLightShadowFramebuffer = nullptr;
		Cubemap *m_CustomPointLightShadowCubemap = nullptr;
	};
}
#endif
