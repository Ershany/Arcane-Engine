#pragma once

#include <string>
#include <glm/common.hpp>
#include "../vendor/imgui/imgui.h"

namespace arcane { namespace ui {

	class Pane {
	public:
		Pane(std::string &paneName, glm::vec2 &paneSize);

		void render();
	protected:
		virtual void setupPaneObjects() = 0;

		std::string m_PaneName;
		glm::vec2 m_PaneSize;
	};

} }