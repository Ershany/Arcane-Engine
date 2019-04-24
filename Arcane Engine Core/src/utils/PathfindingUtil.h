#pragma once
#include "pathfinding/NavigationMesh.h"
#include <graphics/Shader.h>
#include <utils/loaders/ShaderLoader.h>

namespace arcane
{
	struct PathfindingNode
	{
		TrianglePrim* triangle;
		float nodeScore;

		PathfindingNode(TrianglePrim* tri, int score) : triangle(tri), nodeScore(score)
		{}
	};

	class PathfindingUtil
	{
	public:
		static PathfindingNode* BuildPath(const PathfindingNode* agentNode, PathfindingNode* pathStart, std::unordered_map<PathfindingNode*, PathfindingNode*>& tileToParent);
		static PathfindingNode* DFS(const glm::vec3& agentPosition, const glm::vec3& destination, std::vector<TrianglePrim>& navmesh, std::unordered_map<glm::vec3*, std::unordered_set<TrianglePrim*>>& pointToTriangle);
		static PathfindingNode* BFS(const glm::vec3& agentPosition, const glm::vec3& destination, std::vector<TrianglePrim>& navmesh, std::unordered_map<glm::vec3*, std::unordered_set<TrianglePrim*>>& pointToTriangle);
		static PathfindingNode* AStar(const glm::vec3& agentPosition, const glm::vec3& destination, std::vector<TrianglePrim>& navmesh, std::unordered_map<glm::vec3*, std::unordered_set<TrianglePrim*>>& pointToTriangle);
		static PathfindingNode* BestFirstSearch(const glm::vec3& agentPosition, const glm::vec3& destination, std::vector<TrianglePrim>& navmesh, std::unordered_map<glm::vec3*, std::unordered_set<TrianglePrim*>>& triangleSet);
		static float ManhattanDistanceHeuristic(const glm::vec3& node1, const glm::vec3& node2);
		static float ChebyshevDistanceHeuristic(const glm::vec3& node1, const glm::vec3& node2);
		static float EuclideanDistanceHeuristic(const glm::vec3& node1, const glm::vec3& node2); // USE EUCLIDEAN SQUARED
		static glm::vec3 FindCenterTriangle(const TrianglePrim& triangle);

		static void draw(ICamera *camera);
	public:
		static std::vector<PathfindingNode*> s_Path;
		static std::vector<TrianglePrim*> s_TrianglesSearched;
	};
}