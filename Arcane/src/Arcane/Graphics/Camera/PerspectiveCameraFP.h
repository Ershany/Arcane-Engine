#pragma once
#ifndef FPSCAMERA_H
#define FPSCAMERA_H

#ifndef ICAMERA_H
#include <Arcane/Graphics/Camera/ICamera.h>
#endif

namespace Arcane
{
	enum Camera_Movement {
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		UPWARDS,
		DOWNWARDS
	};

	class FPSCamera : public ICamera {

#define FPSCAMERA_MAX_SPEED 5.0f
#define FPSCAMERA_ROTATION_SENSITIVITY 0.1f
#define FPSCAMERA_MAX_FOV 90.0f

	public:
		FPSCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f, float nearPlane = DEFAULT_NEAR_PLANE, float farPlane = DEFAULT_FAR_PLANE);
		FPSCamera(float xPos, float yPos, float zPos, float xUp, float yUp, float zUp, float yaw = -90.0f, float pitch = 0.0f, float nearPlane = DEFAULT_NEAR_PLANE, float farPlane = DEFAULT_FAR_PLANE);

		virtual glm::mat4 GetViewMatrix() override;
		virtual glm::mat4 GetProjectionMatrix() override;

		void ProcessInput(float deltaTime);

		virtual void InvertPitch() override;

		// Getters
		inline float GetYaw() const { return m_CurrentYaw; }
		inline float GetPitch() const { return m_CurrentPitch; }
		inline float GetMovementSpeed() const { return m_CurrentMovementSpeed; }
		inline float GetFOV() const { return m_CurrentFOV; }
		inline virtual const glm::vec3& GetFront() const override { return m_Front; }
		inline virtual const glm::vec3& GetUp() const override { return m_Up; }
	private:
		void UpdateCameraVectors();
		void ProcessCameraMovement(glm::vec3 &direction, float deltaTime);
		void ProcessCameraRotation(double xOffset, double yOffset, GLboolean constrainPitch = true);
		void ProcessCameraFOV(double yOffset);
	private:
		// Camera Attributes
		glm::vec3 m_Front, m_Up, m_Right, m_WorldUp;

		// Euler Angles
		float m_CurrentYaw;
		float m_CurrentPitch;

		float m_CurrentMovementSpeed;
		float m_CurrentFOV;
	};
}
#endif
