#include "pch.h"
#include "Obstacle.h"

namespace arcane {

	Obstacle::Obstacle(glm::vec3& min, glm::vec3& max) : m_Min(min), m_Max(max) {

	}

	bool Obstacle::containsPoint(const glm::vec3& position) {
		if (position.x >= m_Min.x && position.x <= m_Max.x && position.y >= m_Min.y && position.y <= m_Max.y && position.z >= m_Min.z && position.z <= m_Max.z)
			return true;

		return false;
	}

}
