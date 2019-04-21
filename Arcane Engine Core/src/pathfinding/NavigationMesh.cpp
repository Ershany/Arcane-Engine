#include "pch.h"
#include "NavigationMesh.h"

#include <ui/NavmeshPane.h>
#include <utils/loaders/ShaderLoader.h>
#include <graphics/mesh/common/Cube.h>

namespace arcane
{
	NavigationMesh::NavigationMesh(Terrain* terrain) : terrain(terrain)
	{
		m_CubePositionInstancedVBO = 0;
		m_CubeTransformInstancedVBO = 0;
		m_NavmeshVBO = 0;

		regenerationCallback = [&] { OnRegenerateButtonClick(); };
		NavmeshPane::setRegenerationFunctionPtr(regenerationCallback);

		// Setup debug drawing
		m_GLCache = GLCache::getInstance();
		m_DebugVerticesShader = ShaderLoader::loadShader("src/shaders/simple_instanced.vert", "src/shaders/simple.frag");
		m_DebugNavmeshShader = ShaderLoader::loadShader("src/shaders/simple.vert", "src/shaders/simple.frag");

		// Do not generate on creation 
		//OnRegenerateButtonClick();
	}

	NavigationMesh::~NavigationMesh()
	{}

	bool NavigationMesh::IsPointInTriangle(const glm::vec3& point, const TrianglePrim& triangle)
	{
		return SameSide(point, *(triangle.a), *(triangle.b), *(triangle.c)) && 
			SameSide(point, *(triangle.b), *(triangle.a), *(triangle.c)) && 
			SameSide(point, *(triangle.c), *(triangle.a), *(triangle.b));
	}

