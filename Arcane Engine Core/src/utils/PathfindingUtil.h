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
		PathfindingNode* DFS(const glm::vec3& agentPosition, const glm::vec3& destination, std::vector<TrianglePrim>& navmesh, std::unordered_map<glm::vec3*, std::unordered_set<TrianglePrim*>>& triangleSet);
		PathfindingNode* BFS(const glm::vec3& agentPosition, const glm::vec3& destination, std::vector<TrianglePrim>& navmesh, std::unordered_map<glm::vec3*, std::unordered_set<TrianglePrim*>>& triangleSet);
		PathfindingNode* AStar(const glm::vec3& agentPosition, const glm::vec3& destination, std::vector<TrianglePrim>& navmesh, std::unordered_map<glm::vec3*, std::unordered_set<TrianglePrim*>>& pointToTriangle);
		//int ManhattanDistanceHeuristic(const PathfindingNode& node1, const PathfindingNode& node2);
		//int ChebyshevDistanceHeuristic(const PathfindingNode& node1, const PathfindingNode& node2);
		//int EuclideanDistanceHeuristic(const PathfindingNode& node1, const PathfindingNode& node2); // USE EUCLIDEAN SQUARED
		glm::vec3 FindCenterTriangle(const TrianglePrim& triangle);

		void drawPath(ICamera *camera);
	};
}