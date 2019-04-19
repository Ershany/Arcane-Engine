#pragma once
#include "pathfinding/NavigationMesh.h"

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

		PathfindingNode* BuildPath(const glm::vec3& spider, PathfindingNode* pathStart, std::unordered_map<PathfindingNode*, PathfindingNode*>& tileToParent);
		//PathfindingNode* DFS(const Spider& spider, const Ant& ant, Assignment1& tileSystem);
		//PathfindingNode* BFS(const Spider& spider, const Ant& ant, Assignment1& tileSystem);
		PathfindingNode* AStar(const glm::vec3& agentPosition, const glm::vec3& destination, std::vector<TrianglePrim>& navmesh, std::unordered_map<glm::vec3*, std::unordered_set<TrianglePrim*>>& pointToTriangle);
		//int ManhattanDistanceHeuristic(const PathfindingNode& node1, const PathfindingNode& node2);
		//int ChebyshevDistanceHeuristic(const PathfindingNode& node1, const PathfindingNode& node2);
		//int EuclideanDistanceHeuristic(const PathfindingNode& node1, const PathfindingNode& node2); // USE EUCLIDEAN SQUARED
		glm::vec3 FindCenterTriangle(const TrianglePrim& triangle);
	};
}