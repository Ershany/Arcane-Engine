#include "pch.h"
#include "NavigationMesh.h"

#include <ui/NavmeshPane.h>
#include <utils/loaders/ShaderLoader.h>
#include <graphics/mesh/common/Cube.h>

namespace arcane
{
	NavigationMesh::NavigationMesh(Terrain* terrain) : terrain(terrain)
	{
		SetSlopeMesh(NavmeshPane::getNavmeshSlope()); // Checks the angle given for the slope and calculates the cosine of that angle

		regenerationCallback = [&] { OnRegenerateButtonClick(); };
		NavmeshPane::setRegenerationFunctionPtr(regenerationCallback);

		// Setup debug drawing
		m_GLCache = GLCache::getInstance();
		m_DebugShader = ShaderLoader::loadShader("src/shaders/simple_instanced.vert", "src/shaders/simple_instanced.frag");

		OnRegenerateButtonClick();
	}

	NavigationMesh::~NavigationMesh()
	{}

	bool NavigationMesh::IsPointOnTriangle(const glm::vec3& point, const TrianglePrim& triangle)
	{
		// Check if point is on the same plane as the triangle
		if (!IsPointOnPlane(point, *triangle.a, *triangle.b, *triangle.c))
			return false;

		// Check if point is inside the triangle or outside within the plane they share
		return SameSideTriangle(point, *triangle.a, *triangle.b, *triangle.c) ||
			SameSideTriangle(point, *triangle.b, *triangle.a, *triangle.c) || 
			SameSideTriangle(point, *triangle.c, *triangle.a, *triangle.b);
	}

