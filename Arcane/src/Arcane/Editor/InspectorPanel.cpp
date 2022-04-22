#include "arcpch.h"
#include "InspectorPanel.h"

#include <Arcane/Vendor/Imgui/imgui.h>

namespace Arcane
{
	InspectorPanel::InspectorPanel() : m_FocusedEntity(nullptr)
	{

	}

	void InspectorPanel::OnImGuiRender()
	{
		ImGui::Begin("Inspector");
		{
			if (m_FocusedEntity)
			{
				// TODO: Display each component info for the selected entity
				DrawVec3Control("Translation", m_FocusedEntity->GetPositionRef());
				glm::vec3 rotation = glm::degrees(glm::eulerAngles(m_FocusedEntity->GetOrientation()));
				DrawVec3Control("Rotation", rotation, 5.0f);
				m_FocusedEntity->SetOrientation(glm::quat(glm::radians(rotation)));
				DrawVec3Control("Scale", m_FocusedEntity->GetScaleRef());
			}
		}
		ImGui::End();
	}

	bool InspectorPanel::DrawVec3Control(const std::string &label, glm::vec3 &values, float speed, float resetValue, float columnWidth)
	{
		bool modified = false;

		ImGui::Columns(4);
		ImGui::PushID(label.c_str());
		ImGui::PushItemWidth(80);
		ImGui::Text(label.c_str()); ImGui::SameLine(); ImGui::NextColumn();
		ImGui::Text("X"); ImGui::SameLine(); modified |= ImGui::DragFloat("##X", &values.x, speed, 0.0f, 0.0f, "%.2f"); ImGui::SameLine(); ImGui::NextColumn();
		ImGui::Text("Y"); ImGui::SameLine(); modified |= ImGui::DragFloat("##Y", &values.y, speed, 0.0f, 0.0f, "%.2f"); ImGui::SameLine(); ImGui::NextColumn();
		ImGui::Text("Z"); ImGui::SameLine(); modified |= ImGui::DragFloat("##Z", &values.z, speed, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::Columns(1);
		ImGui::PopID();

		return modified;
	}
}
