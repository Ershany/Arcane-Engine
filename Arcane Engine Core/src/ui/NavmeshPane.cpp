#include "pch.h"
#include "NavmeshPane.h"

namespace arcane {

	float NavmeshPane::s_HeightRestriction = 115.0f;
	bool NavmeshPane::s_ShowNavmeshVertices = false;
	bool NavmeshPane::s_ShowNavmesh = false;
	std::function<void()> NavmeshPane::s_RegenerationFuncPtr = nullptr;

	NavmeshPane::NavmeshPane(glm::vec2& panePosition) : Pane(std::string("Navmesh Controls"), panePosition)
	{
	}

	void NavmeshPane::setupPaneObjects() {
		ImGui::SliderFloat("Height", &s_HeightRestriction, 100.0f, 120.0f);
		ImGui::Checkbox("Show Navmesh Vertices", &s_ShowNavmeshVertices);
		ImGui::Checkbox("Show Navmesh", &s_ShowNavmesh);
		if (ImGui::Button("Regenerate", ImVec2(240, 20))) {
			s_RegenerationFuncPtr();
		}
	}

}
