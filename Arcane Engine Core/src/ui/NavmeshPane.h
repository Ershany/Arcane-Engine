#pragma once

#include "Pane.h"

namespace arcane {

	class NavmeshPane : public Pane {
	public:
		NavmeshPane(glm::vec2& panePosition);

		virtual void setupPaneObjects();

		inline static float getHeightRestriction() { return s_HeightRestriction; }
		inline static bool getShowNavmeshVertices() { return s_ShowNavmeshVertices; }
		inline static bool getShowNavmesh() { return s_ShowNavmesh; }
		
		inline static void setRegenerationFunctionPtr(std::function<void()> funcPtr) { s_RegenerationFuncPtr = funcPtr; }
	private:
		static float s_HeightRestriction;
		static bool s_ShowNavmeshVertices, s_ShowNavmesh;

		static std::function<void()> s_RegenerationFuncPtr;
	};

}
