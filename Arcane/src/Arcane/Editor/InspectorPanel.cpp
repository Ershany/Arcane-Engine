#include "arcpch.h"
#include "InspectorPanel.h"

#include <Arcane/Graphics/Mesh/Model.h>
#include <Arcane/Graphics/Mesh/Material.h>
#include <Arcane/Util/Loaders/AssetManager.h>
#include <Arcane/Vendor/Imgui/imgui.h>
#include <Arcane/Animation/AnimationClip.h>
#include <Arcane/Graphics/Texture/Texture3D.h>
#include <Arcane/Graphics/VolumetricClouds.h>

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
						DrawVec3Control("Rotation", rotation, 0.1f);
						rotation.x = fmod(rotation.x, 360.0f);
						rotation.y = fmod(rotation.y, 360.0f);
						rotation.z = fmod(rotation.z, 360.0f);
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
						ImGui::Checkbox("Is Static", &meshComponent.IsStatic);
						ImGui::Checkbox("Cull Backface", &meshComponent.ShouldBackfaceCull);
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
						if (meshMaterial.GetDisplacementMap())
						{
							ImGui::SliderFloat("Displacement Strength", &meshMaterial.GetDisplacementStrengthRef(), 0.0f, 1.0f, "%.2f");
							int minSteps = meshMaterial.GetDisplacementMinStepsRef();
							int maxSteps = meshMaterial.GetDisplacementMaxStepsRef();
							bool minModified = ImGui::SliderInt("Displacement Min Steps", &minSteps, 1, 40);
							bool maxModified = ImGui::SliderInt("Displacement Max Steps", &maxSteps, 1, 40);
							if (minModified)
							{
								if (minSteps > maxSteps)
									maxSteps = minSteps;
							}
							else if (maxModified)
							{
								if (maxSteps < minSteps)
									minSteps = maxSteps;
							}
							meshMaterial.SetDisplacementMinSteps(minSteps);
							meshMaterial.SetDisplacementMaxSteps(maxSteps);
						}

						ImGui::Image(meshMaterial.GetEmissionMap() ? (ImTextureID)meshMaterial.GetEmissionMap()->GetTextureId() : (ImTextureID)AssetManager::GetInstance().GetBlackTexture()->GetTextureId(), ImVec2(50, 50), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1)); ImGui::SameLine();
						if (ImGui::IsItemHovered() && meshMaterial.GetEmissionMap())
						{
							ImGui::BeginTooltip();
							ImGui::Image((ImTextureID)meshMaterial.GetEmissionMap()->GetTextureId(), ImVec2(400, 400));
							ImGui::EndTooltip();
						}
						ImGui::Text("Emission");
						if (meshMaterial.GetEmissionMap() || (meshMaterial.GetEmissionColourRef().r != 0.0f || meshMaterial.GetEmissionColourRef().g != 0.0f || meshMaterial.GetEmissionColourRef().b != 0.0f))
						{
							ImGui::SameLine();  ImGui::SliderFloat("##Emission Intensity", &meshMaterial.GetEmissionIntensityRef(), 1.0f, 255.0f, "%.0f");
						}
						ImGui::ColorEdit3("Emission Colour", (float*)&meshMaterial.GetEmissionColourRef(), ImGuiColorEditFlags_DisplayRGB);
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
						DrawFloatControl("Intensity", lightComponent.Intensity, 0.1f, 0.0f, 1000000.0f);
						if (lightComponent.Type != LightType::LightType_Directional)
						{
							DrawFloatControl("Range", lightComponent.AttenuationRange, 0.1f, 0.0f, 1000000.0f);
						}
						if (lightComponent.Type == LightType::LightType_Spot)
						{
							float innerDegrees = glm::degrees(glm::acos(lightComponent.InnerCutOff));
							float outerDegrees = glm::degrees(glm::acos(lightComponent.OuterCutOff));

							bool innerModified = DrawFloatControl("Inner Cutoff Angle", innerDegrees, 0.1f, 0.0f, 180.0f, "%.1f");
							bool outerModified = DrawFloatControl("Outer Cutoff Angle", outerDegrees, 0.1f, 0.0f, 180.0f, "%.1f");

							if (innerModified)
							{
								if (innerDegrees > outerDegrees)
									outerDegrees = innerDegrees;
							}
							else if (outerModified)
							{
								if (outerDegrees < innerDegrees)
									innerDegrees = outerDegrees;
							}

							lightComponent.InnerCutOff = glm::cos(glm::radians(innerDegrees));
							lightComponent.OuterCutOff = glm::cos(glm::radians(outerDegrees));
						}
						ImGui::Checkbox("Static", &lightComponent.IsStatic);
						ImGui::Separator();

						ImGui::Checkbox("Cast Shadows", &lightComponent.CastShadows);
						if (lightComponent.CastShadows)
						{
							const char *shadowItems[] = { "Low", "Medium", "High", "Ultra", "Nightmare" };
							glm::uvec2 resolution = LightManager::GetShadowQualityResolution(lightComponent.ShadowResolution);
							int shadowChoice = static_cast<int>(lightComponent.ShadowResolution);
							ImGui::Combo("Shadow Quality", &shadowChoice, shadowItems, IM_ARRAYSIZE(shadowItems)); ImGui::SameLine();
							ImGui::Text("- %u x %u", resolution.x, resolution.y);
							lightComponent.ShadowResolution = static_cast<ShadowQuality>(shadowChoice);
							DrawFloatControl("Shadow Bias", lightComponent.ShadowBias, 0.0001f, 0.0f, 1.0f, "%.4f");
							
							bool nearModified = DrawFloatControl("Near Plane", lightComponent.ShadowNearPlane, 0.01f, 0.0f, 100.0f);
							bool farModified = DrawFloatControl("Far Plane", lightComponent.ShadowFarPlane, 1.0f, 0.01f, 10000.0f);
							if (nearModified)
							{
								if (lightComponent.ShadowNearPlane > lightComponent.ShadowFarPlane)
									lightComponent.ShadowFarPlane = lightComponent.ShadowNearPlane;
							}
							else if (farModified)
							{
								if (lightComponent.ShadowFarPlane < lightComponent.ShadowNearPlane)
									lightComponent.ShadowNearPlane = lightComponent.ShadowFarPlane;
							}
						}
					}
				}

				if (m_FocusedEntity.HasComponent<WaterComponent>())
				{
					if (ImGui::CollapsingHeader("Water", ImGuiTreeNodeFlags_DefaultOpen))
					{
						auto &waterComponent = m_FocusedEntity.GetComponent<WaterComponent>();

						ImGui::ColorEdit3("##Water Albedo", (float*)&waterComponent.WaterAlbedo, ImGuiColorEditFlags_DisplayRGB);
						ImGui::SliderFloat("Albedo Power", &waterComponent.AlbedoPower, 0.0f, 1.0f, "%.2f");

						const char *qualityItems[] = { "Low", "Medium", "High", "Ultra", "Nightmare" };
						ImGui::Checkbox("Refraction Enabled", &waterComponent.RefractionEnabled);
						if (waterComponent.RefractionEnabled)
						{
							glm::uvec2 resolution = WaterManager::GetWaterReflectionRefractionQualityResolution(waterComponent.WaterRefractionResolution);
							int refractionChoice = static_cast<int>(waterComponent.WaterRefractionResolution);
							ImGui::Combo("Refraction Quality", &refractionChoice, qualityItems, IM_ARRAYSIZE(qualityItems)); ImGui::SameLine();
							ImGui::Text("- %u x %u", resolution.x, resolution.y);
							waterComponent.WaterRefractionResolution = static_cast<WaterReflectionRefractionQuality>(refractionChoice);
						}

						ImGui::Checkbox("Reflection Enabled", &waterComponent.ReflectionEnabled);
						if (waterComponent.ReflectionEnabled)
						{
							glm::uvec2 resolution = WaterManager::GetWaterReflectionRefractionQualityResolution(waterComponent.WaterReflectionResolution);
							int reflectionChoice = static_cast<int>(waterComponent.WaterReflectionResolution);
							ImGui::Combo("Reflection Quality", &reflectionChoice, qualityItems, IM_ARRAYSIZE(qualityItems)); ImGui::SameLine();
							ImGui::Text("- %u x %u", resolution.x, resolution.y);
							waterComponent.WaterReflectionResolution = static_cast<WaterReflectionRefractionQuality>(reflectionChoice);
						}

						ImGui::Checkbox("Reflection uses MSAA", &waterComponent.ReflectionMSAA);
						ImGui::Checkbox("Refraction uses MSAA", &waterComponent.RefractionMSAA);

						ImGui::Checkbox("Clear Water", &waterComponent.ClearWater);
						ImGui::Checkbox("Enable Shine", &waterComponent.EnableShine);

						DrawVec2Control("Water Tiling", waterComponent.WaterTiling, 0.01f, 0.1f, 100.0f);
						ImGui::SliderFloat("Wave Speed", &waterComponent.WaveSpeed, 0.0f, 2.0f, "%.3f");
						ImGui::SliderFloat("Wave Strength", &waterComponent.WaveStrength, 0.0f, 0.2f, "%.2f");

						ImGui::SliderFloat("Shine Damper", &waterComponent.ShineDamper, 0.1f, 1000.0f, "%.2f");
						if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
							ImGui::SetTooltip("Used to reduce bright specular reflections from lights on the surface of the water.");
						ImGui::SliderFloat("Normal Smoothing", &waterComponent.NormalSmoothing, 0.0f, 5.0f, "%.2f");
						if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
							ImGui::SetTooltip("Used to make normals have less x/y variance. Meaning a higher 'Normal Smoothing' value, the more the vectors from the normal map will be aligned with the water planes' normal.");
						ImGui::SliderFloat("Depth Dampening", &waterComponent.DepthDampening, 0.0f, 1.0f, "%.2f");
						if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
							ImGui::SetTooltip("Used to reduce distortion around edges because that leads to aliasing on reflection/refraction. So you can reduce distortion where there is less depth. Downsides of this is that it can reduce specular shimmer on the surface so adjust appropriately.");

						ImGui::SliderFloat("Reflection Plane Bias", &waterComponent.ReflectionPlaneBias, 0.0f, 10.0f, "%.2f");
						if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
							ImGui::SetTooltip("With distortion the planar reflection can cut off some things, this is used as a bias so more is rendered to the planar reflection. Downsides of this can be incorrect reflections");
						ImGui::SliderFloat("Refraction Plane Bias", &waterComponent.RefractionPlaneBias, 0.0f, 10.0f, "%.2f");
						if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
							ImGui::SetTooltip("With distortion the planar refraction can cut off some things, this is used as a bias so more is rendered to the planar refraction. Downsides of this can be incorrect refractions");

						bool reflectionNearModified = DrawFloatControl("Reflection Near Plane", waterComponent.ReflectionNearPlane, 0.01f, 0.0f, 100.0f);
						if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
							ImGui::SetTooltip("Controls the frustum's near plane when rendering reflections. If range is too big you can lose precision so pushing this out a bit can help alleviate precision issues and cost less to render");
						bool reflectionFarModified = DrawFloatControl("Reflection Far Plane", waterComponent.ReflectionFarPlane, 1.0f, 0.01f, 10000.0f);
						if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
							ImGui::SetTooltip("Controls the frustum's far plane when rendering reflections. If range is too big you can lose precision, also  can be used to reduce render times by reducing the far plane, since less things will be rendered");
						if (reflectionNearModified)
						{
							if (waterComponent.ReflectionNearPlane > waterComponent.ReflectionFarPlane)
								waterComponent.ReflectionFarPlane = waterComponent.ReflectionNearPlane;
						}
						else if (reflectionFarModified)
						{
							if (waterComponent.ReflectionFarPlane < waterComponent.ReflectionNearPlane)
								waterComponent.ReflectionNearPlane = waterComponent.ReflectionFarPlane;
						}

						bool refractionNearModified = DrawFloatControl("Refraction Near Plane", waterComponent.RefractionNearPlane, 0.01f, 0.0f, 100.0f);
						if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
							ImGui::SetTooltip("Controls the frustum's near plane when rendering refractions. If range is too big you can lose precision so pushing this out a bit can help alleviate precision issues and cost less to render");
						bool refractionFarModified = DrawFloatControl("Refraction Far Plane", waterComponent.RefractionFarPlane, 1.0f, 0.01f, 10000.0f);
						if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
							ImGui::SetTooltip("Controls the frustum's far plane when rendering refractions. If range is too big you can lose precision, also  can be used to reduce render times by reducing the far plane, since less things will be rendered");
						if (refractionNearModified)
						{
							if (waterComponent.RefractionNearPlane > waterComponent.RefractionFarPlane)
								waterComponent.RefractionFarPlane = waterComponent.RefractionNearPlane;
						}
						else if (refractionFarModified)
						{
							if (waterComponent.RefractionFarPlane < waterComponent.RefractionNearPlane)
								waterComponent.RefractionNearPlane = waterComponent.RefractionFarPlane;
						}

						ImGui::Image(waterComponent.WaterNormalMap ? (ImTextureID)waterComponent.WaterNormalMap->GetTextureId() : (ImTextureID)AssetManager::GetInstance().GetDefaultNormalTexture()->GetTextureId(), ImVec2(50, 50), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1)); ImGui::SameLine();
						if (ImGui::IsItemHovered() && waterComponent.WaterNormalMap)
						{
							ImGui::BeginTooltip();
							ImGui::Image((ImTextureID)waterComponent.WaterNormalMap->GetTextureId(), ImVec2(400, 400));
							ImGui::EndTooltip();
						}
						ImGui::Text("Water Normals");

						ImGui::Image(waterComponent.WaterDistortionTexture ? (ImTextureID)waterComponent.WaterDistortionTexture->GetTextureId() : (ImTextureID)AssetManager::GetInstance().GetDefaultWaterDistortionTexture()->GetTextureId(), ImVec2(50, 50), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1)); ImGui::SameLine();
						if (ImGui::IsItemHovered() && waterComponent.WaterDistortionTexture)
						{
							ImGui::BeginTooltip();
							ImGui::Image((ImTextureID)waterComponent.WaterDistortionTexture->GetTextureId(), ImVec2(400, 400));
							ImGui::EndTooltip();
						}
						ImGui::Text("Water Distortion");
					}
				}

				if (m_FocusedEntity.HasComponent<PoseAnimatorComponent>())
				{
					if (ImGui::CollapsingHeader("Pose Animator", ImGuiTreeNodeFlags_DefaultOpen))
					{
						auto &animatorComponent = m_FocusedEntity.GetComponent<PoseAnimatorComponent>();
						
						AnimationClip *clip = animatorComponent.PoseAnimator.GetCurrentAnimationClip();
#if !ARC_FINAL
						if (clip && !clip->GetAnimationName().empty())
						{
							ImGui::Text("Animation Name: %s", clip->GetAnimationName());
						}
#endif
					}
				}

				if (m_FocusedEntity.HasComponent<VolumetricCloudComponent>())
				{
					static int sliceToVisualize = 0;
					if (ImGui::CollapsingHeader("Volumetric Clouds", ImGuiTreeNodeFlags_DefaultOpen))
					{
						auto& volumetricCloudComponent = m_FocusedEntity.GetComponent<VolumetricCloudComponent>();

						bool shouldForceLoad = false;
						if (ImGui::Button("Generate Noise Texture"))
						{
							if (volumetricCloudComponent.GeneratedNoiseTexture3D)
							{
								// Delete the old Texture3D and we will generate a new one. This will re-use the ID so we need to force-load when visualizing the texture slice
								delete volumetricCloudComponent.GeneratedNoiseTexture3D;
								shouldForceLoad = true;
							}

							// Build the custom params for the noise gen algorithm
							NoiseTextureParams params;
							params.NoiseAlgorithm = volumetricCloudComponent.NoiseAlgorithm;
							volumetricCloudComponent.GeneratedNoiseTexture3D = VolumetricClouds::Generate3DNoiseTexture(params);
						}

						if (volumetricCloudComponent.GeneratedNoiseTexture3D)
						{
							bool minModified = ImGui::SliderInt(" - Slice", &sliceToVisualize, 0, volumetricCloudComponent.GeneratedNoiseTexture3D->GetDepth() - 1);
							Visualize3DTextureSlice(volumetricCloudComponent.GeneratedNoiseTexture3D, sliceToVisualize, shouldForceLoad);
						}

						const char* algorithmChoices[] = { "Worley", "Perlin" };
						int algorithmChoice = static_cast<int>(volumetricCloudComponent.NoiseAlgorithm);
						ImGui::Combo("Noise Algorithm", &algorithmChoice, algorithmChoices, IM_ARRAYSIZE(algorithmChoices)); ImGui::SameLine();
						volumetricCloudComponent.NoiseAlgorithm = static_cast<CloudNoiseAlgorithm>(algorithmChoice);
					}
				}
			}
		}
		ImGui::End();
	}

	bool InspectorPanel::DrawVec2Control(const std::string &label, glm::vec2 &values, float speed /*= 0.1f*/, float min /*= 0.0f*/, float max /*= 0.0f*/, const char *displayDigits /*= "%.2f"*/, float columnWidth /*= 100.0f*/)
	{
		bool modified = false;

		ImGui::Columns(3);
		ImGui::PushID(label.c_str());
		ImGui::PushItemWidth(80);
		ImGui::Text(label.c_str()); ImGui::SameLine(); ImGui::NextColumn();
		ImGui::Text("X"); ImGui::SameLine(); modified |= ImGui::DragFloat("##X", &values.x, speed, min, max, "%.2f"); ImGui::SameLine(); ImGui::NextColumn();
		ImGui::Text("Y"); ImGui::SameLine(); modified |= ImGui::DragFloat("##Y", &values.y, speed, min, max, "%.2f");
		ImGui::PopItemWidth();
		ImGui::Columns(1);
		ImGui::PopID();

		return modified;
	}

	bool InspectorPanel::DrawVec3Control(const std::string &label, glm::vec3 &values, float speed /*= 0.1f*/, float min /*= 0.0f*/, float max /*= 0.0f*/, const char *displayDigits /*= "%.2f"*/, float columnWidth /*= 100.0f*/)
	{
		bool modified = false;

		ImGui::Columns(4);
		ImGui::PushID(label.c_str());
		ImGui::PushItemWidth(80);
		ImGui::Text(label.c_str()); ImGui::SameLine(); ImGui::NextColumn();
		ImGui::Text("X"); ImGui::SameLine(); modified |= ImGui::DragFloat("##X", &values.x, speed, min, max, "%.2f"); ImGui::SameLine(); ImGui::NextColumn();
		ImGui::Text("Y"); ImGui::SameLine(); modified |= ImGui::DragFloat("##Y", &values.y, speed, min, max, "%.2f"); ImGui::SameLine(); ImGui::NextColumn();
		ImGui::Text("Z"); ImGui::SameLine(); modified |= ImGui::DragFloat("##Z", &values.z, speed, min, max, "%.2f");
		ImGui::PopItemWidth();
		ImGui::Columns(1);
		ImGui::PopID();

		return modified;
	}

	bool InspectorPanel::DrawFloatControl(const std::string &label, float &value, float speed /*= 0.1f*/, float min /*= 0.0f*/, float max /*= 0.0f*/, const char *displayDigits /*= "%.2f"*/, float columnWidth /*= 200.0f*/)
	{
		bool modified = false;

		ImGui::Columns(2);
		ImGui::PushID(label.c_str());
		ImGui::PushItemWidth(80);
		ImGui::Text(label.c_str()); ImGui::SameLine(); ImGui::NextColumn();
		modified |= ImGui::DragFloat("##Value", &value, speed, min, max, displayDigits);
		ImGui::PopItemWidth();
		ImGui::Columns(1);
		ImGui::PopID();

		return modified;
	}

	// Note: This function is only good if ImGui is only displaying one 3D texture a frame, if you are displaying multiple we will need to abstract this functionality properly
	// instead of uploading data constantly from VRAM -> CPU and back to GPU via a 2D Texture
	// TODO: Also would be nice to use Texture class instead of just storing indices of OpenGL objects
	void InspectorPanel::Visualize3DTextureSlice(Texture3D* texture3D, int sliceIndex, bool forceRegen /* = false*/)
	{
		if (texture3D == nullptr)
			return;

		static GLuint sliceTexture = 0;
		static int lastSliceIndex = -1;
		static int lastTextureID = -1;
		if (sliceTexture == 0) glGenTextures(1, &sliceTexture);

		int width = texture3D->GetWidth();
		int height = texture3D->GetHeight();
		int depth = texture3D->GetDepth();
		sliceIndex = glm::clamp(sliceIndex, 0, depth - 1);

		if (sliceIndex != lastSliceIndex || lastTextureID != texture3D->GetTextureId() || forceRegen)
		{
			lastSliceIndex = sliceIndex;
			lastTextureID = texture3D->GetTextureId();

			// Grab the slice from VRAM -> CPU
			texture3D->Bind(0);
			std::vector<unsigned char> sliceData(width * height * 3);
			glGetTextureSubImage(texture3D->GetTextureId(), 0, 0, 0, sliceIndex, width, height, 1, GL_RGB, GL_UNSIGNED_BYTE, sliceData.size(), sliceData.data());

			// Upload the slice to our slice texture for display
			glBindTexture(GL_TEXTURE_2D, sliceTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, sliceData.data());

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		ImGui::Text("Slice %d / %d", sliceIndex, depth - 1);
		ImGui::Image((void*)(intptr_t)sliceTexture, ImVec2(256, 256));
	}
}
