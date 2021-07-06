#pragma once

#include <Arcane/UI/Pane.h>

namespace Arcane
{
	class RuntimePane : public Pane {
	public:
		RuntimePane(glm::vec2 &panePosition);

		virtual void SetupPaneObjects();

		static inline float GetShadowmapTimer() { return s_ShadowmapTimer; }
		static inline float GetSsaoTimer() { return s_SsaoTimer; }
		static inline float GetFxaaTimer() { return s_FxaaTimer; }
		static inline void SetShadowmapTimer(float frameTime) { s_ShadowmapTimer = frameTime; }
		static inline void SetSsaoTimer(float frameTime) { s_SsaoTimer = frameTime; }
		static inline void SetFxaaTimer(float frameTime) { s_FxaaTimer = frameTime; }
		static inline float GetWaterTimer() { return s_WaterTimer; }
		static inline void SetWaterTimer(float frameTime) { s_WaterTimer = frameTime; }
	private:
		static float s_ShadowmapTimer;
		static float s_SsaoTimer;
		static float s_FxaaTimer;
		static float s_WaterTimer;

		int m_ValueOffset;
		float m_MaxFrametime;
		std::array<float, 300> m_Frametimes;
	};
}
