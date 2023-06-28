#pragma once
#ifndef WATERPASS_H
#define WATERPASS_H

#ifndef RENDERPASS_H
#include <Arcane/graphics/Renderer/Renderpass/RenderPass.h>
#endif

#ifndef RENDERPASSTYPE_H
#include <Arcane/Graphics/Renderer/Renderpass/RenderPassType.h>
#endif

#ifndef QUAD_H
#include <Arcane/Graphics/Mesh/Common/Quad.h>
#endif

#ifndef TIMER_H
#include <Arcane/Util/Timer.h>
#endif

namespace Arcane
{
	class Shader;
	class Scene;
	class ICamera;
	class Texture;

	class WaterPass : public RenderPass
	{
	public:
		WaterPass(Scene *scene);
		virtual ~WaterPass() override;

		WaterPassOutput ExecuteWaterPass(ShadowmapPassOutput &inputShadowmapData, Framebuffer *inputFramebuffer, ICamera *camera);
	private:
		bool m_WaterEnabled;

		Framebuffer m_SceneReflectionFramebuffer, m_SceneRefractionFramebuffer;
#ifdef WATER_REFLECTION_USE_MSAA
		Framebuffer m_ResolveFramebuffer;
#endif

		Shader *m_WaterShader;
		Texture *m_WaveTexture, *m_WaterNormalMap;

		Quad m_WaterPlane;
		glm::vec3 m_WaterPos;
		float m_WaterScale;

		bool m_EnableClearWater, m_EnableShine;
		float m_WaterTiling;
		glm::vec3 m_WaterAlbedo;
		float m_AlbedoPower;
		float m_WaveSpeed;
		float m_WaveMoveFactor;
		float m_WaveStrength;
		float m_ShineDamper;
		float m_WaterNormalSmoothing;
		float m_DepthdampeningEffect;
		float m_ReflectionBias, m_RefractionBias;

		Timer m_EffectsTimer;
	};
}
#endif
