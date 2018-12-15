#include "pch.h"
#include "Pane.h"

namespace arcane {

	Pane::Pane(std::string &paneName, glm::vec2 &paneSize) : m_PaneName(paneName), m_PaneSize(paneSize) {
	}

	void Pane::render() {
		ImGui::Begin(m_PaneName.c_str(), nullptr, ImVec2(m_PaneSize.x, m_PaneSize.y));
		setupPaneObjects();
		ImGui::End();
	}

}
