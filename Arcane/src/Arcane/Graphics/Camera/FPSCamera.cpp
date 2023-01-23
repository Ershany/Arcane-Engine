#include "arcpch.h"
#include "FPSCamera.h"

#include <Arcane/Graphics/Window.h>
#include <Arcane/UI/DebugPane.h>

namespace Arcane
{
	FPSCamera::FPSCamera(glm::vec3 &position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 &up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f)
		: m_Front(glm::vec3(0.0f, 0.0f, -1.0f)), m_CurrentMovementSpeed(FPSCAMERA_MAX_SPEED), m_CurrentFOV(FPSCAMERA_MAX_FOV)
	{
		m_Position = position;
		m_WorldUp = up;
		m_Up = up;
		m_CurrentYaw = yaw;
		m_CurrentPitch = pitch;
		UpdateCameraVectors();

		DebugPane::BindCameraPositionValue(&m_Position);
	}

	FPSCamera::FPSCamera(float xPos, float yPos, float zPos, float xUp, float yUp, float zUp, float yaw = -90.0f, float pitch = 0.0f)
		: m_Front(glm::vec3(0.0f, 0.0f, -1.0f)), m_CurrentMovementSpeed(FPSCAMERA_MAX_SPEED), m_CurrentFOV(FPSCAMERA_MAX_FOV)
	{
		m_Position = glm::vec3(xPos, yPos, zPos);
		m_WorldUp = glm::vec3(xUp, yUp, zUp);
		m_CurrentYaw = yaw;
		m_CurrentPitch = pitch;
		UpdateCameraVectors();
	}

	glm::mat4 FPSCamera::GetViewMatrix() {
		return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
	}

	glm::mat4 FPSCamera::GetProjectionMatrix() {
		return glm::perspective(glm::radians(m_CurrentFOV), (float)Window::GetRenderResolutionWidth() / (float)Window::GetRenderResolutionHeight(), NEAR_PLANE, FAR_PLANE);
	}

	void FPSCamera::ProcessInput(float deltaTime) {
		// You can specify NULL, NULL for the device to capture on if you have only one device and
		// either no windows at all or only one window, and it will capture from that device.
		// See the documentation below for a longer explanation
		// NOTE: MIGHT NOT BE A BAD IDEA TO ADD AN INDICATION THAT WE ARE CAPTURING
		auto renderdoc = Application::GetInstance().GetRenderdocApi();
		assert(renderdoc);

		// TODO: wait for a release of a button
		if (InputManager::IsKeyPressedDown(GLFW_KEY_F9) && !renderdoc->IsFrameCapturing())
		{
			ARC_LOG_INFO("Started renderdoc frame capture");
			renderdoc->StartFrameCapture(NULL, NULL);
			//assert(renderdoc->IsFrameCapturing());
		}

		if (InputManager::IsKeyPressedDown(GLFW_KEY_F10) && renderdoc->IsFrameCapturing())
		{
			ARC_LOG_INFO("Ended renderdoc frame capture");
			renderdoc->EndFrameCapture(NULL, NULL);
			//assert(!renderdoc->IsFrameCapturing());
		}

		if (InputManager::IsKeyPressedDown(GLFW_KEY_F11))
		{
			ARC_LOG_INFO("Getting a 1 time renderdoc frame capture");
			renderdoc->TriggerCapture();
			//assert(renderdoc->IsFrameCapturing());
		}

		// Movement speed
		if (InputManager::IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
			m_CurrentMovementSpeed = FPSCAMERA_MAX_SPEED * 4.0f;
		else if (InputManager::IsKeyPressed(GLFW_KEY_LEFT_ALT))
			m_CurrentMovementSpeed = FPSCAMERA_MAX_SPEED / 4.0f;
		else
			m_CurrentMovementSpeed = FPSCAMERA_MAX_SPEED;

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
#if 0 // Temporary controls for controller to test controller data
		direction += m_Front * JoystickManager::GetLeftStick(0).y * 2.0f;
		direction += m_Right * JoystickManager::GetLeftStick(0).x * 2.0f;
		direction += m_WorldUp * JoystickManager::GetTriggers(0).y;
		direction -= m_WorldUp * JoystickManager::GetTriggers(0).x;
#endif
		ProcessCameraMovement(direction, deltaTime);


		// Camera FOV
		float scrollDelta = glm::clamp((float)(InputManager::GetScrollYDelta() * 4.0 + (JoystickManager::GetButton(0, ARCANE_GAMEPAD_A) - JoystickManager::GetButton(0, ARCANE_GAMEPAD_B) * 2.0)), -4.0f, 4.0f);
		ProcessCameraFOV(scrollDelta);

		// Camera rotation
		float mouseXDelta = (float)InputManager::GetMouseXDelta() /*+ ((float)JoystickManager::GetRightStick(0).x * 20.0)*/ * FPSCAMERA_ROTATION_SENSITIVITY;
		float mouseYDelta = (float)-InputManager::GetMouseYDelta() /*+ ((float)JoystickManager::GetRightStick(0).y * 20.0)*/ * FPSCAMERA_ROTATION_SENSITIVITY;
		ProcessCameraRotation(mouseXDelta, mouseYDelta, true);
	}

	void FPSCamera::InvertPitch()
	{
		m_CurrentPitch = -m_CurrentPitch;
		UpdateCameraVectors();
	}

	void FPSCamera::ProcessCameraMovement(glm::vec3 &direction, float deltaTime) {
		float velocity = m_CurrentMovementSpeed * deltaTime;
		m_Position += direction * velocity;
	}

	void FPSCamera::ProcessCameraRotation(double xOffset, double yOffset, GLboolean constrainPitch = true) {
		// Make sure the user isn't interacting with the UI
		if (!Window::GetHideCursor())
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

		UpdateCameraVectors();
	}

	void FPSCamera::ProcessCameraFOV(double offset) {
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

	void FPSCamera::UpdateCameraVectors() {
		m_Front.x = cos(glm::radians(m_CurrentYaw)) * cos(glm::radians(m_CurrentPitch));
		m_Front.y = sin(glm::radians(m_CurrentPitch));
		m_Front.z = sin(glm::radians(m_CurrentYaw)) * cos(glm::radians(m_CurrentPitch));
		m_Front = glm::normalize(m_Front);

		m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
		m_Up = glm::normalize(glm::cross(m_Right, m_Front));
	}
}
