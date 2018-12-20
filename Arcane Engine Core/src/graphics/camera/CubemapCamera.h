#pragma once

#include <graphics/camera/ICamera.h>

namespace arcane {

	class CubemapCamera : public ICamera {
	public:
		CubemapCamera();
		CubemapCamera(glm::vec3 &centerPosition);

		void switchCameraToFace(int cubeFaceIndex);
		inline void setCenterPosition(glm::vec3 &newPosition) { m_CenterPosition = newPosition; }

		virtual glm::mat4 getViewMatrix();
		virtual glm::mat4 getProjectionMatrix();
		inline virtual const glm::vec3& getPosition() const override { return m_CenterPosition; }
	private:
		glm::vec3 m_CenterPosition, m_Front, m_Up;
	};

}
