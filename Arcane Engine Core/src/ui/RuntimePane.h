#pragma once

#include "Pane.h"

namespace arcane {

	class RuntimePane : public Pane {
	public:
		RuntimePane(glm::vec2 &panePosition);

		virtual void setupPaneObjects();

		inline static float getShadowmapTimer() { return m_ShadowmapTimer; }
		inline static float getPostProcessTimer() { return m_PostProcessTimer; }
		inline static void setShadowmapTimer(float frameTime) { m_ShadowmapTimer = frameTime; }
		inline static void setPostProcessTimer(float frameTime) { m_PostProcessTimer = frameTime; }
	private:
		static float m_ShadowmapTimer;
		static float m_PostProcessTimer;
	};

}
