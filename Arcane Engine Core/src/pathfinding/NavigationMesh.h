#pragma once

namespace arcane
{
	struct Triangle
	{
		union
		{
			std::array<glm::vec3*, 3> v;
			glm::vec3* a;
			glm::vec3* b;
			glm::vec3* c;
		};
	};

	struct Quad
	{
		union
		{
			std::array<glm::vec3*, 4> v;
			glm::vec3* a;
			glm::vec3* b;
			glm::vec3* c;
			glm::vec3* d;
		};
	};

	class NavigationMesh
	{
	public:
		static bool IsPointOnTriangle(const glm::vec3& point, const Triangle& triangle);
		static bool IsPointOnPlane(const glm::vec3& point, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);
		static bool SameSideTriangle(const glm::vec3& point, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);

		NavigationMesh();
		~NavigationMesh();
	};
}
