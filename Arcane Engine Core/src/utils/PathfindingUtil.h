#pragma once
#include "pathfinding/NavigationMesh.h"
#include <graphics/Shader.h>
#include <utils/loaders/ShaderLoader.h>

namespace arcane
{
	namespace PathfindingUtil
	{
		struct PathfindingNode
		{
			TrianglePrim* triangle;
			float nodeScore;

			PathfindingNode(TrianglePrim* tri, int score) : triangle(tri), nodeScore(score)
			{}
		};

		PathfindingNode* BuildPath(const PathfindingNode* agentNode, PathfindingNode* pathStart, std::unordered_map<PathfindingNode*, PathfindingNode*>& tileToParent);
		PathfindingNode* DFS(const glm::vec3& agentPosition, const glm::vec3& destination, std::vector<TrianglePrim>& navmesh, std::unordered_map<glm::vec3*, std::unordered_set<TrianglePrim*>>& pointToTriangle);
		PathfindingNode* BFS(const glm::vec3& agentPosition, const glm::vec3& destination, std::vector<TrianglePrim>& navmesh, std::unordered_map<glm::vec3*, std::unordered_set<TrianglePrim*>>& pointToTriangle);
		PathfindingNode* AStar(const glm::vec3& agentPosition, const glm::vec3& destination, std::vector<TrianglePrim>& navmesh, std::unordered_map<glm::vec3*, std::unordered_set<TrianglePrim*>>& pointToTriangle);
		float ManhattanDistanceHeuristic(const glm::vec3& node1, const glm::vec3& node2);
		float ChebyshevDistanceHeuristic(const glm::vec3& node1, const glm::vec3& node2);
		float EuclideanDistanceHeuristic(const glm::vec3& node1, const glm::vec3& node2); // USE EUCLIDEAN SQUARED
		glm::vec3 FindCenterTriangle(const TrianglePrim& triangle);

		void drawPath(ICamera *camera);
	};
}