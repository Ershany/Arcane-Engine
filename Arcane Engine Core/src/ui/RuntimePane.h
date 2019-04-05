#pragma once

#include "Pane.h"

namespace arcane {

	class RuntimePane : public Pane {
	public:
		RuntimePane(glm::vec2 &panePosition);

		virtual void setupPaneObjects();

		static inline float getShadowmapTimer() { return s_ShadowmapTimer; }
		static inline float getPostProcessTimer() { return s_PostProcessTimer; }
		static inline void setShadowmapTimer(float frameTime) { s_ShadowmapTimer = frameTime; }
		static inline void setPostProcessTimer(float frameTime) { s_PostProcessTimer = frameTime; }
	private:
		static float s_ShadowmapTimer;
		static float s_PostProcessTimer;
	};

}
