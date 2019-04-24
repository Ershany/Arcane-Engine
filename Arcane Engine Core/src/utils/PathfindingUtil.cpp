#include "pch.h"
#include "PathfindingUtil.h"

#include <ui/NavmeshPane.h>

namespace arcane
{
	std::vector<PathfindingNode*> PathfindingUtil::s_Path;
	std::vector<TrianglePrim*> PathfindingUtil::s_TrianglesSearched;

	// Comparator lambda for priority queue
	auto comparatorLambda = [](PathfindingNode * node1, PathfindingNode * node2) -> bool {
		return node1->nodeScore > node2->nodeScore;
	};

	PathfindingNode* PathfindingUtil::BFS(const glm::vec3& agentPosition, const glm::vec3& destination, std::vector<TrianglePrim>& navmesh, std::unordered_map<glm::vec3*, std::unordered_set<TrianglePrim*>>& triangleSet)
	{
		TrianglePrim* startingTri = NavigationMesh::GetTriangleFromPoint(agentPosition, navmesh);
		TrianglePrim* destinationTri = NavigationMesh::GetTriangleFromPoint(destination, navmesh);
		if (!startingTri || !destinationTri)
			return nullptr;

		s_TrianglesSearched.clear();

		std::unordered_map<PathfindingNode*, PathfindingNode*> tileToParent; // Tile to parent
		std::deque<PathfindingNode*> currentTiles; // Current tiles
		std::unordered_set<TrianglePrim*> closedList; // Closed list of tiles
		PathfindingNode* agentNode = new PathfindingNode(startingTri, 0);

		currentTiles.push_back(agentNode); // Add the first tile
		closedList.insert(startingTri); // Add to query set
		while (!currentTiles.empty())
		{
			PathfindingNode* currentNode = currentTiles.front();
			s_TrianglesSearched.push_back(currentNode->triangle);

			if (*currentNode->triangle == *destinationTri)
				return BuildPath(agentNode, currentNode, tileToParent);

			currentTiles.pop_front();

			// Get the triangles that are neighbors of the 3 points of the triangle
			for (int i = 0; i < currentNode->triangle->v.size(); ++i)
			{
				if (triangleSet.count(currentNode->triangle->v[i]) == 0)
					continue;

				std::unordered_set<TrianglePrim*>& neighborTris = triangleSet[currentNode->triangle->v[i]];
				for (auto iter = neighborTris.begin(); iter != neighborTris.end(); ++iter)
				{
					TrianglePrim* neighborTri = *iter;
					if (closedList.count(neighborTri) != 0)
						continue;

					PathfindingNode* nextNode = new PathfindingNode(neighborTri, 0);
					currentTiles.push_back(nextNode); // Add the new unexplored tile
					closedList.insert(neighborTri);
					tileToParent[nextNode] = currentNode; // tile parent relationship
				}
			}
		}

		return nullptr;
	}

