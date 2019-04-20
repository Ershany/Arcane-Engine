#include "pch.h"
#include "PathfindingUtil.h"

namespace arcane
{
	namespace PathfindingUtil
	{
		// Comparator lambda for priority queue
		auto comparatorLambda = [](PathfindingNode * node1, PathfindingNode * node2) -> bool {
			return node1->nodeScore > node2->nodeScore;
		};

		//PathfindingNode* BFS(const glm::vec3& agentPosition, const glm::vec3& destination, Graph& tileSystem)
		//{
		//	std::unordered_map<PathfindingNode*, PathfindingNode*> tileToParent; // Tile to parent
		//	std::deque<PathfindingNode*> currentTiles; // Current tiles
		//	std::unordered_set<const PathfindingNode*> closedList; // CLosed list of tiles

		//	PathfindingNode agentNode = PathfindingNode(&agentPosition, 0);
		//	currentTiles.push_back(&agentNode); // Add the first tile
		//	closedList.insert(&agentNode); // Add to query set
		//	while (!currentTiles.empty())
		//	{
		//		PathfindingNode* currentNode = currentTiles.front();
		//		if (*currentNode->position == destination)
		//			return BuildPath(&agentNode, currentNode, tileToParent);
		//	
		//		currentTiles.pop_front();
		//		//int currentX = currentTile->position.first;
		//		//int currentY = currentTile->position.second;
		//		
		//		//for (int i = 0; i < spider.spiderMoveSpread.size(); i++)
		//		//{
		//		//	int directionX = spider.spiderMoveSpread[i].first;
		//		//	int directionY = spider.spiderMoveSpread[i].second;

		//			PathfindingNode* neighborNode = nullptr;
		//			if (!neighborNode || closedList.count(neighborNode) != 0)
		//				continue;
		//
		//			tileToParent[neighborNode] = currentNode; // Establish parent child relationship
		//			currentTiles.push_back(neighborNode); // Add tile to the queue
		//			closedList.insert(neighborNode); // Add tile to retrieval set 
		//		//}
		//	}

		//	return nullptr;
		//}

		//PathfindingNode* DFS(const glm::vec3& agentPosition, const glm::vec3& destination, Graph& tileSystem)
		//{
		//	std::unordered_map<PathfindingNode*, PathfindingNode*> tileToParent; // Tile to parent
		//	std::deque<PathfindingNode*> currentTiles; // Current tiles
		//	std::unordered_set<const PathfindingNode*> closedList; // Closed list

		//	PathfindingNode agentNode = PathfindingNode(&agentPosition, 0);
		//	currentTiles.push_front(&agentNode); // Add the first tile
		//	closedList.insert(&agentNode); // This node should never be visited again
		//	while (!currentTiles.empty())
		//	{
		//		PathfindingNode* currentNode = currentTiles.front();
		//		if (*currentNode->position == destination)
		//			return BuildPath(&agentNode, currentNode, tileToParent);

		//		currentTiles.pop_front();
		//		//int currentX = currentTile->position.first;
		//		//int currentY = currentTile->position.second;

		//		//for (int i = 0; i < spider.spiderMoveSpread.size(); i++)
		//		//{
		//		//	int directionX = spider.spiderMoveSpread[i].first;
		//		//	int directionY = spider.spiderMoveSpread[i].second;
		//			PathfindingNode* neighborNode = nullptr;
		//			if (!neighborNode || closedList.count(neighborNode) != 0)
		//				continue;

		//			tileToParent[neighborNode] = currentNode; // Establish parent child relationship
		//			currentTiles.push_front(neighborNode); // Add tile to the queue
		//			closedList.insert(neighborNode); // Tiles that we should never visit or add to out deque again
		//		//}
		//	}

		//	return nullptr;
		//}

