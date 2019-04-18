#include "pch.h"
#include "NavmeshPane.h"

namespace arcane {

	float NavmeshPane::s_NavmeshSlope = 45.0f;
	std::function<void()> NavmeshPane::s_RegenerationFuncPtr = nullptr;

	NavmeshPane::NavmeshPane(glm::vec2& panePosition) : Pane(std::string("Navmesh Controls"), panePosition)
	{
	}

	void NavmeshPane::setupPaneObjects() {
		ImGui::SliderFloat("Slope", &s_NavmeshSlope, 0.1f, 89.9f);
		if (ImGui::Button("Regenerate", ImVec2(240, 20))) {
			s_RegenerationFuncPtr();
		}
	}

}