	PathfindingNode* PathfindingUtil::DFS(const glm::vec3 & agentPosition, const glm::vec3 & destination, std::vector<TrianglePrim> & navmesh, std::unordered_map<glm::vec3*, std::unordered_set<TrianglePrim*>> & triangleSet)
	{
		TrianglePrim* startingTri = NavigationMesh::GetTriangleFromPoint(agentPosition, navmesh);
		TrianglePrim* destinationTri = NavigationMesh::GetTriangleFromPoint(destination, navmesh);
		if (!startingTri || !destinationTri)
			return nullptr;

		s_TrianglesSearched.clear();

		std::unordered_map<PathfindingNode*, PathfindingNode*> tileToParent; // Tile to parent
		std::deque<PathfindingNode*> currentTiles; // Current tiles
		std::unordered_set<TrianglePrim*> closedList; // Closed list of tiles
		PathfindingNode* agentNode = new PathfindingNode(startingTri, 0);

		currentTiles.push_front(agentNode); // Add the first tile
		closedList.insert(startingTri); // Add to query set
		while (!currentTiles.empty())
		{
			PathfindingNode* currentNode = currentTiles.front();
			s_TrianglesSearched.push_back(currentNode->triangle);

			if (*currentNode->triangle == *destinationTri)
				return BuildPath(agentNode, currentNode, tileToParent);

			currentTiles.pop_front();

			// Get the triangles that are neighbors of the 3 points of the triangle
			for (int i = 0; i < currentNode->triangle->v.size(); ++i)
			{
				if (triangleSet.count(currentNode->triangle->v[i]) == 0)
					continue;

				std::unordered_set<TrianglePrim*> & neighborTris = triangleSet[currentNode->triangle->v[i]];
				for (auto iter = neighborTris.begin(); iter != neighborTris.end(); ++iter)
				{
					TrianglePrim* neighborTri = *iter;
					if (closedList.count(neighborTri) != 0)
						continue;

					PathfindingNode* nextNode = new PathfindingNode(neighborTri, 0);
					currentTiles.push_front(nextNode); // Add the new unexplored tile
					closedList.insert(neighborTri);
					tileToParent[nextNode] = currentNode; // tile parent relationship
				}
			}
		}

		return nullptr;
	}

