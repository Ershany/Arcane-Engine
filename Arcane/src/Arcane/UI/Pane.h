#pragma once

#include <Arcane/Vendor/Imgui/imgui.h>

namespace Arcane
{
	class Pane {
	public:
		Pane(std::string &paneName, glm::vec2 &paneSize);

		void Render();
	protected:
		virtual void SetupPaneObjects() = 0;

		std::string m_PaneName;
		glm::vec2 m_PaneSize;
	};
}
