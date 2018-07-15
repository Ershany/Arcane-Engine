#include "Camera.h"

#include <iostream>

namespace arcane {	namespace graphics {

	Camera::Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
		: m_Front(glm::vec3(0.0f, 0.0f, -1.0f)), m_MovementSpeed(SPEED), m_MouseSensitivity(SENSITIVITY), m_FOV(FOV)
	{
		m_Position = position;
		m_WorldUp = up;
		m_Up = up;
		m_Yaw = yaw;
		m_Pitch = pitch;
		updateCameraVectors();
	}

	Camera::Camera(float xPos, float yPos, float zPos, float xUp, float yUp, float zUp, float yaw = YAW, float pitch = PITCH)
		: m_Front(glm::vec3(0.0f, 0.0f, -1.0f)), m_MovementSpeed(SPEED), m_MouseSensitivity(SENSITIVITY), m_FOV(FOV)
	{
		m_Position = glm::vec3(xPos, yPos, zPos);
		m_WorldUp = glm::vec3(xUp, yUp, zUp);
		m_Yaw = yaw;
		m_Pitch = pitch;
		updateCameraVectors();
	}

	glm::mat4 Camera::getViewMatrix() {
		return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
	}

	void Camera::processInput(float deltaTime) {
		// Keyboard input
		if (Window::isKeyPressed(GLFW_KEY_W))
			processKeyboard(arcane::graphics::FORWARD, deltaTime);
		if (Window::isKeyPressed(GLFW_KEY_S))
			processKeyboard(arcane::graphics::BACKWARD, deltaTime);
		if (Window::isKeyPressed(GLFW_KEY_A))
			processKeyboard(arcane::graphics::LEFT, deltaTime);
		if (Window::isKeyPressed(GLFW_KEY_D))
			processKeyboard(arcane::graphics::RIGHT, deltaTime);
		if (Window::isKeyPressed(GLFW_KEY_SPACE))
			processKeyboard(arcane::graphics::UPWARDS, deltaTime);
		if (Window::isKeyPressed(GLFW_KEY_LEFT_CONTROL))
			processKeyboard(arcane::graphics::DOWNWARDS, deltaTime);
		if (Window::isKeyPressed(GLFW_KEY_LEFT_SHIFT))
			m_MovementSpeed = SPEED * 4.0f;
		else
			m_MovementSpeed = SPEED;

		// Mouse scrolling
		processMouseScroll(Window::getScrollY() * 6);

		// Mouse movement
		processMouseMovement(Window::getMouseXDelta(), -Window::getMouseYDelta(), true);
	}

	void Camera::processKeyboard(Camera_Movement direction, float deltaTime) {
		float velocity = m_MovementSpeed * deltaTime;
		switch (direction) {
		case FORWARD:
			m_Position += m_Front * velocity;
			break;
		case BACKWARD:
			m_Position -= m_Front * velocity;
			break;
		case LEFT:
			m_Position -= m_Right * velocity;
			break;
		case RIGHT:
			m_Position += m_Right * velocity;
			break;
		case UPWARDS:
			m_Position += m_WorldUp * velocity;
			break;
		case DOWNWARDS:
			m_Position -= m_WorldUp * velocity;
			break;
		}
	}

	void Camera::processMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch = true) {
		xOffset *= m_MouseSensitivity;
		yOffset *= m_MouseSensitivity;

		m_Yaw += xOffset;
		m_Pitch += yOffset;

		// Constrain the pitch
		if (constrainPitch) {
			if (m_Pitch > 89.0f) {
				m_Pitch = 89.0f;
			}
			else if (m_Pitch < -89.0f) {
				m_Pitch = -89.0f;
			}
		}

		updateCameraVectors();
	}

	void Camera::processMouseScroll(float offset) {
		if (offset != 0 && m_FOV >= 1.0f && m_FOV <= FOV) {
			m_FOV -= offset;
		}
		if (m_FOV < 1.0f) {
			m_FOV = 1.0f;
		}
		else if (m_FOV > FOV) {
			m_FOV = FOV;
		}
	}

	void Camera::updateCameraVectors() {
		glm::vec3 front;
		front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		front.y = sin(glm::radians(m_Pitch));
		front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		m_Front = glm::normalize(front);

		// Recalculate Vectors
		m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
		m_Up = glm::normalize(glm::cross(m_Right, m_Front));
	}

} }