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
		static inline void bindSsaoSampleRadiusValue(float *ptr) { s_SsaoSampleRadius = ptr; }
		static inline void bindSsaoStrengthValue(float *ptr) { s_SsaoStrength = ptr; }
		static inline bool getWireframeMode() { return s_WireframeMode; }
		static inline void setWireframeMode(bool choice) { s_WireframeMode = choice; }
		static inline void bindSsaoEnabled(bool *ptr) { s_SsaoEnabled = ptr; }

	private:
		static glm::vec3 *s_CameraPosition;
		static bool *s_FxaaEnabled;
		static float *s_GammaCorrectionValue;
		static float *s_ExposureValue;
		static bool s_WireframeMode;
		static bool* s_SsaoEnabled;
		static float *s_SsaoSampleRadius;
		static float *s_SsaoStrength;
	};

}
