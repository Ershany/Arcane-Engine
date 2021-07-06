#pragma once

#include <Arcane/UI/Pane.h>

namespace Arcane
{
	class WaterPane : public Pane
	{
	public:
		WaterPane(glm::vec2 &panePosition);

		virtual void SetupPaneObjects();

		static inline void BindClearWater(bool *ptr) { s_ClearWater = ptr; }
		static inline void BindEnableShine(bool *ptr) { s_EnableShine = ptr; }
		static inline void BindWaterTiling(float *ptr) { s_WaterTiling = ptr; }
		static inline void BindWaterAlbedo(glm::vec3 *ptr) { s_WaterAlbedo = ptr; }
		static inline void BindAlbedoPower(float *ptr) { s_AlbedoPower = ptr; }
		static inline void BindWaveSpeed(float *ptr) { s_WaveSpeed = ptr; }
		static inline void BindWaveStrength(float *ptr) { s_WaveStrength = ptr; }
		static inline void BindShineDamper(float *ptr) { s_ShineDamper = ptr; }
		static inline void BindWaterNormalSmoothing(float *ptr) { s_WaterNormalSmoothing = ptr; }
		static inline void BindDepthDampeningEffect(float *ptr) { s_DepthDampeningEffect = ptr; }
		static inline void BindReflectionBias(float *ptr) { s_ReflectionBias = ptr; }
		static inline void BindRefractionBias(float *ptr) { s_RefractionBias = ptr; }
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
