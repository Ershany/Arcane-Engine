#pragma once

#include <Arcane/Graphics/Renderer/Renderpass/RenderPass.h>
#include <Arcane/Graphics/Renderer/Renderpass/PostProcessPass.h>

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

		EditorPassOutput ExecuteEditorPass(Framebuffer *sceneFramebuffer, Framebuffer *extraFramebuffer, ICamera *camera);
	private:
		Shader *m_ColourWriteShader;
		Shader *m_OutlineShader;

		// Shader tweaks
		float m_OutlineSize = 10.0f;
		glm::vec3 m_OutlineColour = glm::vec3(0.34117f, 0.0f, 0.49804f);
	};
}
