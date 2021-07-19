#pragma once

#include "RenderPassType.h"

namespace Arcane
{
	class GLCache;
	class Scene3D;

	class RenderPass {
	public:
		RenderPass(Scene3D *scene);
		virtual ~RenderPass();
	protected:
		GLCache *m_GLCache;

		Scene3D *m_ActiveScene;
	};
}
