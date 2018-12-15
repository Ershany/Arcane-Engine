#pragma once

#include <vendor/imgui/imgui.h>

namespace arcane {

	class Pane {
	public:
		Pane(std::string &paneName, glm::vec2 &paneSize);

		void render();
	protected:
		virtual void setupPaneObjects() = 0;

		std::string m_PaneName;
		glm::vec2 m_PaneSize;
	};

}
