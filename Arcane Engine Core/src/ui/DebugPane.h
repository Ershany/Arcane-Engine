#pragma once

#include "Pane.h"

namespace arcane {

	class DebugPane : public Pane {
	public:
		DebugPane(glm::vec2 &panePosition);

		virtual void setupPaneObjects();

		static inline void bindFxaaEnabled(bool *ptr) { s_FxaaEnabled = ptr; }
		static inline void bindCameraPositionValue(glm::vec3 *ptr) { s_CameraPosition = ptr; }
		static inline void bindGammaCorrectionValue(float *ptr) { s_GammaCorrectionValue = ptr; }
		static inline void bindExposureValue(float *ptr) { s_ExposureValue = ptr; }
		static inline void bindBloomThresholdValue(float *ptr) { s_BloomThresholdValue = ptr; }
		static inline void bindSsaoSampleRadiusValue(float *ptr) { s_SsaoSampleRadius = ptr; }
		static inline void bindSsaoStrengthValue(float *ptr) { s_SsaoStrength = ptr; }
		static inline void bindVignetteIntensityValue(float *ptr) { s_VignetteIntensity = ptr; }
		static inline void bindChromaticAberrationIntensityValue(float *ptr) { s_ChromaticAberrationIntensity = ptr; }
		static inline void bindFilmGrainIntensityValue(float *ptr) { s_FilmGrainIntensity = ptr; }
		static inline bool getWireframeMode() { return s_WireframeMode; }
		static inline void setWireframeMode(bool choice) { s_WireframeMode = choice; }
		static inline void bindSsaoEnabled(bool *ptr) { s_SsaoEnabled = ptr; }
		static inline void bindVignetteEnabled(bool *ptr) { s_VignetteEnabled = ptr; }
		static inline void bindChromaticAberrationEnabled(bool *ptr) { s_ChromaticAberrationEnabled = ptr; }
		static inline void bindFilmGrainEnabled(bool *ptr) { s_FilmGrainEnabled = ptr; }

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