	bool NavigationMesh::SameSide(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& a, const glm::vec3& b) {
		glm::vec3 cp1 = glm::cross(b - a, p1 - a);
		glm::vec3 cp2 = glm::cross(b - a, p2 - a);

		if (glm::dot(cp1, cp2) >= 0) return true;
		
		return false;
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

	float NavigationMesh::GetSlopePoints(const glm::vec3& point1, const glm::vec3& point2)
	{
		glm::vec3 difference = point2 - point1; // Getting the difference vectors
		glm::vec3 referenceVec(difference);
		referenceVec.y = 0; 
		float dot = glm::dot(glm::normalize(difference), glm::normalize(referenceVec)); // This will give us cos(theta)

		return dot;
	}

	TrianglePrim* NavigationMesh::GetTriangleFromPoint(const glm::vec3& point, std::vector<TrianglePrim>& triangles)
	{
		for (int i = 0; i < triangles.size(); ++i)
		{
			if (IsPointInTriangle(point, triangles[i]))
				return &triangles[i];
		}

		return nullptr;
	}

	bool NavigationMesh::ObstacleOnPoint(const glm::vec3& point)
	{
		for (int i = 0; i < m_Obstacles.size(); i++) {
			if (m_Obstacles[i].containsPoint(point))
				return true;
		}

		return false;
	}

	std::vector<TrianglePrim> NavigationMesh::TriangulatePoly(std::vector<std::vector<glm::vec3*>>& polygon)
	{
		std::vector<TrianglePrim> triangles;
		
		for (int i = 0; i < polygon.size(); ++i)
		{
			for (int j = 0; j < polygon[i].size(); ++j)
			{
				// Allocate on heap to fix to memory corruption. We could do this much better (ie use the stack and not have dangling pointers), but with time constraints I do not care :)
				TrianglePrim *currentTriangle = new TrianglePrim();
				glm::vec3* current = polygon[i][j];
				glm::vec3* right = j + 1 < polygon[i].size() && (*current - *polygon[i][j + 1]).x == -4.f ? polygon[i][j + 1] : nullptr;
				glm::vec3* up = nullptr;
				glm::vec3* upRight = nullptr;

				if (!right)
					continue;

				if (i + 1 < polygon.size())
				{
					for (int w = 0; w < polygon[i + 1].size(); ++w)
					{
						glm::vec3 difference = *polygon[i + 1][w] - *current;
						if (difference.x == 0.f && difference.z == 4)
							up = polygon[i + 1][w];
						else if (difference.x == 4.f && difference.z == 4)
						{
							upRight = polygon[i + 1][w];
							break;
						}
					}
				}

				if (!up && !upRight)
					continue;
				
				currentTriangle->a = current;
				
				if (up && right)
				{
					currentTriangle->b = up;
					currentTriangle->c = right;

					if (upRight)
					{
						// Allocate on heap to fix to memory corruption. We could do this much better (ie use the stack and not have dangling pointers), but with time constraints I do not care :)
						TrianglePrim *otherTri = new TrianglePrim();
						otherTri->a = up;
						otherTri->b = upRight;
						otherTri->c = right;
						for (int w = 0; w < otherTri->v.size(); ++w)
						{
							m_PointToTriangle[otherTri->v[w]].insert(otherTri);
						}
						triangles.push_back(*otherTri);
					}
				}
				else if (!up && upRight && right)
				{
					currentTriangle->b = upRight;
					currentTriangle->c = right;
				}

				for (int w = 0; w < currentTriangle->v.size(); ++w)
				{
					m_PointToTriangle[currentTriangle->v[w]].insert(currentTriangle);
				}
				triangles.push_back(*currentTriangle);
			}
		}

		return triangles;
	}

	void NavigationMesh::SetupVertexDebugView() {
		std::vector<glm::vec3> verticesToDraw;
		for (int i = 0; i < m_NavigationPolygon.size(); i++) {
			for (int j = 0; j < m_NavigationPolygon[i].size(); j++) {
				verticesToDraw.push_back(*(m_NavigationPolygon[i][j]));
			}
		}
		m_NumCubesToDraw = verticesToDraw.size();

		if (m_NumCubesToDraw != 0) {
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
			glVertexAttribDivisor(1, 1); // Only updates this vertex data per instance (instead of per vertex)

			glBindVertexArray(0);
		}
	}

	void NavigationMesh::SetupNavmeshDebugView() {
		std::vector<glm::vec3> triangleVertices;
		for (int i = 0; i < m_TriangulatedPolygon.size(); i++) {
			triangleVertices.push_back(*(m_TriangulatedPolygon[i].a) + glm::vec3(0.0f, m_NavmeshDebugYOffset, 0.0f));
			triangleVertices.push_back(*(m_TriangulatedPolygon[i].b) + glm::vec3(0.0f, m_NavmeshDebugYOffset, 0.0f));
			triangleVertices.push_back(*(m_TriangulatedPolygon[i].c) + glm::vec3(0.0f, m_NavmeshDebugYOffset, 0.0f));
		}
		m_NumVerticesInNavmesh = triangleVertices.size();

		if (m_NumVerticesInNavmesh != 0) {
			glGenVertexArrays(1, &m_NavmeshVAO);
			glGenBuffers(1, &m_NavmeshVBO);

			glBindVertexArray(m_NavmeshVAO);
			glBindBuffer(GL_ARRAY_BUFFER, m_NavmeshVBO);
			glBufferData(GL_ARRAY_BUFFER, triangleVertices.size() * sizeof(glm::vec3), &triangleVertices[0], GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

			glBindVertexArray(0);
		}
	}

	void NavigationMesh::OnRegenerateButtonClick() {
		std::cout << "Regenerating Nav Mesh" << std::endl;

		// Clean up old buffers on the GPU
		if (m_CubePositionInstancedVBO != 0 && m_CubeTransformInstancedVBO != 0) {
			glDeleteVertexArrays(1, &m_CubeInstancedVAO);
			glDeleteBuffers(1, &m_CubePositionInstancedVBO);
			glDeleteBuffers(1, &m_CubeTransformInstancedVBO);
		}
		if (m_NavmeshVBO != 0) {
			glDeleteVertexArrays(1, &m_NavmeshVAO);
			glDeleteBuffers(1, &m_NavmeshVBO);
		}
		
		// Regenerate
		GenerateNavigationMesh();

		// Post generation setup (generate new buffers on the GPU)
		SetupVertexDebugView();
		SetupNavmeshDebugView();

		NavmeshPane::setShowNavmesh(true);
	}

	void NavigationMesh::DrawMesh(ICamera* camera) {
		// Check with the UI layer, if it should even be drawn
		if (!NavmeshPane::getShowNavmesh())
			return;

		m_GLCache->switchShader(m_DebugNavmeshShader);
		m_GLCache->setFaceCull(false);
		m_GLCache->setBlend(true);
		m_GLCache->setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Setup colour and also view and projection matrices
		m_DebugNavmeshShader->setUniformMat4("view", camera->getViewMatrix());
		m_DebugNavmeshShader->setUniformMat4("projection", camera->getProjectionMatrix());
		m_DebugNavmeshShader->setUniform3f("colour", glm::vec3(0.0f, 1.0f, 1.0f));

		// Draw our navmesh
		if (m_NumVerticesInNavmesh != 0) {
			glBindVertexArray(m_NavmeshVAO);
			glDrawArrays(GL_TRIANGLES, 0, m_NumVerticesInNavmesh);
		}
		m_GLCache->setFaceCull(true);
		m_GLCache->setBlend(false);
	}

	void NavigationMesh::DrawVertices(ICamera* camera) {
		// Check with the UI layer, if it should even be drawn
		if (!NavmeshPane::getShowNavmeshVertices())
			return;

		m_GLCache->switchShader(m_DebugVerticesShader);

		// Setup colour and also view and projection matrices
		m_DebugVerticesShader->setUniformMat4("view", camera->getViewMatrix());
		m_DebugVerticesShader->setUniformMat4("projection", camera->getProjectionMatrix());
		m_DebugVerticesShader->setUniform3f("colour", glm::vec3(1.0f, 0.0f, 0.0f));
		m_DebugVerticesShader->setUniform1f("yOffset", m_NavmeshDebugYOffset);
		

		// Draw our cube
		if (m_NumCubesToDraw != 0) {
			glBindVertexArray(m_CubeInstancedVAO);
			glDrawArraysInstanced(GL_TRIANGLES, 0, m_Cube.GetPositions().size(), m_NumCubesToDraw);
		}
	}

	void NavigationMesh::GenerateNavigationMesh()
	{
		// Clear the old nav mesh stuff
		m_NavigationPolygon.clear();
		m_TriangulatedPolygon.clear();
		m_PointToTriangle.clear();

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
					continue;

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
						if(terrainPoints[col + (row * columnCount)].y < NavmeshPane::getHeightRestriction())
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
	}

	void NavigationMesh::AddObstacle(const Obstacle& obstacle) {
		m_Obstacles.push_back(obstacle);
	}
}
