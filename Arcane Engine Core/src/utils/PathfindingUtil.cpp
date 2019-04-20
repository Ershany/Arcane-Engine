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
				if (*currentNode->triangle == *destinationTri)
					return BuildPath(agentNode, currentNode, tileToParent);

				tileQueue.pop();
				int gCostCurrent = gCost[currentNode->triangle];
				glm::vec3 currentTriCenter = FindCenterTriangle(*currentNode->triangle);

				// Get the triangles that are neighbors of the 3 points of the triangle
				for (int i = 0; i < currentNode->triangle->v.size(); ++i)
				{
					if (triangleSet.count(currentNode->triangle->v[i]) == 0)
						continue;

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

		int g_NumVerticesInPath;
		unsigned int g_PathVAO = 0, g_PathVBO = 0;
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


			// Clean up old data on the GPU if it exists
			if (g_PathVAO != 0 && g_PathVBO != 0) {
				glDeleteVertexArrays(1, &g_PathVAO);
				glDeleteBuffers(1, &g_PathVBO);
			}

			// Create buffers on the GPU to render the data (debug)
			std::vector<glm::vec3> triangleVertices;
			for (int i = 0; i < path.size(); i++) {
				triangleVertices.push_back(*(path[i]->triangle->a) + glm::vec3(0.0f, 1.5f, 0.0f));
				triangleVertices.push_back(*(path[i]->triangle->b) + glm::vec3(0.0f, 1.5f, 0.0f));
				triangleVertices.push_back(*(path[i]->triangle->c) + glm::vec3(0.0f, 1.5f, 0.0f));
			}
			g_NumVerticesInPath = triangleVertices.size();

			glGenVertexArrays(1, &g_PathVAO);
			glGenBuffers(1, &g_PathVBO);

			glBindVertexArray(g_PathVAO);
			glBindBuffer(GL_ARRAY_BUFFER, g_PathVBO);
			glBufferData(GL_ARRAY_BUFFER, triangleVertices.size() * sizeof(glm::vec3), &triangleVertices[0], GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

			glBindVertexArray(0);

			// Finally return
			return previousNode; // This is the next position that the unit should be moving to
		}

		void PathfindingUtil::drawPath(ICamera* camera) {
			static Shader* pathShader = ShaderLoader::loadShader("src/shaders/simple.vert", "src/shaders/simple.frag");

			GLCache* cache = GLCache::getInstance();
			cache->switchShader(pathShader);
			cache->setFaceCull(false);
			cache->setDepthTest(false);
			cache->setBlend(true);
			cache->setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			// Setup colour and also view and projection matrices
			pathShader->setUniformMat4("view", camera->getViewMatrix());
			pathShader->setUniformMat4("projection", camera->getProjectionMatrix());
			pathShader->setUniform3f("colour", glm::vec3(1.0f, 0.0f, 0.0f));

			// Draw our navmesh
			if (g_NumVerticesInPath != 0) {
				glBindVertexArray(g_PathVAO);
				glDrawArrays(GL_TRIANGLES, 0, g_NumVerticesInPath);
			}
			cache->setFaceCull(true);
			cache->setDepthTest(true);
			cache->setBlend(false);
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
			glm::vec3 center = glm::vec3(0,0,0);
			for (auto iter = triangle.v.begin(); iter != triangle.v.end(); ++iter)
			{
				center += *(*iter);
			}

			return center * 0.33333333f; // Find the average of the 3 vertices
		}
	}
}