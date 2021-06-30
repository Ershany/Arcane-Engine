#include "pch.h"
#include "DebugPane.h"

namespace Arcane
{
	glm::vec3* DebugPane::s_CameraPosition = nullptr;
	bool* DebugPane::s_FxaaEnabled = nullptr;
	float* DebugPane::s_GammaCorrectionValue = nullptr;
	float* DebugPane::s_ExposureValue = nullptr;
	float* DebugPane::s_BloomThresholdValue = nullptr;
	bool* DebugPane::s_SsaoEnabled = nullptr;
	float* DebugPane::s_SsaoSampleRadius = nullptr;
	float* DebugPane::s_SsaoStrength = nullptr;
	bool* DebugPane::s_VignetteEnabled = nullptr;
	float* DebugPane::s_VignetteIntensity = nullptr;
	bool* DebugPane::s_ChromaticAberrationEnabled = nullptr;
	float* DebugPane::s_ChromaticAberrationIntensity = nullptr;
	bool* DebugPane::s_FilmGrainEnabled = nullptr;
	float* DebugPane::s_FilmGrainIntensity = nullptr;
	bool DebugPane::s_WireframeMode = false;

	DebugPane::DebugPane(glm::vec2 &panePosition) : Pane(std::string("Debug Controls"), panePosition)
	{
	}

	void DebugPane::setupPaneObjects() {
		if (s_CameraPosition != nullptr)
			ImGui::Text("Camera Pos x:%.1f y:%.1f z:%.1f", s_CameraPosition->x, s_CameraPosition->y, s_CameraPosition->z);
		if (s_FxaaEnabled != nullptr)
			ImGui::Checkbox("FXAA", s_FxaaEnabled);
		if (s_GammaCorrectionValue != nullptr)
			ImGui::SliderFloat("Gamma", s_GammaCorrectionValue, 0.5f, 3.0f, "%.2f");
		if (s_ExposureValue != nullptr)
			ImGui::SliderFloat("Exposure", s_ExposureValue, 0.1f, 5.0f, "%.2f");
		if (s_BloomThresholdValue != nullptr)
			ImGui::SliderFloat("Bloom Threshold", s_BloomThresholdValue, 0.1f, 10.0f, "%.2f");
		if (s_SsaoEnabled != nullptr)
			ImGui::Checkbox("SSAO Enabled", s_SsaoEnabled);
		if (s_SsaoSampleRadius != nullptr)
			ImGui::SliderFloat("SSAO Radius", s_SsaoSampleRadius, 1.0f, 3.0f, "%.2f");
		if (s_SsaoStrength != nullptr)
			ImGui::SliderFloat("SSAO Power", s_SsaoStrength, 1.0f, 5.0f, "%.2f");
		if (s_VignetteEnabled != nullptr)
			ImGui::Checkbox("Vignette Enabled", s_VignetteEnabled);
		if (s_VignetteIntensity != nullptr)
			ImGui::SliderFloat("Vignette Intensity", s_VignetteIntensity, 0.0f, 1.0f, "%.2f");
		if (s_ChromaticAberrationEnabled != nullptr)
			ImGui::Checkbox("Chromatic Aberration Enabled", s_ChromaticAberrationEnabled);
		if (s_ChromaticAberrationIntensity != nullptr)
			ImGui::SliderFloat("Chromatic Aberration Intensity", s_ChromaticAberrationIntensity, 0.0f, 1.0f, "%.2f");
		if (s_FilmGrainEnabled != nullptr)
			ImGui::Checkbox("Film Grain Enabled", s_FilmGrainEnabled);
		if (s_FilmGrainIntensity != nullptr)
			ImGui::SliderFloat("Film Grain Intensity", s_FilmGrainIntensity, 0.0f, 1.0f, "%.2f");
#if ARC_DEBUG
		ImGui::Text("Hit \"P\" to show/hide the cursor");
		ImGui::Checkbox("Wireframe Mode", &s_WireframeMode);
#endif // ARC_DEBUG
	}
}
