#pragma once

#include "Pane.h"

namespace arcane {

	class RuntimePane : public Pane {
	public:
		RuntimePane(glm::vec2 &panePosition);

		virtual void setupPaneObjects();

		static inline float getShadowmapTimer() { return s_ShadowmapTimer; }
		static inline float getSsaoTimer() { return s_SsaoTimer; }
		static inline float getFxaaTimer() { return s_FxaaTimer; }
		static inline void setShadowmapTimer(float frameTime) { s_ShadowmapTimer = frameTime; }
		static inline void setSsaoTimer(float frameTime) { s_SsaoTimer = frameTime; }
		static inline void setFxaaTimer(float frameTime) { s_FxaaTimer = frameTime; }
	private:
		static float s_ShadowmapTimer;
		static float s_SsaoTimer;
		static float s_FxaaTimer;
	};

}
