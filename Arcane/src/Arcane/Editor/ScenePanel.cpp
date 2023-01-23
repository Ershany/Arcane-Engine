#include "arcpch.h"
#include "ScenePanel.h"

#include <Arcane/Vendor/Imgui/imgui.h>

namespace Arcane
{ 
	ScenePanel::ScenePanel(Scene3D *scene, InspectorPanel *inspectorPanel) : m_Scene(scene), m_InspectorPanel(inspectorPanel), m_SelectedEntity(" ")
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
	//       Also using the name of the model is shitty. Comparing strings and relying on each string to be different is a recipe for disaster. Need to use the entity's id when it exists and store it instead of the m_SelectedEntity's model name
	void ScenePanel::DrawEntityNode(RenderableModel *entity)
	{
		std::string name = "Unnamed Entity";
		if (entity->GetModel())
			name = entity->GetModel()->GetNameRef();

		bool selected = (m_SelectedEntity == name);
		ImGui::PushID(name.c_str());
		if (ImGui::Selectable(name.c_str(), &selected))
		{
			m_InspectorPanel->SetFocusedEntity(entity);
			m_SelectedEntity = name;
		}
		ImGui::PopID();
	}
}
