#include "pch.h"
#include "Sphere.h"

namespace arcane {

	Sphere::Sphere(int xSegments, int ySegments) {
		for (int y = 0; y <= ySegments; ++y)
		{
			for (int x = 0; x <= xSegments; ++x)
			{
				float xSegment = (float)x / (float)xSegments;
				float ySegment = (float)y / (float)ySegments;
				float xPos = std::cos(xSegment * glm::pi<float>() * 2.0f) * std::sin(ySegment * glm::pi<float>());
				float yPos = std::cos(ySegment * glm::pi<float>());
				float zPos = std::sin(xSegment * glm::pi<float>() * 2.0f) * std::sin(ySegment * glm::pi<float>());

				m_Positions.push_back(glm::vec3(xPos, yPos, zPos));
				m_UVs.push_back(glm::vec2(xSegment, ySegment));
				m_Normals.push_back(glm::vec3(xPos, yPos, zPos));
			}
		}

		for (int y = 0; y < ySegments; ++y)
		{
			for (int x = 0; x < xSegments; ++x)
			{
				m_Indices.push_back((y + 1) * (xSegments + 1) + x);
				m_Indices.push_back(y       * (xSegments + 1) + x);
				m_Indices.push_back(y       * (xSegments + 1) + x + 1);

				m_Indices.push_back((y + 1) * (xSegments + 1) + x);
				m_Indices.push_back(y       * (xSegments + 1) + x + 1);
				m_Indices.push_back((y + 1) * (xSegments + 1) + x + 1);
			}
		}

		LoadData();
	}

}
