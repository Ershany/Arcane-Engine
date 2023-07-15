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

	class WaterPass : public RenderPass
	{
	public:
		WaterPass(Scene *scene);
		virtual ~WaterPass() override;

		WaterPassOutput ExecuteWaterPass(ShadowmapPassOutput &inputShadowmapData, Framebuffer *inputFramebuffer, ICamera *camera);
	private:
		bool m_WaterEnabled;

		Shader *m_WaterShader;
		Quad m_WaterPlane;
		Timer m_EffectsTimer;
	};
}
#endif
