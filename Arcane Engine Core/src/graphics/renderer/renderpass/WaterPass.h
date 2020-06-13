#pragma once

#include <graphics/renderer/renderpass/RenderPass.h>
#include <scene/Scene3D.h>
#include <utils/Timer.h>

namespace arcane
{
	class Shader;

	class WaterPass : public RenderPass
	{
	public:
		WaterPass(Scene3D *scene);
		virtual ~WaterPass() override;

		WaterPassOutput executeWaterPass(LightingPassOutput &postTransparency, ICamera *camera);
	private:
		bool m_WaterEnabled;

		Framebuffer m_SceneShadowFramebuffer, m_SceneReflectionFramebuffer, m_SceneRefractionFramebuffer;
#ifdef WATER_REFLECTION_USE_MSAA
		Framebuffer m_ResolveFramebuffer;
#endif

		Shader *m_WaterShader;
		Texture *m_WaveTexture, *m_WaterNormalMap;

		Quad m_WaterPlane;
		glm::vec3 m_WaterPos;
		float m_WaterScale;

		bool m_EnableClearWater;
		glm::vec3 m_WaterAlbedo;
		float m_AlbedoPower;
		float m_WaveSpeed;
		float m_WaveMoveFactor;

		Timer m_Timer;
	};
}
