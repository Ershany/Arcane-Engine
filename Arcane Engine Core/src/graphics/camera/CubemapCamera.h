#pragma once

namespace arcane {

	class CubemapCamera {
	public:
		CubemapCamera(glm::vec3 &centerPosition);

		void switchCameraToFace(int cubeFaceIndex);

		glm::mat4 getViewMatrix();
		glm::mat4 getProjectionMatrix();
	private:
		glm::vec3 m_CenterPosition, m_Front, m_Up;
	};

}
