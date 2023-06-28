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
		FPSCamera(glm::vec3 &position, glm::vec3 &up, float yaw, float pitch);
		FPSCamera(float xPos, float yPos, float zPos, float xUp, float yUp, float zUp, float yaw, float pitch);

		virtual glm::mat4 GetViewMatrix() override;
		virtual glm::mat4 GetProjectionMatrix(float nearPlane = NEAR_PLANE, float farPlane = FAR_PLANE) override;

		void ProcessInput(float deltaTime);

		inline virtual void SetPosition(const glm::vec3 &position) override { m_Position = position; };
		virtual void InvertPitch() override;

		// Getters
		inline float GetYaw() const { return m_CurrentYaw; }
		inline float GetPitch() const { return m_CurrentPitch; }
		inline float GetMovementSpeed() const { return m_CurrentMovementSpeed; }
		inline float GetFOV() const { return m_CurrentFOV; }
		inline virtual const glm::vec3& GetPosition() const override { return m_Position; }
		inline virtual const glm::vec3& GetFront() const override { return m_Front; }
		inline virtual const glm::vec3& GetUp() const override { return m_Up; }
	private:
		void UpdateCameraVectors();
		void ProcessCameraMovement(glm::vec3 &direction, float deltaTime);
		void ProcessCameraRotation(double xOffset, double yOffset, GLboolean constrainPitch);
		void ProcessCameraFOV(double yOffset);
	private:
		// Camera Attributes
		glm::vec3 m_Position, m_Front, m_Up, m_Right, m_WorldUp;

		// Euler Angles
		float m_CurrentYaw;
		float m_CurrentPitch;

		float m_CurrentMovementSpeed;
		float m_CurrentFOV;
	};
}
#endif
