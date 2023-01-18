#pragma once

#include "RenderPassType.h"

namespace Arcane
{
	class GLCache;
	class Scene;

	class RenderPass {
	public:
		RenderPass(Scene *scene);
		virtual ~RenderPass();
	protected:
		GLCache *m_GLCache;

		Scene *m_ActiveScene;
	};
}
