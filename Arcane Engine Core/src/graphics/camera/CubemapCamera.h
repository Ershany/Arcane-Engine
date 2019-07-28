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
		inline virtual const glm::vec3& getFront() const { return m_Front; }
		inline virtual const glm::vec3& getUp() const { return m_Up; }
		inline virtual void setPosition(const glm::vec3 &position) { m_CenterPosition = position; };
	private:
		glm::vec3 m_CenterPosition, m_Front, m_Up;
	};

}
