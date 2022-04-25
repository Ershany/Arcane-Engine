#include "arcpch.h"
#include "InspectorPanel.h"

#include <Arcane/Util/Loaders/TextureLoader.h>
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
				if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
				{
					DrawVec3Control("Translation", m_FocusedEntity->GetPositionRef());
					glm::vec3 rotation = glm::degrees(glm::eulerAngles(m_FocusedEntity->GetOrientation()));
					DrawVec3Control("Rotation", rotation, 5.0f);
					m_FocusedEntity->SetOrientation(glm::quat(glm::radians(rotation)));
					DrawVec3Control("Scale", m_FocusedEntity->GetScaleRef());
				}
				if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
				{
					Material &meshMaterial = m_FocusedEntity->GetModel()->GetMeshes()[0].GetMaterial();
					const char *items[] = { "Opaque", "Transparent" };
					int choice = -1;
					ImGui::Combo("Rendering Mode", &choice, items, IM_ARRAYSIZE(items));
					ImGui::Text("Texture Maps");
					ImGui::Separator();

					ImGui::Image(meshMaterial.GetAlbedoMap() ? (ImTextureID)meshMaterial.GetAlbedoMap()->GetTextureId() : (ImTextureID)TextureLoader::GetWhiteTexture()->GetTextureId(), ImVec2(50, 50), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1)); ImGui::SameLine();
					if (ImGui::IsItemHovered() && meshMaterial.GetAlbedoMap())
					{
						ImGui::BeginTooltip();
						ImGui::Image((ImTextureID)meshMaterial.GetAlbedoMap()->GetTextureId(), ImVec2(400, 400));
						ImGui::EndTooltip();
					}
					ImGui::Text("Albedo"); ImGui::SameLine();
					ImGui::ColorEdit4("##Albedo Colour", (float*)&meshMaterial.GetAlbedoColourRef(), ImGuiColorEditFlags_DisplayRGB);

					ImGui::Image(meshMaterial.GetRoughnessMap() ? (ImTextureID)meshMaterial.GetRoughnessMap()->GetTextureId() : (ImTextureID)TextureLoader::GetWhiteTexture()->GetTextureId(), ImVec2(50, 50), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1)); ImGui::SameLine();
					if (ImGui::IsItemHovered() && meshMaterial.GetRoughnessMap())
					{
						ImGui::BeginTooltip();
						ImGui::Image((ImTextureID)meshMaterial.GetRoughnessMap()->GetTextureId(), ImVec2(400, 400));
						ImGui::EndTooltip();
					}
					ImGui::Text("Roughness");
					if (!meshMaterial.GetRoughnessMap())
					{
						ImGui::SameLine();
						ImGui::SliderFloat("##Roughness Slider", &meshMaterial.GetRoughnessValueRef(), 0.0f, 1.0f, "%.2f");
					}

					ImGui::Image(meshMaterial.GetMetallicMap() ? (ImTextureID)meshMaterial.GetMetallicMap()->GetTextureId() : (ImTextureID)TextureLoader::GetWhiteTexture()->GetTextureId(), ImVec2(50, 50), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1)); ImGui::SameLine();
					if (ImGui::IsItemHovered() && meshMaterial.GetMetallicMap())
					{
						ImGui::BeginTooltip();
						ImGui::Image((ImTextureID)meshMaterial.GetMetallicMap()->GetTextureId(), ImVec2(400, 400));
						ImGui::EndTooltip();
					}
					ImGui::Text("Metallic");
					if (!meshMaterial.GetMetallicMap())
					{
						ImGui::SameLine();
						ImGui::SliderFloat("##Metallic Slider", &meshMaterial.GetMetallicValueRef(), 0.0f, 1.0f, "%.2f");
					}

					ImGui::Image(meshMaterial.GetNormalMap() ? (ImTextureID)meshMaterial.GetNormalMap()->GetTextureId() : (ImTextureID)TextureLoader::GetWhiteTexture()->GetTextureId(), ImVec2(50, 50), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1)); ImGui::SameLine();
					if (ImGui::IsItemHovered() && meshMaterial.GetNormalMap())
					{
						ImGui::BeginTooltip();
						ImGui::Image((ImTextureID)meshMaterial.GetNormalMap()->GetTextureId(), ImVec2(400, 400));
						ImGui::EndTooltip();
					}
					ImGui::Text("Normals");

					ImGui::Image(meshMaterial.GetAmbientOcclusionMap() ? (ImTextureID)meshMaterial.GetAmbientOcclusionMap()->GetTextureId() : (ImTextureID)TextureLoader::GetWhiteTexture()->GetTextureId(), ImVec2(50, 50), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1)); ImGui::SameLine();
					if (ImGui::IsItemHovered() && meshMaterial.GetAmbientOcclusionMap())
					{
						ImGui::BeginTooltip();
						ImGui::Image((ImTextureID)meshMaterial.GetAmbientOcclusionMap()->GetTextureId(), ImVec2(400, 400));
						ImGui::EndTooltip();
					}
					ImGui::Text("Ambient Occlusion");

					ImGui::Image(meshMaterial.GetDisplacementMap() ? (ImTextureID)meshMaterial.GetDisplacementMap()->GetTextureId() : (ImTextureID)TextureLoader::GetWhiteTexture()->GetTextureId(), ImVec2(50, 50), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1)); ImGui::SameLine();
					if (ImGui::IsItemHovered() && meshMaterial.GetDisplacementMap())
					{
						ImGui::BeginTooltip();
						ImGui::Image((ImTextureID)meshMaterial.GetDisplacementMap()->GetTextureId(), ImVec2(400, 400));
						ImGui::EndTooltip();
					}
					ImGui::Text("Displacement");
				}
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
