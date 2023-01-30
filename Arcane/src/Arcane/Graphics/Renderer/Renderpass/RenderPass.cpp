#include "arcpch.h"
#include "RenderPass.h"

#include <Arcane/Graphics/Renderer/GLCache.h>
#include <Arcane/Scene/Scene.h>

namespace Arcane
{
	RenderPass::RenderPass(Scene *scene) : m_ActiveScene(scene) {
		m_GLCache = GLCache::GetInstance();
	}

	RenderPass::~RenderPass() {}
}
