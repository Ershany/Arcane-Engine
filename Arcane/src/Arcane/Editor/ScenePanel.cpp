#include "arcpch.h"
#include "ScenePanel.h"

#include <Arcane/Scene/Scene.h>
#include <Arcane/Editor/InspectorPanel.h>
#include <Arcane/Vendor/Imgui/imgui.h>

namespace Arcane
{
	ScenePanel::ScenePanel(Scene *scene, InspectorPanel *inspectorPanel) : m_Scene(scene), m_InspectorPanel(inspectorPanel), m_SelectedEntity()
	{
	}

	void ScenePanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");
		{
			if (m_Scene)
			{
				m_Scene->m_Registry.each([this](auto entity)
				{
					DrawEntityNode({ m_Scene, entity });
				});
			}
		}
		ImGui::End();
	}

	int someNum = 0;
	void ScenePanel::DrawEntityNode(Entity entity)
	{
		const char *name = "Unnamed entity";
		if (entity.HasComponent<TagComponent>())
			name = entity.GetComponent<TagComponent>().Tag.c_str();

		bool selected = entity == m_SelectedEntity;
		ImGui::PushID(static_cast<std::uint32_t>(entity.m_Handle));
		if (ImGui::Selectable(name, &selected))
		{
			m_InspectorPanel->SetFocusedEntity(entity);
			m_SelectedEntity = entity;
		}
		ImGui::PopID();
	}
}
