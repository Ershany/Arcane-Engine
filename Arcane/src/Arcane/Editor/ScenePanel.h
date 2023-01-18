#pragma once

#include <Arcane/Editor/InspectorPanel.h>
#include <Arcane/Scene/Scene.h>

namespace Arcane
{
	class ScenePanel
	{
	public:
		ScenePanel(Scene *scene, InspectorPanel *inspectorPanel);

		void OnImGuiRender();
	private:
		// TODO: Should move to an entity system. This is needed for ECS as well. For now leave it as RenderableModel until new system is added
		void DrawEntityNode(RenderableModel *entity);
	private:
		Scene *m_Scene;
		InspectorPanel *m_InspectorPanel;

		std::string m_SelectedEntity;
	};
}
