#pragma once

namespace arcane {

	class CubemapCamera {
	public:
		CubemapCamera();
		CubemapCamera(glm::vec3 &centerPosition);

		void switchCameraToFace(int cubeFaceIndex);
		inline void setCenterPosition(glm::vec3 &newPosition) { m_CenterPosition = newPosition; }

		glm::mat4 getViewMatrix();
		glm::mat4 getProjectionMatrix();
	private:
		glm::vec3 m_CenterPosition, m_Front, m_Up;
	};

}
