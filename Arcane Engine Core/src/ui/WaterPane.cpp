#include "pch.h"
#include "WaterPane.h"

namespace Arcane
{
	bool* WaterPane::s_ClearWater = nullptr;
	bool* WaterPane::s_EnableShine = nullptr;
	float* WaterPane::s_WaterTiling = nullptr;
	glm::vec3* WaterPane::s_WaterAlbedo = nullptr;
	float* WaterPane::s_AlbedoPower = nullptr;
	float* WaterPane::s_WaveSpeed = nullptr;
	float* WaterPane::s_WaveStrength = nullptr;
	float* WaterPane::s_ShineDamper = nullptr;
	float* WaterPane::s_WaterNormalSmoothing = nullptr;
	float* WaterPane::s_DepthDampeningEffect = nullptr;
	float* WaterPane::s_ReflectionBias = nullptr;
	float* WaterPane::s_RefractionBias = nullptr;

	WaterPane::WaterPane(glm::vec2 &panePosition) : Pane(std::string("Water Settings"), panePosition)
	{
	}

	void WaterPane::setupPaneObjects()
	{
#if ARC_DEBUG
		ImGui::Checkbox("Enable Clear Water", s_ClearWater);
		ImGui::Checkbox("Enable Shine", s_EnableShine);
		ImGui::SliderFloat3("Water Albedo", glm::value_ptr(*s_WaterAlbedo), 0.0f, 1.0f);
		ImGui::SliderFloat("Albedo Power", s_AlbedoPower, 0.0f, 1.0f);
		ImGui::SliderFloat("Water Tiling", s_WaterTiling, 0.001f, 1000.0f);
		ImGui::SliderFloat("Wave Speed", s_WaveSpeed, 0.0f, 1.0f);
		ImGui::SliderFloat("Wave Strength", s_WaveStrength, 0.01f, 0.1f);
		ImGui::SliderFloat("Shine Damper", s_ShineDamper, 1.0f, 100.0f);
		ImGui::SliderFloat("Normal Smoothing", s_WaterNormalSmoothing, 1.0f, 2.0f);
		ImGui::SliderFloat("Depth Dampening", s_DepthDampeningEffect, 0.01f, 1.0f);
		ImGui::SliderFloat("Reflection Bias", s_ReflectionBias, 0.01f, 5.0f);
		ImGui::SliderFloat("Refraction Bias", s_RefractionBias, 0.01f, 5.0f);
#endif // ARC_DEBUG
	}
}
