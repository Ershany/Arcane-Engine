#include "DebugPane.h"

namespace arcane { namespace ui {

	float* DebugPane::s_GammaCorrectionValue = nullptr;
	bool DebugPane::s_WireframeMode = false;

	DebugPane::DebugPane(glm::vec2 &panePosition) : Pane(std::string("Debug Controls"), panePosition)
	{
	}

	void DebugPane::setupPaneObjects() {
		if (s_GammaCorrectionValue != nullptr)
			ImGui::SliderFloat("Gamma", s_GammaCorrectionValue, 0.5f, 3.0f, "%.2f");
#if DEBUG_ENABLED
		ImGui::Text("Hit \"P\" to show/hide the cursor");
		ImGui::Checkbox("Wireframe Mode", &s_WireframeMode);
#endif
	}

} }