#include "arcpch.h"
#include "Pane.h"

#include <Arcane/Vendor/Imgui/imgui.h>

namespace Arcane
{
	Pane::Pane(std::string &paneName, glm::vec2 &paneSize) : m_PaneName(paneName), m_PaneSize(paneSize) {
	}

	void Pane::Render() {
		ImGui::Begin(m_PaneName.c_str());
		SetupPaneObjects();
		ImGui::End();
	}
}
