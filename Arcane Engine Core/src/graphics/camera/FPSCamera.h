#pragma once

#include <graphics/camera/ICamera.h>
#include <graphics/Window.h>
#include <ui/DebugPane.h>

namespace arcane {

	enum Camera_Movement {
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		UPWARDS,
		DOWNWARDS
	};

	class FPSCamera : public ICamera {

#define FPSCAMERA_MAX_SPEED 40.0f
#define FPSCAMERA_ROTATION_SENSITIVITY 0.1f
#define FPSCAMERA_MAX_FOV 90.0f

	public:
		FPSCamera(glm::vec3 &position, glm::vec3 &up, float yaw, float pitch);
		FPSCamera(float xPos, float yPos, float zPos, float xUp, float yUp, float zUp, float yaw, float pitch);

		virtual glm::mat4 getViewMatrix() override;
		virtual glm::mat4 getProjectionMatrix() override;

		void processInput(float deltaTime);

		// Getters
		inline float getYaw() const { return m_CurrentYaw; }
		inline float getPitch() const { return m_CurrentPitch; }
		inline float getMovementSpeed() const { return m_CurrentMovementSpeed; }
		inline float getFOV() const { return m_CurrentFOV; }
		inline virtual const glm::vec3& getPosition() const override { return m_Position; }
		inline virtual const glm::vec3& getFront() const { return m_Front; }
		inline virtual const glm::vec3& getUp() const { return m_Up; }
		inline virtual void setPosition(const glm::vec3 &position) { m_Position = position; };
	private:
		void updateCameraVectors();
		void processCameraMovement(glm::vec3 &direction, float deltaTime);
		void processCameraRotation(double xOffset, double yOffset, GLboolean constrainPitch);
		void processCameraFOV(double yOffset);
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
