#pragma once
#ifndef RENDERPASS_H
#define RENDERPASS_H

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
#endif
