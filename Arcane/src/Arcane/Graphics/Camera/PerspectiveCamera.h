#pragma once
#ifndef PERSPECTIVECAMERA_H
#define PERSPECTIVECAMERA_H

#ifndef CAMERACONTROLLER_H
#include <Arcane/Graphics/Camera/CameraController.h>
#endif

namespace Arcane
{
	class PerspectiveCamera : public CameraController
	{
	public:
		PerspectiveCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f, float nearPlane = DEFAULT_NEAR_PLANE, float farPlane = DEFAULT_FAR_PLANE, 
			float fov = CAMERA_MAX_FOV);

		virtual glm::mat4 GetProjectionMatrix() override;

		virtual void ProcessCameraScroll(float yOffset) override;

		// Getters
		inline float GetFOV() const { return m_CurrentFOV; }
	private:
		float m_CurrentFOV;
	};
}
#endif
