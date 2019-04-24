#include "pch.h"
#include "NavmeshPane.h"

namespace arcane {

	int NavmeshPane::s_RaycastType = 0;
	int NavmeshPane::s_SearchAlgo = 0;
	int NavmeshPane::s_HeuristicChoice = 0;
	float NavmeshPane::s_HeightRestriction = 115.0f;
	bool NavmeshPane::s_ShowNavmeshVertices = false;
	bool NavmeshPane::s_ShowNavmesh = false;
	bool NavmeshPane::s_ShowSearch = false;
	std::function<void()> NavmeshPane::s_RegenerationFuncPtr = nullptr;

	NavmeshPane::NavmeshPane(glm::vec2& panePosition) : Pane(std::string("Navmesh Controls"), panePosition)
	{
	}

	void NavmeshPane::setupPaneObjects() {
		ImGui::SliderFloat("Height", &s_HeightRestriction, 100.0f, 120.0f);

		ImGui::Checkbox("Show Navmesh Vertices", &s_ShowNavmeshVertices);
		ImGui::Checkbox("Show Navmesh", &s_ShowNavmesh);
		ImGui::Checkbox("Show Nodes Searched", &s_ShowSearch);

		ImGui::RadioButton("Move Agent", &s_RaycastType, 0); ImGui::SameLine();
		ImGui::RadioButton("Static Obstacle", &s_RaycastType, 1);

		ImGui::RadioButton("A*", &s_SearchAlgo, 0); ImGui::SameLine();
		ImGui::RadioButton("BFS", &s_SearchAlgo, 1); ImGui::SameLine();
		ImGui::RadioButton("DFS", &s_SearchAlgo, 2);

		if (s_SearchAlgo == SearchAlgo::AStar) {
			ImGui::RadioButton("Manhattan", &s_HeuristicChoice, 0); ImGui::SameLine();
			ImGui::RadioButton("Chebyshev", &s_HeuristicChoice, 1); ImGui::SameLine();
			ImGui::RadioButton("Euclidean", &s_HeuristicChoice, 2);
		}

		if (ImGui::Button("Regenerate", ImVec2(240, 20))) {
			s_RegenerationFuncPtr();
		}
	}

}
