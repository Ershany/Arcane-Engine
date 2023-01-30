#pragma once

#include <Arcane/Editor/InspectorPanel.h>
#include <Arcane/Scene/Entity.h>
#include <Arcane/Scene/Scene.h>

namespace Arcane
{
	class ScenePanel
	{
	public:
		ScenePanel(Scene *scene, InspectorPanel *inspectorPanel);

		void OnImGuiRender();
	private:
		void DrawEntityNode(Entity entity);
	private:
		Scene *m_Scene;
		InspectorPanel *m_InspectorPanel;

		Entity m_SelectedEntity;
	};
}