	PathfindingNode* PathfindingUtil::AStar(const glm::vec3 & agentPosition, const glm::vec3 & destination, std::vector<TrianglePrim> & navmesh, std::unordered_map<glm::vec3*, std::unordered_set<TrianglePrim*>> & triangleSet)
	{
		TrianglePrim* startingTri = NavigationMesh::GetTriangleFromPoint(agentPosition, navmesh);
		TrianglePrim* destinationTri = NavigationMesh::GetTriangleFromPoint(destination, navmesh);
		if (!startingTri || !destinationTri)
			return nullptr;

		s_TrianglesSearched.clear();

		std::priority_queue<PathfindingNode*, std::vector<PathfindingNode*>, decltype(comparatorLambda)> tileQueue(comparatorLambda); // The tiles that we have queued to be searched
		std::unordered_map<TrianglePrim*, float> gCost; // The gCost of a certain tile (distance from start point to the tile)
		std::unordered_map<PathfindingNode*, PathfindingNode*> tileToParent; // Tile to parent relationship for path regeneration
		PathfindingNode * agentNode = new PathfindingNode(startingTri, 0);

		// DEALLOCATION???? 
		tileQueue.push(agentNode);
		gCost[startingTri] = 0;
		while (!tileQueue.empty())
		{
			PathfindingNode* currentNode = tileQueue.top();
			s_TrianglesSearched.push_back(currentNode->triangle);

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
					// Heuristic Choice
					float hCostNext = 0.0f;
					if (NavmeshPane::getHeuristicChoice() == HeuristicChoice::Manhattan) {
						hCostNext = ManhattanDistanceHeuristic(centerNeighbor, destination);
					}
					else if (NavmeshPane::getHeuristicChoice() == HeuristicChoice::Euclidean) {
						hCostNext = EuclideanDistanceHeuristic(centerNeighbor, destination);
					}
					else if (NavmeshPane::getHeuristicChoice() == HeuristicChoice::Chebyshev) {
						hCostNext = ChebyshevDistanceHeuristic(centerNeighbor, destination);
					}

					PathfindingNode* nextNode = new PathfindingNode(neighborTri, gCostNext + hCostNext);

					gCost[neighborTri] = gCostNext;
					tileQueue.push(nextNode); // Add the new unexplored tile
					tileToParent[nextNode] = currentNode; // tile parent relationship
				}
			}
		}

		return nullptr;
	}

	PathfindingNode* PathfindingUtil::BestFirstSearch(const glm::vec3& agentPosition, const glm::vec3& destination, std::vector<TrianglePrim>& navmesh, std::unordered_map<glm::vec3*, std::unordered_set<TrianglePrim*>>& triangleSet)
	{
		TrianglePrim* startingTri = NavigationMesh::GetTriangleFromPoint(agentPosition, navmesh);
		TrianglePrim* destinationTri = NavigationMesh::GetTriangleFromPoint(destination, navmesh);
		if (!startingTri || !destinationTri)
			return nullptr;

		s_TrianglesSearched.clear();

		std::priority_queue<PathfindingNode*, std::vector<PathfindingNode*>, decltype(comparatorLambda)> tileQueue(comparatorLambda); // The tiles that we have queued to be searched
		std::unordered_map<TrianglePrim*, float> gCost; // The gCost of a certain tile (distance from start point to the tile)
		std::unordered_map<PathfindingNode*, PathfindingNode*> tileToParent; // Tile to parent relationship for path regeneration
		PathfindingNode * agentNode = new PathfindingNode(startingTri, 0);

		// DEALLOCATION???? 
		tileQueue.push(agentNode);
		gCost[startingTri] = 0;
		while (!tileQueue.empty())
		{
			PathfindingNode* currentNode = tileQueue.top();
			s_TrianglesSearched.push_back(currentNode->triangle);

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

				std::unordered_set<TrianglePrim*> & neighborTris = triangleSet[currentNode->triangle->v[i]];
				for (auto iter = neighborTris.begin(); iter != neighborTris.end(); ++iter)
				{
					TrianglePrim* neighborTri = *iter;
					if (gCost.count(neighborTri) != 0)
						continue;

					glm::vec3 centerNeighbor = FindCenterTriangle(*neighborTri);
					float gCostNext = gCostCurrent + glm::length2(currentTriCenter - centerNeighbor);
					// Heuristic Choice
					float hCostNext = 0.0f;
					if (NavmeshPane::getHeuristicChoice() == HeuristicChoice::Manhattan) {
						hCostNext = ManhattanDistanceHeuristic(centerNeighbor, destination);
					}
					else if (NavmeshPane::getHeuristicChoice() == HeuristicChoice::Euclidean) {
						hCostNext = EuclideanDistanceHeuristic(centerNeighbor, destination);
					}
					else if (NavmeshPane::getHeuristicChoice() == HeuristicChoice::Chebyshev) {
						hCostNext = ChebyshevDistanceHeuristic(centerNeighbor, destination);
					}

					PathfindingNode* nextNode = new PathfindingNode(neighborTri, hCostNext);

					gCost[neighborTri] = gCostNext;
					tileQueue.push(nextNode); // Add the new unexplored tile
					tileToParent[nextNode] = currentNode; // tile parent relationship
				}
			}
		}

		return nullptr;
	}

	int g_NumVerticesInPath;
	int g_NumVerticesInSearch;
	unsigned int g_PathVAO = 0, g_PathVBO = 0;
	unsigned int g_SearchedVAO = 0, g_SearchedVBO = 0;
	PathfindingNode* PathfindingUtil::BuildPath(const PathfindingNode * agentNode, PathfindingNode * pathStart, std::unordered_map<PathfindingNode*, PathfindingNode*> & tileToParent)
	{
		PathfindingNode* currentNode = pathStart; // In this situation it is the destination node
		PathfindingNode* previousNode = nullptr;
		s_Path.clear();

		while (currentNode != agentNode)
		{
			s_Path.push_back(currentNode);
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
		if (g_SearchedVAO != 0 && g_SearchedVBO != 0) {
			glDeleteVertexArrays(1, &g_SearchedVAO);
			glDeleteBuffers(1, &g_SearchedVBO);
		}

		// Create buffers on the GPU to render the data (debug)
		std::vector<glm::vec3> triangleVertices;
		for (int i = 0; i < s_Path.size(); i++) {
			triangleVertices.push_back(*(s_Path[i]->triangle->a) + glm::vec3(0.0f, 1.5f, 0.0f));
			triangleVertices.push_back(*(s_Path[i]->triangle->b) + glm::vec3(0.0f, 1.5f, 0.0f));
			triangleVertices.push_back(*(s_Path[i]->triangle->c) + glm::vec3(0.0f, 1.5f, 0.0f));
		}
		g_NumVerticesInPath = triangleVertices.size();

		std::vector<glm::vec3> searchedVertices;
		for (int i = 0; i < s_TrianglesSearched.size(); i++) {
			searchedVertices.push_back(*s_TrianglesSearched[i]->a);
			searchedVertices.push_back(*s_TrianglesSearched[i]->b);
			searchedVertices.push_back(*s_TrianglesSearched[i]->c);
		}
		g_NumVerticesInSearch = searchedVertices.size();

		glGenVertexArrays(1, &g_PathVAO);
		glGenBuffers(1, &g_PathVBO);
		glBindVertexArray(g_PathVAO);
		glBindBuffer(GL_ARRAY_BUFFER, g_PathVBO);
		glBufferData(GL_ARRAY_BUFFER, triangleVertices.size() * sizeof(glm::vec3), &triangleVertices[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glBindVertexArray(0);

		glGenVertexArrays(1, &g_SearchedVAO);
		glGenBuffers(1, &g_SearchedVBO);
		glBindVertexArray(g_SearchedVAO);
		glBindBuffer(GL_ARRAY_BUFFER, g_SearchedVBO);
		glBufferData(GL_ARRAY_BUFFER, searchedVertices.size() * sizeof(glm::vec3), &searchedVertices[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glBindVertexArray(0);

		// Finally return
		return previousNode; // This is the next position that the unit should be moving to
	}

	void PathfindingUtil::draw(ICamera * camera) {
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
		pathShader->setUniform1f("alpha", 1.0f);

		// Draw our navmesh
		if (g_NumVerticesInPath != 0) {
			glBindVertexArray(g_PathVAO);
			glDrawArrays(GL_TRIANGLES, 0, g_NumVerticesInPath);
		}

		// Draw our search mesh
		pathShader->setUniform1f("alpha", 0.5f);
		pathShader->setUniform3f("colour", glm::vec3(0.5f, 0.0f, 0.5f));
		if (NavmeshPane::getShowSearch() && g_NumVerticesInSearch != 0) {
			glBindVertexArray(g_SearchedVAO);
			glDrawArrays(GL_TRIANGLES, 0, g_NumVerticesInSearch);
		}

		cache->setFaceCull(true);
		cache->setDepthTest(true);
		cache->setBlend(false);
	}

	float PathfindingUtil::ManhattanDistanceHeuristic(const glm::vec3 & node1, const glm::vec3 & node2)
	{
		// Manhattan distance is used for grid distance when we have 4 way movement and no diagonal movement
		int absDx = abs(node1.x - node2.x);
		int absDy = abs(node1.y - node2.y);
		int absDz = abs(node1.z - node2.z);
		return absDx + absDy + absDz;
	}

	float PathfindingUtil::ChebyshevDistanceHeuristic(const glm::vec3 & node1, const glm::vec3 & node2)
	{
		// Used when we have 8 way movement
		int absDx = abs(node1.x - node2.x);
		int absDy = abs(node1.y - node2.y);
		int absDz = abs(node1.z - node2.z);
		return std::max(std::max(absDx, absDy), absDz);
	}

	float PathfindingUtil::EuclideanDistanceHeuristic(const glm::vec3 & node1, const glm::vec3 & node2)
	{
		// Using the euclidean distance squared allows us to not use sqrt which is an expensive operation
		return glm::length2(node1 - node2);
	}

	glm::vec3 PathfindingUtil::FindCenterTriangle(const TrianglePrim & triangle)
	{
		glm::vec3 center = glm::vec3(0, 0, 0);
		for (auto iter = triangle.v.begin(); iter != triangle.v.end(); ++iter)
		{
			center += *(*iter);
		}

		return center * 0.33333333f; // Find the average of the 3 vertices
	}
}