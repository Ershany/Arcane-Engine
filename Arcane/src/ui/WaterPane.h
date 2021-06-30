#pragma once

#include "Pane.h"

namespace Arcane
{
	class WaterPane : public Pane
	{
	public:
		WaterPane(glm::vec2 &panePosition);

		virtual void setupPaneObjects();

		static inline void bindClearWater(bool *ptr) { s_ClearWater = ptr; }
		static inline void bindEnableShine(bool *ptr) { s_EnableShine = ptr; }
		static inline void bindWaterTiling(float *ptr) { s_WaterTiling = ptr; }
		static inline void bindWaterAlbedo(glm::vec3 *ptr) { s_WaterAlbedo = ptr; }
		static inline void bindAlbedoPower(float *ptr) { s_AlbedoPower = ptr; }
		static inline void bindWaveSpeed(float *ptr) { s_WaveSpeed = ptr; }
		static inline void bindWaveStrength(float *ptr) { s_WaveStrength = ptr; }
		static inline void bindShineDamper(float *ptr) { s_ShineDamper = ptr; }
		static inline void bindWaterNormalSmoothing(float *ptr) { s_WaterNormalSmoothing = ptr; }
		static inline void bindDepthDampeningEffect(float *ptr) { s_DepthDampeningEffect = ptr; }
		static inline void bindReflectionBias(float *ptr) { s_ReflectionBias = ptr; }
		static inline void bindRefractionBias(float *ptr) { s_RefractionBias = ptr; }
	private:
		static bool *s_ClearWater;
		static bool *s_EnableShine;
		static float *s_WaterTiling;
		static glm::vec3 *s_WaterAlbedo;
		static float *s_AlbedoPower;
		static float *s_WaveSpeed;
		static float *s_WaveStrength;
		static float *s_ShineDamper;
		static float *s_WaterNormalSmoothing;
		static float *s_DepthDampeningEffect;
		static float *s_ReflectionBias, *s_RefractionBias;
	};
}
