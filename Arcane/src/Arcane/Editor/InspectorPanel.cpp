#include "arcpch.h"
#include "InspectorPanel.h"

#include <Arcane/Util/Loaders/AssetManager.h>
#include <Arcane/Vendor/Imgui/imgui.h>

namespace Arcane
{
	InspectorPanel::InspectorPanel() : m_FocusedEntity()
	{

	}

	void InspectorPanel::OnImGuiRender()
	{
		ImGui::Begin("Inspector");
		{
			if (m_FocusedEntity)
			{
				// Display each component info for the selected entity
				if (m_FocusedEntity.HasComponent<TransformComponent>())
				{
					auto &transform = m_FocusedEntity.GetComponent<TransformComponent>();
					if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
					{
						DrawVec3Control("Translation", transform.Translation);
						glm::vec3 rotation = glm::degrees(transform.Rotation);
						DrawVec3Control("Rotation", rotation, 1.0f);
						transform.Rotation = glm::radians(rotation);
						DrawVec3Control("Scale", transform.Scale);
					}
				}
				
				if (m_FocusedEntity.HasComponent<MeshComponent>())
				{
					auto &meshComponent = m_FocusedEntity.GetComponent<MeshComponent>();
					if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
					{
						Material &meshMaterial = meshComponent.AssetModel->GetMeshes()[0].GetMaterial();
						const char *items[] = { "Opaque", "Transparent" };
						int choice = meshComponent.IsTransparent;
						ImGui::Combo("Rendering Mode", &choice, items, IM_ARRAYSIZE(items));
						meshComponent.IsTransparent = choice;
						ImGui::Text("Texture Maps");
						ImGui::Separator();

						ImGui::Image(meshMaterial.GetAlbedoMap() ? (ImTextureID)meshMaterial.GetAlbedoMap()->GetTextureId() : (ImTextureID)AssetManager::GetInstance().GetWhiteTexture()->GetTextureId(), ImVec2(50, 50), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1)); ImGui::SameLine();
						if (ImGui::IsItemHovered() && meshMaterial.GetAlbedoMap())
						{
							ImGui::BeginTooltip();
							ImGui::Image((ImTextureID)meshMaterial.GetAlbedoMap()->GetTextureId(), ImVec2(400, 400));
							ImGui::EndTooltip();
						}
						ImGui::Text("Albedo"); ImGui::SameLine();
						ImGui::ColorEdit4("##Albedo Colour", (float*)&meshMaterial.GetAlbedoColourRef(), ImGuiColorEditFlags_DisplayRGB);

						ImGui::Image(meshMaterial.GetRoughnessMap() ? (ImTextureID)meshMaterial.GetRoughnessMap()->GetTextureId() : (ImTextureID)AssetManager::GetInstance().GetWhiteTexture()->GetTextureId(), ImVec2(50, 50), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1)); ImGui::SameLine();
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

						ImGui::Image(meshMaterial.GetMetallicMap() ? (ImTextureID)meshMaterial.GetMetallicMap()->GetTextureId() : (ImTextureID)AssetManager::GetInstance().GetWhiteTexture()->GetTextureId(), ImVec2(50, 50), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1)); ImGui::SameLine();
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

						ImGui::Image(meshMaterial.GetNormalMap() ? (ImTextureID)meshMaterial.GetNormalMap()->GetTextureId() : (ImTextureID)AssetManager::GetInstance().GetWhiteTexture()->GetTextureId(), ImVec2(50, 50), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1)); ImGui::SameLine();
						if (ImGui::IsItemHovered() && meshMaterial.GetNormalMap())
						{
							ImGui::BeginTooltip();
							ImGui::Image((ImTextureID)meshMaterial.GetNormalMap()->GetTextureId(), ImVec2(400, 400));
							ImGui::EndTooltip();
						}
						ImGui::Text("Normals");

						ImGui::Image(meshMaterial.GetAmbientOcclusionMap() ? (ImTextureID)meshMaterial.GetAmbientOcclusionMap()->GetTextureId() : (ImTextureID)AssetManager::GetInstance().GetWhiteTexture()->GetTextureId(), ImVec2(50, 50), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1)); ImGui::SameLine();
						if (ImGui::IsItemHovered() && meshMaterial.GetAmbientOcclusionMap())
						{
							ImGui::BeginTooltip();
							ImGui::Image((ImTextureID)meshMaterial.GetAmbientOcclusionMap()->GetTextureId(), ImVec2(400, 400));
							ImGui::EndTooltip();
						}
						ImGui::Text("Ambient Occlusion");

