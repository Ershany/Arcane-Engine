#include "arcpch.h"
#include "PerspectiveCamera.h"

#include <Arcane/Graphics/Window.h>
#include <Arcane/Input/InputManager.h>

namespace Arcane
{
	PerspectiveCamera::PerspectiveCamera(glm::vec3 position, glm::vec3 up, float yaw, float pitch, float nearPlane, float farPlane, float fov)
		: CameraController(position, up, yaw, pitch, nearPlane, farPlane), m_CurrentFOV(fov)
	{

	}

	glm::mat4 PerspectiveCamera::GetProjectionMatrix()
	{
		return glm::perspective(glm::radians(m_CurrentFOV), (float)Window::GetRenderResolutionWidth() / (float)Window::GetRenderResolutionHeight(), m_NearPlane, m_FarPlane);
	}

	void PerspectiveCamera::ProcessCameraScroll(float yOffset)
	{
		if (yOffset != 0.0 && m_CurrentFOV >= 1.0 && m_CurrentFOV <= CAMERA_MAX_FOV)
		{
			m_CurrentFOV -= (float)yOffset;
		}
		if (m_CurrentFOV < 1.0f)
		{
			m_CurrentFOV = 1.0f;
		}
		else if (m_CurrentFOV > CAMERA_MAX_FOV)
		{
			m_CurrentFOV = CAMERA_MAX_FOV;
		}
	}
}