	bool NavigationMesh::IsPointOnPlane(const glm::vec3& point, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
	{
		glm::vec3 p1p2Crossp1p3 = glm::cross(p2 - p1, p3 - p1); // This is the normal of the plane formed by the 3 points of the plane
		float r = glm::dot(p1, p1p2Crossp1p3); // find the last unknown on the plane equation
		
		return glm::dot(point, p1p2Crossp1p3) + r == 0; // Might need to round this value a little bit
	}

	bool NavigationMesh::SameSideTriangle(const glm::vec3& point, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
	{
		glm::vec3 abCrossap = glm::cross(b - a, point - a);
		glm::vec3 abCrossac = glm::cross(b - a, c - a);

		return glm::dot(abCrossap, abCrossac) >= 0;
	}

	void NavigationMesh::SetSlopeMesh(float angle) 
	{
		this->m_slopeAngle = std::cos(angle);
	}
	
	float NavigationMesh::GetSlopePoints(const glm::vec3& point1, const glm::vec3& point2)
	{
		glm::vec3 difference = point2 - point1; // Getting the difference vectors
		glm::vec3 referenceVec(difference);
		referenceVec.y = 0; 
		float dot = glm::dot(glm::normalize(difference), glm::normalize(referenceVec)); // This will give us cos(theta)

		return dot;
	}

	bool NavigationMesh::ObstacleOnPoint(const glm::vec3& point)
	{
		return false;
	}

	bool NavigationMesh::ExistsPathToPoint(const glm::vec3& point, const std::vector<glm::vec3>& terrainPoints)
	{
		// Check if we can go from this point to any of the others around it
		unsigned int columnNumber = terrain->GetVertexCount();
		return false;
	}

	std::vector<TrianglePrim> NavigationMesh::TriangulatePoly(std::vector<std::vector<glm::vec3*>>& polygon)
	{
		std::vector<TrianglePrim> triangles;
		
		for (int i = 0; i < polygon.size(); ++i)
		{
			for (int j = 0; j < polygon[i].size(); ++j)
			{
				TrianglePrim currentTriangle;
				
				// DO bounds check
				glm::vec3* current = polygon[i][j];
				glm::vec3* up = polygon[i + 1][j] && glm::length2(*current - *polygon[i + 1][j]) == 16.f && polygon[i + 1][j] ? polygon[i + 1][j] : nullptr;
				glm::vec3* right = polygon[i][j + 1] && glm::length2(*current - *polygon[i][j + 1]) == 16.f ? polygon[i][j + 1] : nullptr;
				glm::vec3* upRight = polygon[i + 1][j + 1] && glm::length2(*current - *polygon[i + 1][j + 1]) == 32.f ? polygon[i][j + 1] : nullptr; // diagonal

				if (!up && !upRight)
					continue;
				
				currentTriangle.a = current;
				
				if (up && right)
				{
					currentTriangle.b = up;
					currentTriangle.c = right;

					if (upRight)
					{
						TrianglePrim otherTri;
						otherTri.a = up;
						otherTri.b = upRight;
						otherTri.c = right;
						triangles.push_back(otherTri);
					}
				}
				else if (!up && upRight && right)
				{
					currentTriangle.b = upRight;
					currentTriangle.c = right;
				}

				triangles.push_back(currentTriangle);
			}
		}

		return triangles;
	}

	void NavigationMesh::OnRegenerateButtonClick() {
		std::cout << "Regenerating Nav Mesh" << std::endl;

		// TODO: Clean up old instance buffers on the GPU

		// Setup
		SetSlopeMesh(NavmeshPane::getNavmeshSlope());
		
		// Regenerate
		GenerateNavigationMesh();

		// Post generation setup
		std::vector<glm::vec3> verticesToDraw;
		for (int i = 0; i < m_NavigationPolygon.size(); i++) {
			for (int j = 0; j < m_NavigationPolygon[i].size(); j++) {
				verticesToDraw.push_back(*(m_NavigationPolygon[i][j]));
			}
		}
		m_NumCubesToDraw = verticesToDraw.size();

		glGenVertexArrays(1, &m_CubeInstancedVAO);
		glGenBuffers(1, &m_CubePositionInstancedVBO);
		glGenBuffers(1, &m_CubeTransformInstancedVBO);

		glBindBuffer(GL_ARRAY_BUFFER, m_CubePositionInstancedVBO);
		glBufferData(GL_ARRAY_BUFFER, m_Cube.GetPositions().size() * sizeof(glm::vec3), &m_Cube.GetPositions()[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, m_CubeTransformInstancedVBO);
		glBufferData(GL_ARRAY_BUFFER, m_NumCubesToDraw * sizeof(glm::vec3), &verticesToDraw[0], GL_STATIC_DRAW);

		glBindVertexArray(m_CubeInstancedVAO);

		// Cube model space position
		glBindBuffer(GL_ARRAY_BUFFER, m_CubePositionInstancedVBO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		// Transform position of each instance
		glBindBuffer(GL_ARRAY_BUFFER, m_CubeTransformInstancedVBO);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glVertexAttribDivisor(1, 1);

		glBindVertexArray(0);
	}

	void NavigationMesh::DrawMesh(ICamera* camera) {
	
	}

	void NavigationMesh::DrawVertices(ICamera* camera) {
		m_GLCache->switchShader(m_DebugShader->getShaderID());

		// Setup model, view, and projection matrix
		m_DebugShader->setUniformMat4("view", camera->getViewMatrix());
		m_DebugShader->setUniformMat4("projection", camera->getProjectionMatrix());

		// Draw our cube
		glBindVertexArray(m_CubeInstancedVAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, m_Cube.GetPositions().size(), m_NumCubesToDraw);
	}

	void NavigationMesh::GenerateNavigationMesh()
	{
		// Clear the old nav mesh stuff
		m_NavigationPolygon.clear();
		m_TriangulatedPolygon.clear();

		// Filter out the points that we cannot reach
		std::vector<glm::vec3>& terrainPoints = terrain->GetPoints();
		int rowNumber = 0;
		int columnCount = terrain->GetVertexCount();

		for (int row = 0; row < terrain->GetVertexCount(); ++row)
		{
			m_NavigationPolygon.emplace_back();

			for (int col = 0; col < terrain->GetVertexCount(); ++col)
			{
				if (ObstacleOnPoint(terrainPoints[col + (row * columnCount)]))
					continue; // No obstacles being checked atm

				// Check if any of the points around it can navigate to the point we are currently on 
				bool navigable = false;
				for (int j = 0; j < 2; ++j)
				{
					for (int k = -1; k < 2; ++k)
					{
						int c = col + k;
						int r = row + j;

						if (c < 0 || c >= columnCount || r < 0 || r >= columnCount)
							continue;

						int index = r * columnCount + c;
						glm::vec3* neighborPoint = &terrainPoints[index];
						if (*neighborPoint == terrainPoints[col + (row * columnCount)])
							continue;

						// Check the slope of the 2 points
						float slope = GetSlopePoints(terrainPoints[col + (row * columnCount)], *neighborPoint);
						//if (slope > m_slopeAngle)
						if(terrainPoints[col + (row * columnCount)].y < 114.f)
						{
							m_NavigationPolygon[m_NavigationPolygon.size() - 1].push_back(&terrainPoints[col + (row * columnCount)]);
							navigable = true;
							break;
						}
					}

					// If we found something 
					if (navigable)
						break;
				}
			}
		}

		// Triangulate these new points to form a new mesh
		m_TriangulatedPolygon = TriangulatePoly(m_NavigationPolygon);

		// Optimize this mesh for pathfinding by attempting to decrease number of triangles
	}
}
