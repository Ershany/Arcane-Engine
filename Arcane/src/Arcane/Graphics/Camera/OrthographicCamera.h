#pragma once
#ifndef ORTHOGRAPHICCAMERA_H
#define ORTHOGRAPHICCAMERA_H

#ifndef CAMERACONTROLLERFP_H
#include <Arcane/Graphics/Camera/CameraController.h>
#endif

namespace Arcane
{
	class OrthographicCamera : public CameraController
	{
	public:
		OrthographicCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f, float nearPlane = DEFAULT_NEAR_PLANE, float farPlane = DEFAULT_FAR_PLANE, 
			float left = DEFAULT_ORTHO_LEFT, float right = DEFAULT_ORTHO_RIGHT, float bottom = DEFAULT_ORTHO_BOTTOM, float top = DEFAULT_ORTHO_TOP, float aspectRatio = 16.0f/9.0f);

		virtual glm::mat4 GetProjectionMatrix() override;

		virtual void ProcessCameraScroll(float yOffset) override;
	private:
		float m_AspectRatio;
		float m_Zoom;
		float m_Left, m_Right, m_Bottom, m_Top;
	};
}
#endif
