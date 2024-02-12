#include "arcpch.h"
#include "CameraController.h"

#include <Arcane/Graphics/Window.h>
#include <Arcane/Input/InputManager.h>

namespace Arcane
{
	CameraController::CameraController(glm::vec3 position, glm::vec3 up, float yaw, float pitch, float nearPlane, float farPlane)
		: ICamera(position, nearPlane, farPlane), m_Front(glm::vec3(0.0f, 0.0f, -1.0f)), m_CurrentMovementSpeed(0.0f)
	{
		m_WorldUp = up;
		m_Up = up;
		m_CurrentYaw = yaw;
		m_CurrentPitch = pitch;
		UpdateCameraVectors();
	}

	glm::mat4 CameraController::GetViewMatrix()
	{
		return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
	}

	void CameraController::ProcessInput(float deltaTime)
	{
		// Movement speed
		if (InputManager::IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
			m_CurrentMovementSpeed = CAMERA_SPEED * 4.0f;
		else if (InputManager::IsKeyPressed(GLFW_KEY_LEFT_ALT))
			m_CurrentMovementSpeed = CAMERA_SPEED / 4.0f;
		else
			m_CurrentMovementSpeed = CAMERA_SPEED;

		// Camera movement
		glm::vec3 direction = glm::vec3(0.0f);
		if (InputManager::IsKeyPressed(GLFW_KEY_W))
			direction += m_Front;
		if (InputManager::IsKeyPressed(GLFW_KEY_S))
			direction -= m_Front;
		if (InputManager::IsKeyPressed(GLFW_KEY_A))
			direction -= m_Right;
		if (InputManager::IsKeyPressed(GLFW_KEY_D))
			direction += m_Right;
		if (InputManager::IsKeyPressed(GLFW_KEY_SPACE))
			direction += m_WorldUp;
		if (InputManager::IsKeyPressed(GLFW_KEY_LEFT_CONTROL))
			direction -= m_WorldUp;
#if 1 // Temporary controls for controller to test controller data
		direction += m_Front * -JoystickManager::GetLeftStick(0).y * CAMERA_SPEED;
		direction += m_Right * JoystickManager::GetLeftStick(0).x * CAMERA_SPEED;
		direction += m_WorldUp * JoystickManager::GetTriggers(0).y * CAMERA_SPEED;
		direction -= m_WorldUp * JoystickManager::GetTriggers(0).x * CAMERA_SPEED;
#endif
		ProcessCameraMovement(direction, deltaTime);

		// Camera FOV
		float controllerScrollDelta = (JoystickManager::GetButton(0, ARCANE_GAMEPAD_A) - JoystickManager::GetButton(0, ARCANE_GAMEPAD_B)) * deltaTime * CAMERA_SCROLL_SPEED;
		float mouseScrollDelta = (float)InputManager::GetScrollYDelta() * deltaTime * CAMERA_SCROLL_SPEED * 50.0f;
		ProcessCameraScroll(mouseScrollDelta + controllerScrollDelta);

		// Camera rotation
		float mouseXDelta = (float)InputManager::GetMouseXDelta() * CAMERA_ROTATION_SENSITIVITY_MOUSE * deltaTime;
		float mouseYDelta = (float)-InputManager::GetMouseYDelta() * CAMERA_ROTATION_SENSITIVITY_MOUSE * deltaTime;
		float controllerXDelta = (float)JoystickManager::GetRightStick(0).x * CAMERA_ROTATION_SENSITIVITY_CONTROLLER * deltaTime;
		float controllerYDelta = (float)-JoystickManager::GetRightStick(0).y * CAMERA_ROTATION_SENSITIVITY_CONTROLLER * deltaTime;
		ProcessCameraRotation(mouseXDelta + controllerXDelta, mouseYDelta + controllerYDelta, true);
	}

	void CameraController::InvertPitch()
	{
		m_CurrentPitch = -m_CurrentPitch;
		UpdateCameraVectors();
	}

	void CameraController::ProcessCameraMovement(glm::vec3 &direction, float deltaTime)
	{
		float velocity = m_CurrentMovementSpeed * deltaTime;
		m_Position += direction * velocity;
	}

	void CameraController::ProcessCameraRotation(double xOffset, double yOffset, GLboolean constrainPitch)
	{
		// Make sure the user isn't interacting with the UI
		if (!Window::GetHideCursor())
			return;

		m_CurrentYaw += (float)xOffset;
		m_CurrentPitch += (float)yOffset;

		// Constrain the pitch
		if (constrainPitch)
		{
			if (m_CurrentPitch > 89.0f)
			{
				m_CurrentPitch = 89.0f;
			}
			else if (m_CurrentPitch < -89.0f)
			{
				m_CurrentPitch = -89.0f;
			}
		}

		UpdateCameraVectors();
	}

	void CameraController::UpdateCameraVectors()
	{
		m_Front.x = cos(glm::radians(m_CurrentYaw)) * cos(glm::radians(m_CurrentPitch));
		m_Front.y = sin(glm::radians(m_CurrentPitch));
		m_Front.z = sin(glm::radians(m_CurrentYaw)) * cos(glm::radians(m_CurrentPitch));
		m_Front = glm::normalize(m_Front);

		m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
		m_Up = glm::normalize(glm::cross(m_Right, m_Front));
	}
}
