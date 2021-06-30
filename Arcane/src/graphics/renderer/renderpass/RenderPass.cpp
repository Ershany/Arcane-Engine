#include "arcpch.h"
#include "RenderPass.h"

namespace Arcane
{
	RenderPass::RenderPass(Scene3D *scene) : m_ActiveScene(scene) {
		m_GLCache = GLCache::getInstance();
	}

	RenderPass::~RenderPass() {}
}