		PathfindingNode* AStar(const glm::vec3& agentPosition, const glm::vec3& destination, std::vector<TrianglePrim>& navmesh, std::unordered_map<glm::vec3*, std::unordered_set<TrianglePrim*>>& triangleSet)
		{
			TrianglePrim* startingTri = NavigationMesh::GetTriangleFromPoint(agentPosition, navmesh);
			TrianglePrim* destinationTri = NavigationMesh::GetTriangleFromPoint(destination, navmesh);
			if (!startingTri || !destinationTri)
				return nullptr;

			std::priority_queue<PathfindingNode*, std::vector<PathfindingNode*>, decltype(comparatorLambda)> tileQueue(comparatorLambda); // The tiles that we have queued to be searched
			std::unordered_map<TrianglePrim*, float> gCost; // The gCost of a certain tile (distance from start point to the tile)
			std::unordered_map<PathfindingNode*, PathfindingNode*> tileToParent; // Tile to parent relationship for path regeneration
			PathfindingNode* agentNode = new PathfindingNode(startingTri, 0);
			
			// DEALLOCATION???? 
			tileQueue.push(agentNode);
			gCost[startingTri] = 0; 
			while (!tileQueue.empty())
			{
				PathfindingNode* currentNode = tileQueue.top();
				if (currentNode->triangle == destinationTri)
					return BuildPath(agentNode, currentNode, tileToParent);

				tileQueue.pop();
				int gCostCurrent = gCost[currentNode->triangle];
				glm::vec3 currentTriCenter = FindCenterTriangle(*currentNode->triangle);

				// Get the triangles that are neighbors of the 3 points of the triangle
				for (int i = 0; i < currentNode->triangle->v.size(); ++i)
				{
					std::unordered_set<TrianglePrim*>& neighborTris = triangleSet[currentNode->triangle->v[i]];
					for (auto iter = neighborTris.begin(); iter != neighborTris.end(); ++iter)
					{
						TrianglePrim* neighborTri = *iter;
						if (gCost.count(neighborTri) != 0)
							continue;

						glm::vec3 centerNeighbor = FindCenterTriangle(*neighborTri);
						float gCostNext = gCostCurrent + glm::length2(currentTriCenter - centerNeighbor);
						float hCostNext = glm::length2(centerNeighbor - destination);
						PathfindingNode* nextNode = new PathfindingNode(neighborTri, gCostNext + hCostNext);

						gCost[neighborTri] = gCostNext;
						tileQueue.push(nextNode); // Add the new unexplored tile
						tileToParent[nextNode] = currentNode; // tile parent relationship
					}
				}
			}

			return nullptr;
		}

		PathfindingNode* BuildPath(const PathfindingNode* agentNode, PathfindingNode* pathStart, std::unordered_map<PathfindingNode*, PathfindingNode*> & tileToParent)
		{
			PathfindingNode* currentNode = pathStart; // In this situation it is the destination node
			PathfindingNode* previousNode = nullptr;
			std::vector<PathfindingNode*> path;

			while (currentNode != agentNode)
			{
				path.push_back(currentNode);
				previousNode = currentNode;

				// Get the current Node's parent if it is not there then we will never find the node we want so break
				if (tileToParent.find(currentNode) != tileToParent.end())
					currentNode = tileToParent[currentNode];
				else
					return nullptr; // something wrong happened if we let this go it will run infinitely
			}

			return previousNode; // This is the next position that the unit should be moving to
		}

		//int ManhattanDistanceHeuristic(const PathfindingNode & node1, const PathfindingNode & node2)
		//{
		//	// Manhattan distance is used for grid distance when we have 4 way movement and no diagonal movement
		//	int absDx = abs(node1.position->x - node2.position->x);
		//	int absDy = abs(node1.position->y - node2.position->y);
		//	return absDx + absDy;
		//}

		//int ChebyshevDistanceHeuristic(const PathfindingNode & node1, const PathfindingNode & node2)
		//{
		//	// Used when we have 8 way movement
		//	int absDx = std::abs(node1.position->x - node2.position->x);
		//	int absDy = std::abs(node1.position->y - node2.position->y);
		//	return std::max(absDx, absDy);
		//}

		//// This is probably what we need to do (OBVIOUSLY SQUARED)
		//int EuclideanDistanceHeuristic(const PathfindingNode & node1, const PathfindingNode & node2)
		//{
		//	// Using the euclidean distance squared allows us to not use sqrt which is an expensive operation
		//	int differenceX = node1.position->x - node2.position->x;
		//	int differenceY = node1.position->y - node2.position->y;;
		//	return sqrt(differenceX * differenceX + differenceY * differenceY);
		//}

		glm::vec3 FindCenterTriangle(const TrianglePrim& triangle)
		{
			glm::vec3 center;
			for (auto iter = triangle.v.begin(); iter != triangle.v.end(); ++iter)
			{
				center += *(*iter);
			}

			return center * 0.33333333f; // Find the average of the 3 vertices
		}
	}
}