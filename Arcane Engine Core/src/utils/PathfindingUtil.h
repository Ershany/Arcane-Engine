#pragma once

namespace PathfindingUtil
{
	struct PathfindingNode
	{
		const glm::vec3* position;
		int nodeScore;

		PathfindingNode(const glm::vec3* position, int score) 
		{
			this->position = position;
			nodeScore = score;
		}
	};

	PathfindingNode* BuildPath(const Spider& spider, PathfindingNode* pathStart, std::unordered_map<PathfindingNode*, PathfindingNode*>& tileToParent);
	PathfindingNode* DFS(const Spider& spider, const Ant& ant, Assignment1& tileSystem);
	PathfindingNode* BFS(const Spider& spider, const Ant& ant, Assignment1& tileSystem);
	PathfindingNode* AStar(const Spider& spider, const Ant& ant, Assignment1& tileSystem);
	int ManhattanDistanceHeuristic(const PathfindingNode& node1, const PathfindingNode& node2);
	int ChebyshevDistanceHeuristic(const PathfindingNode& node1, const PathfindingNode& node2);
	int EuclideanDistanceHeuristic(const PathfindingNode& node1, const PathfindingNode& node2);
};