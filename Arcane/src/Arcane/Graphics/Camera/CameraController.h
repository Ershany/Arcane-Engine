#pragma once
#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#ifndef ICAMERA_H
#include <Arcane/Graphics/Camera/ICamera.h>
#endif

namespace Arcane
{
	enum Camera_Movement
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		UPWARDS,
		DOWNWARDS
	};

	class CameraController : public ICamera
	{
	public:
		CameraController(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f, float nearPlane = DEFAULT_NEAR_PLANE, float farPlane = DEFAULT_FAR_PLANE);

		virtual glm::mat4 GetViewMatrix() override;

		void ProcessInput(float deltaTime);
		virtual void ProcessCameraScroll(float yOffset) = 0;

		virtual void InvertPitch() override;

		// Getters
		inline float GetYaw() const { return m_CurrentYaw; }
		inline float GetPitch() const { return m_CurrentPitch; }
		inline float GetMovementSpeed() const { return m_CurrentMovementSpeed; }
		inline virtual const glm::vec3& GetFront() const override { return m_Front; }
		inline virtual const glm::vec3& GetUp() const override { return m_Up; }
	private:
		void UpdateCameraVectors();
		void ProcessCameraMovement(glm::vec3 &direction, float deltaTime);
		void ProcessCameraRotation(double xOffset, double yOffset, GLboolean constrainPitch = true);
	protected:
		// Camera Attributes
		glm::vec3 m_Front, m_Up, m_Right, m_WorldUp;

		// Euler Angles
		float m_CurrentYaw;
		float m_CurrentPitch;

		float m_CurrentMovementSpeed;
	};
}
#endif
