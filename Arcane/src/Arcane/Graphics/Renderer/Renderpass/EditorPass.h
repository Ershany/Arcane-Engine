#pragma once

#include <Arcane/Graphics/Renderer/Renderpass/RenderPass.h>
#include <Arcane/Graphics/Renderer/Renderpass/PostProcessPass.h>
#include <Arcane/Scene/Entity.h>

namespace Arcane
{
	class Shader;
	class Scene;
	class ICamera;

	class EditorPass : public RenderPass
	{
	public:
		EditorPass(Scene *scene);
		virtual ~EditorPass();

		EditorPassOutput ExecuteEditorPass(Framebuffer *sceneFramebuffer, Framebuffer *extraFramebuffer1, Framebuffer *extraFramebuffer2, ICamera *camera);

		inline void SetFocusedEntity(Entity entity) { m_FocusedEntity = entity; }
	private:
		Shader *m_ColourWriteShader;
		Shader *m_OutlineShader;

		Entity m_FocusedEntity;

		// Shader tweaks
		float m_OutlineSize = 6.0f;
		glm::vec3 m_OutlineColour = glm::vec3(0.34117f, 0.0f, 0.49804f);
	};
}
