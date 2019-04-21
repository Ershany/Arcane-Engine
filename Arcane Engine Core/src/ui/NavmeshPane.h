#pragma once

#include "Pane.h"

namespace arcane {

	enum RaycastType {
		Movement = 0,
		Static_Obstacle = 1,
	};

	enum SearchAlgo {
		AStar = 0,
		BFS = 1,
		DFS = 2,
	};

	enum HeuristicChoice {
		Manhattan = 0,
		Chebyshev = 1,
		Euclidean = 2,
	};

	class NavmeshPane : public Pane {
	public:
		NavmeshPane(glm::vec2& panePosition);

		virtual void setupPaneObjects();

		inline static int getRaycastType() { return s_RaycastType; }
		inline static int getSearchAlgo() { return s_SearchAlgo; }
		inline static int getHeuristicChoice() { return s_HeuristicChoice; }
		inline static float getHeightRestriction() { return s_HeightRestriction; }
		inline static bool getShowNavmeshVertices() { return s_ShowNavmeshVertices; }
		inline static bool getShowNavmesh() { return s_ShowNavmesh; }
		inline static void setShowNavmesh(bool showMesh) { s_ShowNavmesh = showMesh; }
		inline static void setRegenerationFunctionPtr(std::function<void()> funcPtr) { s_RegenerationFuncPtr = funcPtr; }
	private:
		static int s_RaycastType;
		static int s_SearchAlgo;
		static int s_HeuristicChoice;
		static float s_HeightRestriction;
		static bool s_ShowNavmeshVertices, s_ShowNavmesh;

		static std::function<void()> s_RegenerationFuncPtr;
	};

}
