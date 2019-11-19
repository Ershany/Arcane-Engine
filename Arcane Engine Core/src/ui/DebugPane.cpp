#include "pch.h"
#include "DebugPane.h"

namespace arcane {

	glm::vec3* DebugPane::s_CameraPosition = nullptr;
	bool* DebugPane::s_FxaaEnabled = nullptr;
	float* DebugPane::s_GammaCorrectionValue = nullptr;
	float* DebugPane::s_ExposureValue = nullptr;
	bool* DebugPane::s_SsaoEnabled = nullptr;
	float* DebugPane::s_SsaoSampleRadius = nullptr;
	float* DebugPane::s_SsaoStrength = nullptr;
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
		if (s_SsaoEnabled != nullptr)
			ImGui::Checkbox("SSAO Enabled", s_SsaoEnabled);
		if (s_SsaoSampleRadius != nullptr)
			ImGui::SliderFloat("SSAO Radius", s_SsaoSampleRadius, 1.0f, 3.0f, "%.2f");
		if (s_SsaoStrength != nullptr)
			ImGui::SliderFloat("SSAO Power", s_SsaoStrength, 1.0f, 5.0f, "%.2f");
#if DEBUG_ENABLED
		ImGui::Text("Hit \"P\" to show/hide the cursor");
		ImGui::Checkbox("Wireframe Mode", &s_WireframeMode);
#endif
	}

}
