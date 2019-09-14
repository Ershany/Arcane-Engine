#include "pch.h"
#include "RuntimePane.h"

namespace arcane {

	float RuntimePane::s_ShadowmapTimer = 0.0f;
	float RuntimePane::s_SsaoTimer = 0.0f;
	float RuntimePane::s_FxaaTimer = 0.0f;

	RuntimePane::RuntimePane(glm::vec2 &panePosition) : Pane(std::string("Runtime Analytics"), panePosition), m_ValueOffset(0), m_MaxFrametime(0), m_Frametimes()
	{
	}

	void RuntimePane::setupPaneObjects() {
		float frametime = 1000.0f / ImGui::GetIO().Framerate;
		ImGui::Text("Frametime: %.3f ms (FPS %.1f)", frametime, ImGui::GetIO().Framerate);
#if DEBUG_ENABLED
		if (frametime > m_MaxFrametime) {
			m_MaxFrametime = frametime;
		}
		
		m_ValueOffset = (m_ValueOffset + 1) % m_Frametimes.size();
		m_Frametimes[m_ValueOffset] = frametime;
		ImGui::PlotLines("", &m_Frametimes[0], m_Frametimes.size(), m_ValueOffset, (const char*)0, 0.0f, m_MaxFrametime + 1.0f, ImVec2(255, 70));

		ImGui::Text("Shadowmap Generation: %.6f ms", 1000.0f * s_ShadowmapTimer);
		ImGui::Text("SSAO Generation: %.6f ms", 1000.0f * s_SsaoTimer);
		ImGui::Text("FXAA: %.6f ms", 1000.0f * s_FxaaTimer);
#endif
	}

}
