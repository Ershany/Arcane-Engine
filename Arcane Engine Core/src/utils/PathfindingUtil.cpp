#include "pch.h"
#include "PathfindingUtil.h"

namespace PathfindingUtil
{
	// Comparator lambda for priority queue
	auto comparatorLambda = [] (PathfindingNode* node1, PathfindingNode* node2) -> bool {
		return node1->nodeScore > node2->nodeScore;
	};

	PathfindingNode* BFS(const glm::vec3& agentPosition, const glm::vec3& destination, Graph& tileSystem)
	{
		std::unordered_map<PathfindingNode*, PathfindingNode*> tileToParent; // Tile to parent
		std::deque<PathfindingNode*> currentTiles; // Current tiles
		std::unordered_set<const PathfindingNode*> closedList; // CLosed list of tiles

		PathfindingNode agentNode = PathfindingNode(&agentPosition, 0);
		currentTiles.push_back(&agentNode); // Add the first tile
		closedList.insert(&agentNode); // Add to query set
		while (!currentTiles.empty())
		{
			PathfindingNode* currentNode = currentTiles.front();
			if (*currentNode->position == destination)
				return BuildPath(&agentNode, currentNode, tileToParent);
		
			currentTiles.pop_front();
			//int currentX = currentTile->position.first;
			//int currentY = currentTile->position.second;
			
			//for (int i = 0; i < spider.spiderMoveSpread.size(); i++)
			//{
			//	int directionX = spider.spiderMoveSpread[i].first;
			//	int directionY = spider.spiderMoveSpread[i].second;

				PathfindingNode* neighborNode = nullptr;
				if (!neighborNode || closedList.count(neighborNode) != 0)
					continue;
	
				tileToParent[neighborNode] = currentNode; // Establish parent child relationship
				currentTiles.push_back(neighborNode); // Add tile to the queue
				closedList.insert(neighborNode); // Add tile to retrieval set 
			//}
		}

		return nullptr;
	}

	PathfindingNode* DFS(const glm::vec3& agentPosition, const glm::vec3& destination, Graph& tileSystem)
	{
		std::unordered_map<PathfindingNode*, PathfindingNode*> tileToParent; // Tile to parent
		std::deque<PathfindingNode*> currentTiles; // Current tiles
		std::unordered_set<const PathfindingNode*> closedList; // Closed list

		PathfindingNode agentNode = PathfindingNode(&agentPosition, 0);
		currentTiles.push_front(&agentNode); // Add the first tile
		closedList.insert(&agentNode); // This node should never be visited again
		while (!currentTiles.empty())
		{
			PathfindingNode* currentNode = currentTiles.front();
			if (*currentNode->position == destination)
				return BuildPath(&agentNode, currentNode, tileToParent);

			currentTiles.pop_front();
			//int currentX = currentTile->position.first;
			//int currentY = currentTile->position.second;

			//for (int i = 0; i < spider.spiderMoveSpread.size(); i++)
			//{
			//	int directionX = spider.spiderMoveSpread[i].first;
			//	int directionY = spider.spiderMoveSpread[i].second;
				PathfindingNode* neighborNode = nullptr;
				if (!neighborNode || closedList.count(neighborNode) != 0)
					continue;

				tileToParent[neighborNode] = currentNode; // Establish parent child relationship
				currentTiles.push_front(neighborNode); // Add tile to the queue
				closedList.insert(neighborNode); // Tiles that we should never visit or add to out deque again
			//}
		}

		return nullptr;
	}

