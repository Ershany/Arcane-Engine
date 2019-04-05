#pragma once

#include "Pane.h"

namespace arcane {

	class NavmeshPane : public Pane {
	public:
		NavmeshPane(glm::vec2& panePosition);

		virtual void setupPaneObjects();

		inline static float getNavmeshSlope() { return s_NavmeshSlope; }
		inline static void setNavmeshSlope(float slope) { s_NavmeshSlope = slope; }
	private:
		static float s_NavmeshSlope;
	};

}
