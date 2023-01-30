#include "arcpch.h"
#include "GraphicsSettings.h"

#include <Arcane/Util/Loaders/AssetManager.h>
#include <Arcane/Vendor/Imgui/imgui.h>

namespace Arcane
{
	GraphicsSettings::GraphicsSettings(MasterRenderPass *renderer) : m_MasterRenderPass(renderer)
	{

	}

	void GraphicsSettings::OnImGuiRender(bool *pOpen)
	{
		ImGui::SetNextWindowSize(ImVec2(1200, 1200));
		if (!ImGui::Begin("Graphics", pOpen)) // Optimization when window is minimized
		{
			ImGui::End();
			return;
		}

		PostProcessPass* postProcessPass = m_MasterRenderPass->GetPostProcessPass();

		ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_None;
		if (ImGui::BeginTabBar("Render Tabs", tabBarFlags))
		{
			if (ImGui::BeginTabItem("Render Features"))
			{
				if (ImGui::CollapsingHeader("Screen Space Ambient Occlusion (SSAO)", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Checkbox("Enabled", &postProcessPass->GetSsaoEnabledRef());
					ImGui::SliderFloat("Sample Radius", &postProcessPass->GetSsaoSampleRadiusRef(), 0.1f, 10.0f);
					ImGui::SliderFloat("Intensity", &postProcessPass->GetSsaoStrengthRef(), 0.1f, 10.0f);
				}
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Post Processing"))
			{
				if (ImGui::CollapsingHeader("Tonemap Settings", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::PushID("Tonemap Settings Arcane Effect");
					ImGui::SliderFloat("Gamma", &postProcessPass->GetGammaCorrectionRef(), 0.1f, 5.0f);
					ImGui::SliderFloat("Exposure", &postProcessPass->GetExposureRef(), 0.1f, 5.0f);
					ImGui::PopID();
				}
				ImGui::NewLine();
				ImGui::Separator();
				if (ImGui::CollapsingHeader("FXAA", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::PushID("FXAA Arcane Effect");
					ImGui::Checkbox("Enabled", &postProcessPass->GetFxaaEnabledRef());
					ImGui::PopID();
				}
				ImGui::NewLine();
				ImGui::Separator();
				if (ImGui::CollapsingHeader("Vignette", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::PushID("Vignette Arcane Effect");
					ImGui::Checkbox("Enabled", &postProcessPass->GetVignetteEnabledRef());
					Texture *vignetteTexture = postProcessPass->GetVignetteTexture();
					ImGui::Image(postProcessPass->GetVignetteTexture() ? (ImTextureID)vignetteTexture->GetTextureId() : (ImTextureID)AssetManager::GetInstance().GetWhiteTexture()->GetTextureId(), ImVec2(50, 50), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));
					if (ImGui::IsItemHovered() && vignetteTexture)
					{
						ImGui::BeginTooltip();
						ImGui::Image((ImTextureID)vignetteTexture->GetTextureId(), ImVec2(400, 400));
						ImGui::EndTooltip();
					}
					ImGui::ColorEdit3("Vignette Colour", (float*)&postProcessPass->GetVignetteColourRef(), ImGuiColorEditFlags_DisplayRGB);
					ImGui::SliderFloat("Intensity", &postProcessPass->GetVignetteIntensityRef(), 0.01f, 10.0f);
					ImGui::PopID();
				}
				ImGui::NewLine();
				ImGui::Separator();
				if (ImGui::CollapsingHeader("Film Grain", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::PushID("Film Grain Arcane Effect");
					ImGui::Checkbox("Enabled", &postProcessPass->GetFilmGrainEnabledRef());
					ImGui::SliderFloat("Intensity", &postProcessPass->GetFilmGrainIntensityRef(), 0.01f, 10.0f);
					ImGui::PopID();
				}
				ImGui::NewLine();
				ImGui::Separator();
				if (ImGui::CollapsingHeader("Chromatic Aberration", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::PushID("Chromatic Aberration Arcane Effect");
					ImGui::Checkbox("Enabled", &postProcessPass->GetChromaticAberrationEnabledRef());
					ImGui::SliderFloat("Intensity", &postProcessPass->GetChromaticAberrationIntensityRef(), 0.01f, 3.0f);
					ImGui::PopID();
				}

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Environment"))
			{
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Global Illumination"))
			{
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();

		ImGui::End();
	}
}
