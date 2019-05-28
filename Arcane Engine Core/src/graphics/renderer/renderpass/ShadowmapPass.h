#pragma once

#include <graphics/camera/ICamera.h>
#include <graphics/renderer/renderpass/RenderPass.h>
#include <graphics/Shader.h>
#include <scene/Scene3D.h>

namespace arcane {

	class ShadowmapPass : public RenderPass {
	public:
		ShadowmapPass(Scene3D *scene);
		ShadowmapPass(Scene3D *scene, Framebuffer *customFramebuffer);
		virtual ~ShadowmapPass() override;

		ShadowmapPassOutput generateShadowmaps(ICamera *camera, bool renderOnlyStatic);
	private:
		bool m_AllocatedFramebuffer;
		Framebuffer *m_ShadowmapFramebuffer;
		Shader *m_ShadowmapShader;
	};

}
