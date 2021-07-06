#include "arcpch.h"
#include "RenderPass.h"

#include <Arcane/Graphics/Renderer/GLCache.h>
#include <Arcane/Scene/Scene3D.h>

namespace Arcane
{
	RenderPass::RenderPass(Scene3D *scene) : m_ActiveScene(scene) {
		m_GLCache = GLCache::GetInstance();
	}

	RenderPass::~RenderPass() {}
}
