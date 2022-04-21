#include "arcpch.h"
#include "ScenePanel.h"

#include <Arcane/Vendor/Imgui/imgui.h>

namespace Arcane
{
	ScenePanel::ScenePanel(Scene3D *scene, InspectorPanel *inspectorPanel) : m_Scene(scene), m_InspectorPanel(inspectorPanel)
	{
	}

	void ScenePanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");
		{
			if (m_Scene)
			{
				for (auto renderableModel = m_Scene->GetRenderableModelsBegin(), renderableModelEnd = m_Scene->GetRenderableModelsEnd(); renderableModel != renderableModelEnd; ++renderableModel)
				{
					DrawEntityNode(*renderableModel);
				}
			}
		}
		ImGui::End();
	}

	// TODO: Should move to an entity system. This is needed for ECS as well. For now leave it as RenderableModel until new system is added
	void ScenePanel::DrawEntityNode(RenderableModel *entity)
	{
		const char *name = "Unnamed Entity";
		if (entity->GetModel())
			name = entity->GetModel()->GetName().c_str();

		if (ImGui::Selectable(name))
		{
			m_InspectorPanel->SetFocusedEntity(entity);
		}
	}
}
