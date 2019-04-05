#include "pch.h"
#include "RuntimePane.h"

namespace arcane {

	float RuntimePane::s_ShadowmapTimer = 0.0f;
	float RuntimePane::s_PostProcessTimer = 0.0f;

	RuntimePane::RuntimePane(glm::vec2 &panePosition) : Pane(std::string("Runtime Analytics"), panePosition)
	{
	}

	void RuntimePane::setupPaneObjects() {
		ImGui::Text("Frametime: %.3f ms (FPS %.1f)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
#if DEBUG_ENABLED
		ImGui::Text("Shadowmap Generation: %.6f ms", 1000.0f * s_ShadowmapTimer);
		ImGui::Text("Post Process: %.6f ms", 1000.0f * s_PostProcessTimer);
#endif
	}

}
