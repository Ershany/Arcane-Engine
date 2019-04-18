#pragma once

#define DEFAULT_DEGREE 30
#define COS_30 0.866666f

#include "terrain/Terrain.h"
#include "scene/RenderableModel.h"

namespace arcane
{
	struct TrianglePrim
	{
		union
		{
			std::array<glm::vec3*, 3> v;
			glm::vec3* a;
			glm::vec3* b;
			glm::vec3* c;
		};
	};

	struct QuadPrim
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
		Terrain* terrain; // Reference terrain that we want to build the navigation mesh on 
		float slopeAngle; // A parameter that will hold the slope that we will accept for the navigation mesh
		glm::vec3 sideVector; // Store the side vector of the terrain mesh
		std::function<void()> regenerationCallback;

		NavigationMesh(Terrain* terrain);
		~NavigationMesh();

		static bool IsPointOnTriangle(const glm::vec3& point, const TrianglePrim& triangle);
		static bool IsPointOnPlane(const glm::vec3& point, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);
		static bool SameSideTriangle(const glm::vec3& point, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);

		// Set the terrain that this navMesh will use (Might want to regenerate mesh for new Terrain)
		inline void SetTerrain(Terrain* terrain) { this->terrain = terrain; }
		// Generate the navigation mesh from the current terrain
		void GenerateNavigationMesh();
		// Used as the callback for regeneration of the navigation mesh on the UI button click
		void OnRegenerateButtonClick();
		// Checks if there is an obstacle at this location
		bool ObstacleOnPoint(const glm::vec3& point);
		// Checks if this point is navigable
		bool ExistsPathToPoint(const glm::vec3& point, const std::vector<glm::vec3>& terrainPoints);
		// Triangulate the generated polygon
		std::vector<TrianglePrim> TriangulatePoly(std::vector<std::vector<glm::vec3*>>& polygon);
		// Draw the navigation mesh
		void DrawMesh(const std::vector<TrianglePrim>& triangles);
		// Draw the vertices of the navigation mesh
		void DrawVertices(const std::vector<std::vector<glm::vec3*>>& vertices);
		// Get the slope of the 2 pts its mostly the angle between these 2 pts and a reference vector(probably side vector)
		float GetSlopePoints(const glm::vec3& point1, const glm::vec3& point2);
		// Set the Slope that the nav mesh will use for generation
		void SetSlopeMesh(int angle);
	};
}
