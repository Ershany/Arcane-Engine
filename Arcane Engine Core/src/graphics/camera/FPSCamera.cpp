#include "pch.h"
#include "FPSCamera.h"

namespace arcane {

	FPSCamera::FPSCamera(glm::vec3 &position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 &up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f)
		: m_Front(glm::vec3(0.0f, 0.0f, -1.0f)), m_CurrentMovementSpeed(FPSCAMERA_MAX_SPEED), m_CurrentFOV(FPSCAMERA_MAX_FOV)
	{
		m_Position = position;
		m_WorldUp = up;
		m_Up = up;
		m_CurrentYaw = yaw;
		m_CurrentPitch = pitch;
		updateCameraVectors();

		DebugPane::bindCameraPositionValue(&m_Position);
	}

	FPSCamera::FPSCamera(float xPos, float yPos, float zPos, float xUp, float yUp, float zUp, float yaw = -90.0f, float pitch = 0.0f)
		: m_Front(glm::vec3(0.0f, 0.0f, -1.0f)), m_CurrentMovementSpeed(FPSCAMERA_MAX_SPEED), m_CurrentFOV(FPSCAMERA_MAX_FOV)
	{
		m_Position = glm::vec3(xPos, yPos, zPos);
		m_WorldUp = glm::vec3(xUp, yUp, zUp);
		m_CurrentYaw = yaw;
		m_CurrentPitch = pitch;
		updateCameraVectors();
	}

	glm::mat4 FPSCamera::getViewMatrix() {
		return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
	}

	glm::mat4 FPSCamera::getProjectionMatrix() {
		return glm::perspective(glm::radians(m_CurrentFOV), (float)Window::getResolutionWidth() / (float)Window::getResolutionHeight(), NEAR_PLANE, FAR_PLANE);
	}

	void FPSCamera::processInput(float deltaTime) {
		// Movement speed
		if (InputManager::isKeyPressed(GLFW_KEY_LEFT_SHIFT))
			m_CurrentMovementSpeed = FPSCAMERA_MAX_SPEED * 4.0f;
		else if (InputManager::isKeyPressed(GLFW_KEY_LEFT_ALT))
			m_CurrentMovementSpeed = FPSCAMERA_MAX_SPEED / 4.0f;
		else
			m_CurrentMovementSpeed = FPSCAMERA_MAX_SPEED;

		// Camera movement
		glm::vec3 direction = glm::vec3(0.0f);
		if (InputManager::isKeyPressed(GLFW_KEY_W))
			direction += m_Front;
		if (InputManager::isKeyPressed(GLFW_KEY_S))
			direction -= m_Front;
		if (InputManager::isKeyPressed(GLFW_KEY_A))
			direction -= m_Right;
		if (InputManager::isKeyPressed(GLFW_KEY_D))
			direction += m_Right;
		if (InputManager::isKeyPressed(GLFW_KEY_SPACE))
			direction += m_WorldUp;
		if (InputManager::isKeyPressed(GLFW_KEY_LEFT_CONTROL))
			direction -= m_WorldUp;
		direction += m_Front * JoystickManager::getLeftStick(0).y * 2.0f;
		direction += m_Right * JoystickManager::getLeftStick(0).x * 2.0f;
		direction += m_WorldUp * JoystickManager::getTriggers(0).y;
		direction -= m_WorldUp * JoystickManager::getTriggers(0).x;
		processCameraMovement(direction, deltaTime);


		// Camera FOV
		float scrollDelta = glm::clamp((float)(InputManager::getScrollYDelta() * 4.0 + (JoystickManager::getButton(0, ARCANE_GAMEPAD_A) - JoystickManager::getButton(0, ARCANE_GAMEPAD_B) * 2.0)), -4.0f, 4.0f);
		processCameraFOV(scrollDelta);

		// Camera rotation
		float mouseXDelta = (float)(InputManager::getMouseXDelta() + (JoystickManager::getRightStick(0).x * 20.0)) * FPSCAMERA_ROTATION_SENSITIVITY;
		float mouseYDelta = (float)(-InputManager::getMouseYDelta() + (JoystickManager::getRightStick(0).y * 20.0)) * FPSCAMERA_ROTATION_SENSITIVITY;
		processCameraRotation(mouseXDelta, mouseYDelta, true);
	}

	void FPSCamera::processCameraMovement(glm::vec3 &direction, float deltaTime) {
		float velocity = m_CurrentMovementSpeed * deltaTime;
		m_Position += direction * velocity;
	}

	void FPSCamera::processCameraRotation(double xOffset, double yOffset, GLboolean constrainPitch = true) {
		// Make sure the user isn't interacting with the UI
		if (!Window::getHideCursor())
			return;

		m_CurrentYaw += (float)xOffset;
		m_CurrentPitch += (float)yOffset;

		// Constrain the pitch
		if (constrainPitch) {
			if (m_CurrentPitch > 89.0f) {
				m_CurrentPitch = 89.0f;
			}
			else if (m_CurrentPitch < -89.0f) {
				m_CurrentPitch = -89.0f;
			}
		}

		updateCameraVectors();
	}

	void FPSCamera::processCameraFOV(double offset) {
		if (offset != 0.0 && m_CurrentFOV >= 1.0 && m_CurrentFOV <= FPSCAMERA_MAX_FOV) {
			m_CurrentFOV -= (float)offset;
		}
		if (m_CurrentFOV < 1.0f) {
			m_CurrentFOV = 1.0f;
		}
		else if (m_CurrentFOV > FPSCAMERA_MAX_FOV) {
			m_CurrentFOV = FPSCAMERA_MAX_FOV;
		}
	}

	void FPSCamera::updateCameraVectors() {
		m_Front.x = cos(glm::radians(m_CurrentYaw)) * cos(glm::radians(m_CurrentPitch));
		m_Front.y = sin(glm::radians(m_CurrentPitch));
		m_Front.z = sin(glm::radians(m_CurrentYaw)) * cos(glm::radians(m_CurrentPitch));
		m_Front = glm::normalize(m_Front);

		m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
		m_Up = glm::normalize(glm::cross(m_Right, m_Front));
	}

}
