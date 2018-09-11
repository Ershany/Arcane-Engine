#include "Pane.h"

namespace arcane { namespace ui {

	Pane::Pane(std::string &paneName, glm::vec2 &panePositon) : m_PaneName(paneName), m_PanePosition(panePositon) {
	}

	void Pane::render() {
		ImGui::Begin(m_PaneName.c_str(), nullptr, ImVec2(m_PanePosition.x, m_PanePosition.y));
		setupPaneObjects();
		ImGui::End();
	}

} }