						ImGui::Image(meshMaterial.GetDisplacementMap() ? (ImTextureID)meshMaterial.GetDisplacementMap()->GetTextureId() : (ImTextureID)AssetManager::GetInstance().GetWhiteTexture()->GetTextureId(), ImVec2(50, 50), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1)); ImGui::SameLine();
						if (ImGui::IsItemHovered() && meshMaterial.GetDisplacementMap())
						{
							ImGui::BeginTooltip();
							ImGui::Image((ImTextureID)meshMaterial.GetDisplacementMap()->GetTextureId(), ImVec2(400, 400));
							ImGui::EndTooltip();
						}
						ImGui::Text("Displacement");
					}
				}

				if (m_FocusedEntity.HasComponent<LightComponent>())
				{
					if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
					{
						auto &lightComponent = m_FocusedEntity.GetComponent<LightComponent>();

						const char *typeItems[] = { "Directional Light", "Point Light", "Spot Light" };
						int typeChoice = static_cast<int>(lightComponent.Type);
						ImGui::Combo("Type", &typeChoice, typeItems, IM_ARRAYSIZE(typeItems));
						lightComponent.Type = static_cast<LightType>(typeChoice);

						ImGui::Text("Colour"); ImGui::SameLine();
						ImGui::ColorEdit3("##Colour", (float*)&lightComponent.LightColour, ImGuiColorEditFlags_DisplayRGB);
						DrawFloatControl("Intensity", lightComponent.Intensity);
						if (lightComponent.Type != LightType::LightType_Directional)
						{
							DrawFloatControl("Range", lightComponent.AttenuationRange);
						}
						if (lightComponent.Type == LightType::LightType_Spot)
						{
							DrawFloatControl("Inner Cutoff Angle", lightComponent.InnerCutOff);
							DrawFloatControl("Outer Cutoff Angle", lightComponent.OuterCutOff);
						}
						ImGui::Checkbox("Static", &lightComponent.IsStatic);
						ImGui::Separator();

						ImGui::Checkbox("Cast Shadows", &lightComponent.CastShadows);
						if (lightComponent.CastShadows)
						{
							const char *shadowItems[] = { "Low", "Medium", "High", "Ultra", "Nightmare" };
							int shadowChoice = static_cast<int>(lightComponent.ShadowResolution);
							ImGui::Combo("Shadow Quality", &shadowChoice, shadowItems, IM_ARRAYSIZE(shadowItems));
							lightComponent.ShadowResolution = static_cast<ShadowQuality>(shadowChoice);
							DrawFloatControl("Shadow Bias", lightComponent.ShadowBias, 0.001f);
							DrawFloatControl("Near Plane", lightComponent.ShadowNearPlane, 0.01f);
							DrawFloatControl("Far Plane", lightComponent.ShadowFarPlane);
						}
					}
				}
			}
		}
		ImGui::End();
	}

	bool InspectorPanel::DrawVec3Control(const std::string &label, glm::vec3 &values, float speed /*= 0.1f*/, float resetValue /*= 0.0f*/, float columnWidth /*= 100.0f*/)
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

	bool InspectorPanel::DrawFloatControl(const std::string &label, float &value, float speed /*= 0.1f*/, float resetValue /*= 0.0f*/, float columnWidth /*= 200.0f*/)
	{
		bool modified = false;

		ImGui::Columns(2);
		ImGui::PushID(label.c_str());
		ImGui::PushItemWidth(80);
		ImGui::Text(label.c_str()); ImGui::SameLine(); ImGui::NextColumn();
		modified |= ImGui::DragFloat("##Value", &value, speed, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::Columns(1);
		ImGui::PopID();

		return modified;
	}
}
