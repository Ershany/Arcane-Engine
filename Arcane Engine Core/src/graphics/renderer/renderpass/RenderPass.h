#pragma once

#include "RenderPassType.h"

#include <scene/Scene3D.h>

namespace arcane {

	class RenderPass {
	public:
		RenderPass(Scene3D *scene);
		virtual ~RenderPass();
	protected:
		GLCache *m_GLCache;

		Scene3D *m_ActiveScene;
	};

}
