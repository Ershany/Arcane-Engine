#pragma once
#ifndef SCENEPANEL_H
#define SCENEPANEL_H

#ifndef ENTITY_H
#include <Arcane/Scene/Entity.h>
#endif

namespace Arcane
{
	class InspectorPanel;

	class ScenePanel
	{
	public:
		ScenePanel(Scene *scene, InspectorPanel *inspectorPanel);

		void OnImGuiRender();

		inline Entity GetSelectedEntity() { return m_SelectedEntity; }
	private:
		void DrawEntityNode(Entity entity);
	private:
		Scene *m_Scene;
		InspectorPanel *m_InspectorPanel;

		Entity m_SelectedEntity;
	};
}
#endif
