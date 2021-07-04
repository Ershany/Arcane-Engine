#pragma once

#include "Pane.h"

namespace Arcane
{
	class DebugPane : public Pane {
	public:
		DebugPane(glm::vec2 &panePosition);

		virtual void SetupPaneObjects();

		static inline void BindFxaaEnabled(bool *ptr) { s_FxaaEnabled = ptr; }
		static inline void BindCameraPositionValue(glm::vec3 *ptr) { s_CameraPosition = ptr; }
		static inline void BindGammaCorrectionValue(float *ptr) { s_GammaCorrectionValue = ptr; }
		static inline void BindExposureValue(float *ptr) { s_ExposureValue = ptr; }
		static inline void BindBloomThresholdValue(float *ptr) { s_BloomThresholdValue = ptr; }
		static inline void BindSsaoSampleRadiusValue(float *ptr) { s_SsaoSampleRadius = ptr; }
		static inline void BindSsaoStrengthValue(float *ptr) { s_SsaoStrength = ptr; }
		static inline void BindVignetteIntensityValue(float *ptr) { s_VignetteIntensity = ptr; }
		static inline void BindChromaticAberrationIntensityValue(float *ptr) { s_ChromaticAberrationIntensity = ptr; }
		static inline void BindFilmGrainIntensityValue(float *ptr) { s_FilmGrainIntensity = ptr; }
		static inline bool GetWireframeMode() { return s_WireframeMode; }
		static inline void SetWireframeMode(bool choice) { s_WireframeMode = choice; }
		static inline void BindSsaoEnabled(bool *ptr) { s_SsaoEnabled = ptr; }
		static inline void BindVignetteEnabled(bool *ptr) { s_VignetteEnabled = ptr; }
		static inline void BindChromaticAberrationEnabled(bool *ptr) { s_ChromaticAberrationEnabled = ptr; }
		static inline void BindFilmGrainEnabled(bool *ptr) { s_FilmGrainEnabled = ptr; }

	private:
		static glm::vec3 *s_CameraPosition;
		static bool *s_FxaaEnabled;
		static float *s_GammaCorrectionValue;
		static float *s_ExposureValue;
		static float *s_BloomThresholdValue;
		static bool s_WireframeMode;
		static bool* s_SsaoEnabled;
		static float *s_SsaoSampleRadius;
		static float *s_SsaoStrength;
		static bool* s_VignetteEnabled;
		static float *s_VignetteIntensity;
		static bool* s_ChromaticAberrationEnabled;
		static float *s_ChromaticAberrationIntensity;
		static bool* s_FilmGrainEnabled;
		static float *s_FilmGrainIntensity;
	};
}
