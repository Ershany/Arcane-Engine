#include "arcpch.h"
#include "OrthographicCamera.h"

#include <Arcane/Graphics/Window.h>
#include <Arcane/Input/InputManager.h>

namespace Arcane
{
	OrthographicCamera::OrthographicCamera(glm::vec3 position, glm::vec3 up, float yaw, float pitch, float nearPlane, float farPlane, float left, float right, float bottom, float top, float aspectRatio)
		: CameraController(position, up, yaw, pitch, nearPlane, farPlane), m_Left(left), m_Right(right), m_Bottom(bottom), m_Top(top), m_AspectRatio(aspectRatio), m_Zoom(0.0f)
	{

	}

	glm::mat4 OrthographicCamera::GetProjectionMatrix()
	{
		return glm::ortho((m_Left - m_Zoom) * m_AspectRatio, (m_Right + m_Zoom) * m_AspectRatio, m_Bottom - m_Zoom, m_Top + m_Zoom, m_NearPlane, m_FarPlane);
	}

	void OrthographicCamera::ProcessCameraScroll(float yOffset)
	{
		m_Zoom -= yOffset * CAMERA_SPEED * 0.05f;
		m_Zoom = std::clamp<float>(m_Zoom, -10.0f, 10.0f);
	}
}
