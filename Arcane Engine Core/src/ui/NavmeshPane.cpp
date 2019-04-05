#include "pch.h"
#include "NavmeshPane.h"

namespace arcane {

	float NavmeshPane::s_NavmeshSlope = 45.0f;

	NavmeshPane::NavmeshPane(glm::vec2& panePosition) : Pane(std::string("Navmesh Controls"), panePosition)
	{
	}

	void NavmeshPane::setupPaneObjects() {
		ImGui::SliderFloat("Slope", &s_NavmeshSlope, 0.0f, 90.0f);
		ImGui::Button("Regenerate", ImVec2(240, 20));
	}

}