	PathfindingNode* AStar(const glm::vec3& agentPosition, const glm::vec3& destination, Graph& tileSystem)
	{
		std::priority_queue<PathfindingNode*, std::vector<PathfindingNode*>, decltype(comparatorLambda)> tileQueue(comparatorLambda); // The tiles that we have queued to be searched
		std::unordered_map<PathfindingNode*, float> gCost; // The gCost of a certain tile (distance from start point to the tile)
		std::unordered_map<PathfindingNode*, PathfindingNode*> tileToParent; // Tile to parent relationship for path regeneration
		
		PathfindingNode agentNode = PathfindingNode(&agentPosition, 0);
		tileQueue.push(&agentNode); // Add the first tile
		gCost[&agentNode] = 0; // Cost of start to start is 0
		while (!tileQueue.empty())
		{
			PathfindingNode* currentNode = tileQueue.top(); // Get the tile with the highest priority (lowest fCost)
			if (*currentNode->position == agentPosition)
				return BuildPath(&agentNode, currentNode, tileToParent);
			
			tileQueue.pop();
			//int posX = currentTile->position.first;
			//int posY = currentTile->position.second;
			int gCostCurrent = gCost[currentNode]; // If it was popped from the queue that means it has a gCost

			// Go through the movement spreads that are available
			//for (int i = 0; i < spider.spiderMoveSpread.size(); i++)
			//{
			//	int directionX = spider.spiderMoveSpread[i].first;
			//	int directionY = spider.spiderMoveSpread[i].second;
				PathfindingNode* nextTile = nullptr;
				if (!nextTile || gCost.count(nextTile) != 0)
					continue;

				//int dx = abs(currentTile->position.first - nextTile->position.first);
				//int dy = abs(currentTile->position.second - nextTile->position.second);
				float gCostNext = gCostCurrent; // Beginning of gCost starts from the current gCost

				gCost[nextTile] = gCostNext; // Store the gCost
				float hCostNext = 0;
				//float hCostNext = ManhattanDistanceHeuristic(*(nextTile), *(ant.tile)); // Get the heuristic cost
				float fCostNext = gCostNext + hCostNext; // f = g + h

				//nextTile->cost = fCostNext; // Store the cost of this tile
				tileQueue.push(nextTile); // Add the new unexplored tile
				tileToParent[nextTile] = currentNode; // tile parent relationship
			//}
		}

		return nullptr;
	}

	PathfindingNode* BuildPath(const PathfindingNode* agentNode, PathfindingNode* pathStart, std::unordered_map<PathfindingNode*, PathfindingNode*>& tileToParent)
	{
		PathfindingNode* currentNode = pathStart; // In this situation it is the destination node
		PathfindingNode* previousNode = nullptr;

		while (currentNode != agentNode)
		{
			previousNode = currentNode;

			// Get the current Node's parent if it is not there then we will never find the node we want so break
			if (tileToParent.find(currentNode) != tileToParent.end())
				currentNode = tileToParent[currentNode];
			else
				return nullptr; // something wrong happened if we let this go it will run infinitely
		}

		return previousNode; // This is the next position that the unit should be moving to
	}

	int ManhattanDistanceHeuristic(const PathfindingNode& node1, const PathfindingNode& node2)
	{
		// Manhattan distance is used for grid distance when we have 4 way movement and no diagonal movement
		int absDx = abs(node1.position->x - node2.position->x);
		int absDy = abs(node1.position->y - node2.position->y);
		return absDx + absDy;
	}

	int ChebyshevDistanceHeuristic(const PathfindingNode& node1, const PathfindingNode& node2)
	{
		// Used when we have 8 way movement
		int absDx = std::abs(node1.position->x - node2.position->x);
		int absDy = std::abs(node1.position->y - node2.position->y);
		return std::max(absDx, absDy);
	}

	// This is probably what we need to do (OBVIOUSLY SQUARED)
	int EuclideanDistanceHeuristic(const PathfindingNode& node1, const PathfindingNode& node2)
	{
		// Using the euclidean distance squared allows us to not use sqrt which is an expensive operation
		int differenceX = node1.position->x - node2.position->x;
		int differenceY = node1.position->y - node2.position->y;;
		return sqrt(differenceX * differenceX + differenceY * differenceY);
	}
}