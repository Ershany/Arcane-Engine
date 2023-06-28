#pragma once
#ifndef EDITORPASS_H
#define EDITORPASS_H

#ifndef RENDERPASS_H
#include <Arcane/Graphics/Renderer/Renderpass/RenderPass.h>
#endif

#ifndef ENTITY_H
#include <Arcane/Scene/Entity.h>
#endif

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
		Shader *m_ColourWriteShader, *m_ColourWriteShaderSkinned, *m_OutlineShader;
		Shader *m_UnlitSpriteShader;

		Entity m_FocusedEntity;

		// Editor textures
		Texture *m_DirectionalLightTexture, *m_PointLightTexture, *m_SpotLightTexture;

		// Shader tweaks
		float m_OutlineSize = 6.0f;
		glm::vec3 m_OutlineColour = glm::vec3(0.68507f, 0.0f, 1.0f);
	};
}
#endif
