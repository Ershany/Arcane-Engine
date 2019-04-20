#pragma once

namespace arcane {
	
	class Obstacle {
	public:
		Obstacle(glm::vec3 &min, glm::vec3 &max);

		bool containsPoint(const glm::vec3& position);
	private:
		// AABB Info
		glm::vec3 m_Min; glm::vec3 m_Max;
	};

}
