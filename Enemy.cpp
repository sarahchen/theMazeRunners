#include "Enemy.h"
#include <queue>

using namespace std;

void Enemy::spawn(int locX, int locY)
{
	setX(locX);
	setY(locY);
	draw();
}

//draw enemy about center
void Enemy::draw()
{
	int halfsize = 10; //this should probably be a property
	glTranslatef(posCenter.x, posCenter.y, 0);
	//placeholder for now...draw a blue square
	glColor3ub(0, 0, 200);
	glBegin(GL_QUADS);
	glVertex2d(-halfsize, halfsize);
	glVertex2d(-halfsize, halfsize);
	glVertex2d(-halfsize, halfsize);
	glVertex2d(-halfsize, halfsize);
	glEnd;
	//do I need to revert to identity here?
}

//moves enemy to the next position from the movement plan
void Enemy::move()
{
	if (!movementPlan.empty()) {
		posCenter.x = movementPlan.back().first;
		posCenter.y = movementPlan.back().second;
		movementPlan.pop_back();
	}
}

//finds shortest path to the player through the maze, using A*
//mostly borrowed from PS09
void Enemy::findBestPath(Maze theMaze)
{
	//make my own struct to hold coordinate info
	struct Node { 
		double x, y, fCost;
	};
	struct compareNode {
		bool operator()(Node const& n1, Node const& n2) {
			// return "true" if "n1" is ordered after "n2”:
			return n1.fCost > n2.fCost;
		}
	};
	priority_queue<Node, vector<Node>, compareNode> processQueue;

	//will need to build this as unordered maps
	bool isInQueue[theMaze.rowSize][theMaze.colSize];
	int gScore[theMaze.rowSize][theMaze.colSize];
	int parent[theMaze.rowSize][theMaze.colSize]; //array of labels of parents

	for (int i = 1; i <= theMaze.rowSize; i++) {
		for (int j = 1; j <= theMaze.colSize; j++) {
			isInQueue[i][j] = false;
			gScore[i][j] = INT_MAX;
			parent[i][j] = -1;
		}
	}

	//need to get a heuristic function into ViewManager, Model
	Node startNode = { posCenter.x , posCenter.y, theMaze.calcHeuristic(posCenter.x, posCenter.y) };
	gScore[startNode.x][startNode.y] = 0;
	processQueue.push(startNode);
	isInQueue[startNode.x][startNode.y] = true;

	//run the A* search
	Node currNode;
	bool foundGoal = false;
	while (!foundGoal && !processQueue.empty()) {
		currNode = processQueue.top();   // copy the node at front of queue
		processQueue.pop();               // remove node at front of queue
		isInQueue[currNode.x][currNode.y] = false;

		//maybe change this to refer directly to player coords
		if (theMaze.calcHeuristic(currNode.x, currNode.y) == 0) { 
			foundGoal = true;
			break;
		}

		//need to write the function that returns x,y vals of neighbors
		vector<pair<int, int>> neighborCoords = theMaze.getNeighborCoords(currNode.x, currNode.y);

		for (int idx = 0; idx < neighborCoords.size(); idx++) {

			int neighX = neighborCoords[idx].first;
			int neighY = neighborCoords[idx].second;

			int testGscore = gScore[neighX][neighY] + 1; //no unique cell costs, for now

			if (testGscore < gScore[neighX][neighY]) {
				parent[neighX][neighY] = neighX * theMaze.colSize + neighY;
				gScore[neighX][neighY] = testGscore;

				if (!isInQueue[neighX][neighY]) {
					Node testNode = { neighX, neighY, testGscore + theMaze.calcHeuristic(neighX, neighY) };
					processQueue.push(testNode);
					isInQueue[neighX][neighY] = true;
				}
			}
		}
	}

	//if player was found, populate the movement plan
	if (foundGoal) {
		
		movementPlan.push_back(make_pair(currNode.x, currNode.y));
		
		int nextParent = parent[currNode.x][currNode.y];
		while (nextParent > 0) {
			int currX = nextParent / theMaze.colSize;
			int currY = nextParent % theMaze.colSize;

			movementPlan.push_back(make_pair(currX, currY));

			nextParent = parent[currX][currY];
		}
	}
}
