#include "arcpch.h"
#include "OrthographicCamera.h"

#include <Arcane/Graphics/Window.h>
#include <Arcane/Input/InputManager.h>

namespace Arcane
{
	OrthographicCamera::OrthographicCamera(glm::vec3 position, glm::vec3 up, float yaw, float pitch, float nearPlane, float farPlane, float left, float right, float bottom, float top)
		: CameraController(position, up, yaw, pitch, nearPlane, farPlane), m_Left(left), m_Right(right), m_Bottom(bottom), m_Top(top)
	{

	}

	glm::mat4 OrthographicCamera::GetProjectionMatrix()
	{
		return glm::ortho(m_Left, m_Right, m_Bottom, m_Top, m_NearPlane, m_FarPlane);
	}

	void OrthographicCamera::ProcessCameraScroll(float yOffset)
	{
		
	}
}
