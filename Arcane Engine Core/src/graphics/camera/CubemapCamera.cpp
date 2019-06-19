#include "pch.h"
#include "CubemapCamera.h"

namespace arcane {

	CubemapCamera::CubemapCamera() : m_CenterPosition(0.0f, 0.0f, 0.0f) {
		switchCameraToFace(0);
	}

	CubemapCamera::CubemapCamera(glm::vec3 &centerPosiiton) : m_CenterPosition(centerPosiiton) {
		switchCameraToFace(0);
	}

	void CubemapCamera::switchCameraToFace(int cubeFaceIndex) {
		switch (cubeFaceIndex) {
		case 0:
			m_Front.x = 1.0f; m_Front.y = 0.0f; m_Front.z = 0.0f;
			m_Up.x = 0.0f; m_Up.y = -1.0f; m_Up.z = 0.0f;
			break;
		case 1:
			m_Front.x = -1.0f; m_Front.y = 0.0f; m_Front.z = 0.0f;
			m_Up.x = 0.0f; m_Up.y = -1.0f; m_Up.z = 0.0f;
			break;
		case 2:
			m_Front.x = 0.0f; m_Front.y = 1.0f; m_Front.z = 0.0f;
			m_Up.x = 0.0f; m_Up.y = 0.0f; m_Up.z = 1.0f;
			break;
		case 3:
			m_Front.x = 0.0f; m_Front.y = -1.0f; m_Front.z = 0.0f;
			m_Up.x = 0.0f; m_Up.y = 0.0f; m_Up.z = -1.0f;
			break;
		case 4:
			m_Front.x = 0.0f; m_Front.y = 0.0f; m_Front.z = 1.0f;
			m_Up.x = 0.0f; m_Up.y = -1.0f; m_Up.z = 0.0f;
			break;
		case 5:
			m_Front.x = 0.0f; m_Front.y = 0.0f; m_Front.z = -1.0f;
			m_Up.x = 0.0f; m_Up.y = -1.0f; m_Up.z = 0.0f;
			break;
		}
	}

	glm::mat4 CubemapCamera::getViewMatrix() {
		return glm::lookAt(m_CenterPosition, m_CenterPosition + m_Front, m_Up);
	}
	
	glm::mat4 CubemapCamera::getProjectionMatrix() {
		return glm::perspective(glm::radians(90.0f), 1.0f, NEAR_PLANE, FAR_PLANE);
	}

}
